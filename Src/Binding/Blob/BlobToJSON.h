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
#pragma once

namespace Coherent
{
namespace Blob
{
template <typename Derived, typename Stream, typename Utf16Converter>
class BlobToJSON : private Utf16Converter
{
public:
	typedef void ResultType;

	BlobToJSON(Stream& stream)
		: m_Stream(stream)
	{

	}

	template<typename Char, typename StringType>
	void ConvertStringToJSONNoQuotes(const StringType& stringBlob) const
	{
		const Char* string = stringBlob.GetBuffer();
		const Char* end = string + stringBlob.GetLength();

		ConvertStringBufferToJSONNoQuotes(string, end);
	}

	template <typename Char>
	void ConvertStringBufferToJSONNoQuotes(const Char* begin, const Char* end) const
	{
		while (begin != end)
		{
			const Char c = *begin++;
			switch (c)
			{
			case '"' : m_Stream << "\\\""; break;
			case '/' : m_Stream << "\\/";  break;
			case '\\': m_Stream << "\\\\"; break;
			case '\b': m_Stream << "\\b"; break;
			case '\f': m_Stream << "\\f"; break;
			case '\n': m_Stream << "\\n"; break;
			case '\r': m_Stream << "\\r"; break;
			case '\t': m_Stream << "\\t"; break;
			default:
				if (c >= 32 && c <= 126 && c != '<' && c != '>')
				{
					m_Stream.put(static_cast<typename Stream::char_type>(c));
				}
				else
				{
					begin += this->EncodeUtf16(begin - 1, end - begin + 1, m_Stream);
				}
			}
		}
	}

	template <typename T>
	void Invoke(const T& value) const
	{
		m_Stream << value.GetValue();
	}

	void Invoke(const Coherent::Blob::Null&) const
	{
		m_Stream << "null";
	}

	void Invoke(const Coherent::Blob::Number& value) const
	{
		m_Stream << std::fixed << value.GetValue();
	}

	void Invoke(const Coherent::Blob::Boolean& value) const
	{
		m_Stream << ((value.GetValue())? "true" : "false");
	}

	void Invoke(const Coherent::Blob::String& stringBlob) const
	{
		m_Stream.put('"');
		ConvertStringToJSONNoQuotes<char>(stringBlob);
		m_Stream.put('"');
	}

	void Invoke(const Coherent::Blob::WString& stringBlob) const
	{
		m_Stream.put('"');
		ConvertStringToJSONNoQuotes<wchar_t>(stringBlob);
		m_Stream.put('"');
	}

	void Invoke(const Coherent::Blob::Array& array) const
	{
		auto it = array.Begin();
		auto end = array.End();

		m_Stream << '[';

		if (it != end)
		{
			it->Invoke(This());
			++it;
		}

		while (it != end)
		{
			m_Stream << ',';
			it->Invoke(This());
			++it;
		}

		m_Stream << ']';
	}

	void Invoke(const Coherent::Blob::Object& object) const
	{
		m_Stream << '{';
		auto it = object.Begin();
		auto end = object.End();

		if (it != end)
		{
			it->first->Invoke(This());
			m_Stream << ':';
			it->second->Invoke(This());
			++it;
		}

		while (it != end)
		{
			m_Stream << ',';
			it->first->Invoke(This());
			m_Stream << ':';
			it->second->Invoke(This());
			++it;
		}

		m_Stream << '}';
	}

	const Derived& This() const
	{
		return static_cast<const Derived&>(*this);
	}

	Stream& m_Stream;
};

}
}
