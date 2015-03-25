/*
Copyright (c) 2012-2015, Coherent Labs AD
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Coherent UI nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "precompiled.h"

#include <iOS/Binding/BlobFromJS.h>

#include <Binding/Blob/ToBlob.h>

namespace Coherent
{
namespace UI
{
namespace Detail
{
std::pair<size_t, bool> URIDecodeInplace(char* input, size_t length)
{
	char* current = input;
	char* end = input + length;
	char* output = input;
	while (current != end)
	{
		if (*current != '%')
		{
			*output++ = *current;
		}
		else
		{
			if (current + 2 >= end)
			{
				return std::make_pair(current - input, false);
			}
			int value = *++current;
			const int high = (value <= '9')? value - '0' : 10 + value - 'A';
			value = *++current;
			const int low = (value <= '9')? value - '0' : 10 + value - 'A';
			*output++ = (char)((high << 4) | low);
		}
		++current;
	}
	*output = '\0';
	return std::make_pair(output - input, true);
}

class JSONParser
{
public:
	JSONParser(const char* json, size_t length)
		: m_Json(json)
		, m_End(json + length)
		, m_ErrorAt(nullptr)
		, m_ErrorMessage(nullptr)
	{
	}

	bool ParseValue(Coherent::Blob::BlobOStream& blob)
	{
		if (*m_Json == '{')
		{
			return ParseObject(blob);
		}
		else if (*m_Json == '[')
		{
			return ParseArray(blob);
		}
		else if (*m_Json == '"')
		{
			return ParseString(blob);
		}
		else if ((*m_Json >= '0' && *m_Json <= '9') || *m_Json == '-')
		{
			return ParseNumber(blob);
		}
		else if (*m_Json == 't')
		{
			if (m_Json + 3 < m_End && m_Json[1] == 'r' && m_Json[2] == 'u' && m_Json[3] == 'e')
			{
				CoherentToBlob(true, blob);
				m_Json += 4;
				return true;
			}
		}
		else if (*m_Json == 'f')
		{
			if (m_Json + 4 < m_End && m_Json[1] == 'a' && m_Json[2] == 'l' && m_Json[3] == 's' && m_Json[4] == 'e')
			{
				CoherentToBlob(false, blob);
				m_Json += 5;
				return true;
			}
		}
		else if (*m_Json == 'n')
		{
			if (m_Json + 2 < m_End && m_Json[1] == 'i' && m_Json[2] == 'l')
			{
				return SetError("nil is not supported");
			}
		}

		return SetError("Unexpected symbol");
	}

	const char* GetErrorStart() const
	{
		return m_ErrorAt;
	}
	const char* GetErrorMessage() const
	{
		return m_ErrorMessage;
	}

private:
	const char* m_Json;
	const char* m_End;
	const char* m_ErrorAt;
	const char* m_ErrorMessage;
	std::string m_StringValue;

	bool SetError(const char* error)
	{
		m_ErrorMessage = error;
		m_ErrorAt = m_Json;
		m_Json = m_End;

		return false;
	}

		enum UTF16Constants
	{
		HighSurrogateStart	= 0xD800,
		HighSurrogateEnd	= 0xDBFF,
		LowSurrogateStart	= 0xDC00,
		LowSurrogateEnd		= 0xDFFF,
		HalfBase			= 0x0010000,
		HalfShift			= 10,
		OneByte				= 0x80,
		TwoBytes			= 0x800,
		ThreeBytes			= 0x10000,
		FourBytes			= 0x110000,
		ReplacementChar		= 0x0000FFFD, // represents character whose value is unknown or could not be represented in Unicode
		Utf16Error			= 0xFFFFFFFF,
	};

	// return Utf16Error on error
	static int ReadUTF16(const char* data, const char* end)
	{
		if (data + 4 > end)
		{
			return Utf16Error;
		}
		int result = 0;
		for (int i = 0; i < 4; ++i)
		{
			result <<= 4;
			if (data[i] >= '0' && data[i] <= '9')
			{
				result += data[i] - '0';
			}
			else if (data[i] >= 'a' && data[i] <= 'f')
			{
				result += data[i] - 'a' + 10;
			}
			else if (data[i] >= 'A' && data[i] <= 'F')
			{
				result += data[i] - 'A' + 10;
			}
			else
			{
				return Utf16Error;
			}
		}
		return result;
	}

	bool ParseUnicodeCharacters()
	{
		int unicode = ReadUTF16(m_Json, m_End);
		if (unicode == Utf16Error)
		{
			return SetError("could not read utf16 character");
		}
		m_Json += 4;

		if (unicode >= HighSurrogateStart && unicode <= HighSurrogateEnd) // surrogate high symbol
		{
			if (m_Json[0] == '\\' && (m_Json[1] == 'u' || m_Json[1] == 'U'))
			{
				m_Json += 2; // skip the '\u'
				int utf16low = ReadUTF16(m_Json, m_End);
				if (utf16low == Utf16Error)
				{
					return SetError("could not read utf16 character");
				}
				m_Json += 4;
				if (utf16low >= LowSurrogateStart && utf16low <= LowSurrogateEnd)
				{
					unicode = ((unicode - HighSurrogateStart) << HalfShift) + (utf16low - LowSurrogateStart) + HalfBase;
				}
				else
				{
					return SetError("bad utf16 surrogate pair");
				}
			}
			else
			{
				return SetError("bad utf16 surrogate pair");
			}
		}

		int bytesToWrite;
		if (unicode < OneByte)
		{
			bytesToWrite = 1;
		}
		else if (unicode < TwoBytes)
		{
			bytesToWrite = 2;
		}
		else if (unicode < ThreeBytes)
		{
			bytesToWrite = 3;
		}
		else if (unicode < FourBytes)
		{
			bytesToWrite = 4;
		}
		else
		{
			bytesToWrite = 3;
			unicode = ReplacementChar;
		}

		char buffer[4] = { 0, 0, 0, 0 };
		const unsigned char firstByteMark[4] = { 0x00, 0xC0, 0xE0, 0xF0 };

		const unsigned int byteMask = 0xBF;
		const unsigned int byteMark = 0x80;

		switch (bytesToWrite)
		{
			case 4: buffer[3] = (unsigned char)((unicode | byteMark) & byteMask); unicode >>= 6;
			case 3: buffer[2] = (unsigned char)((unicode | byteMark) & byteMask); unicode >>= 6;
			case 2: buffer[1] = (unsigned char)((unicode | byteMark) & byteMask); unicode >>= 6;
			case 1: buffer[0] = (unsigned char) (unicode | firstByteMark[bytesToWrite - 1]);
		}

		m_StringValue.append(buffer, bytesToWrite);

		return true;
	}

	bool ParseString(Coherent::Blob::BlobOStream& blob)
	{
		m_StringValue.clear();
		char current = *++m_Json; // skip "
		while (current != '"' && current)
		{
			if (current == '\\')
			{
				++m_Json;
				switch (*m_Json)
				{
				case '"':
				case '/':
				case '\\':
					m_StringValue.push_back(*m_Json);
					break;

				case 'b': m_StringValue.push_back('\b'); break;
				case 'f': m_StringValue.push_back('\f'); break;
				case 'n': m_StringValue.push_back('\n'); break;
				case 'r': m_StringValue.push_back('\r'); break;
				case 't': m_StringValue.push_back('\t'); break;

				case 'u':
				case 'U':
					++m_Json; // skip the u
					if (!ParseUnicodeCharacters())
					{
						return false;
					}
					--m_Json; // it is incremented below
					break;
				default:
					return SetError("unexpected symbol");
					break;
				}
			}
			else
			{
				m_StringValue.push_back(current);
			}
			current = *++m_Json;
		}
		CoherentToBlob(m_StringValue, blob);
		if (*m_Json != '"')
		{
			return SetError("expected '\"'");
		}
		++m_Json; // skip "
		return true;
	}

	bool ParseNumber(Coherent::Blob::BlobOStream& blob)
	{
		char* numberEnd = nullptr;
		const char* current = m_Json;
		char symbol = *current;
		while (symbol != ',' && symbol != '.' && symbol != 'E' && symbol != 'e' && symbol)
		{
			symbol = *++current;
		}
		if (symbol == ',' || !symbol)
		{
			if (*m_Json != '-')
			{
				unsigned value = strtoul(m_Json, &numberEnd, 10);
				CoherentToBlob(value, blob);
			}
			else
			{
				int value = strtol(m_Json, &numberEnd, 10);
				CoherentToBlob(value, blob);
			}
		}
		else
		{
			double value = strtod(m_Json, &numberEnd);
			CoherentToBlob(value, blob);
		}
		symbol = *numberEnd;
		if (!(symbol == ',' || symbol == ']' || symbol == '}' || symbol == '\0'))
		{
			SetError("invalid number");
		}
		m_Json = numberEnd;

		return true;
	}

	bool ParseArray(Coherent::Blob::BlobOStream& blob)
	{
		Coherent::Blob::SequenceWriter object(blob);
		object.BeginSequence(Coherent::Blob::BT_Array);
		int elements = 0;
		++m_Json; // skip [
		while (m_Json && *m_Json != ']' && *m_Json)
		{
			++elements;
			if (ParseValue(blob) && *m_Json == ',')
			{
				++m_Json;
			}
			else if (*m_Json != ']')
			{
				return SetError("expected ']'");
			}
		}
		object.SetSize(elements);
		object.EndSequence();
		if (*m_Json != ']')
		{
			return SetError("expected ']'");
		}
		++m_Json; // skip ]
		return true;
	}

	bool ParseObject(Coherent::Blob::BlobOStream& blob)
	{
		Coherent::Blob::SequenceWriter object(blob);
		object.BeginSequence(Coherent::Blob::BT_Object);
		int elements = 0;
		++m_Json; // skip {
		while (*m_Json != '}' && *m_Json)
		{
			if (*m_Json != '"')
			{
				return SetError("expected '\"'");
			}
			++elements;
			if (ParseString(blob) && *m_Json == ':')
			{
				++m_Json;
			}
			else
			{
				return SetError("expected ':'");
			}
			if (ParseValue(blob) && *m_Json == ',')
			{
				++m_Json;
			}
			else if (*m_Json != '}')
			{
				return SetError("expected '}'");
			}
		}
		object.SetSize(elements);
		object.EndSequence();
		if (*m_Json != '}')
		{
			return SetError("expected '}'");
		}
		++m_Json; // skip }
		return true;
	}
};

bool JSONtoBlob(const char* json, size_t length, Coherent::Blob::BlobOStream& blob)
{
	JSONParser parser(json, length);
	return parser.ParseValue(blob);
}


}
}
}
