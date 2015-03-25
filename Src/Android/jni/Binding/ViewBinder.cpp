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

#include <iOS/Binding/ViewBinder.h>
#include <Binding/Blob/Blob.h>
#include <Binding/Blob/BlobToJSON.h>

#include <utf8cpp/utf8.h>
#include <iomanip>

namespace Coherent
{
namespace UI
{
namespace Detail
{

ViewBinder::ViewBinder()
	: m_InObject(1, false)
{
}
	
ViewBinder::~ViewBinder()
{}

void ViewBinder::EventBegin(const char* name, size_t arguments)
{
	m_JSEvent.seekp(0, std::ios::beg);
	m_JSEvent << "engine._trigger(";
	BindString(name);
	m_Separator = true;
}

void ViewBinder::EventEnd()
{
	m_JSEvent << ");";
	m_JSEvent.put('\0'); // force '\0' so that taking
						// m_JSEvent.str().c_str() gives the current event code
}

void ViewBinder::OnTypeBegin()
{
	ViewBinder::MapBegin(0);
}

void ViewBinder::OnTypeEnd()
{
	ViewBinder::MapEnd();
}

void ViewBinder::TypeName(const char* name)
{
	PropertyName("__Type");
	Bind(name);
}

void ViewBinder::PropertyName(const char* name)
{
	InsertSeparator();
	BindString(name);
	m_Separator = 2;
}
	
void ViewBinder::BindNull()
{
	InsertSeparator();
	m_JSEvent << "null";
}

void ViewBinder::Bind(bool value)
{
	InsertSeparator();
	m_JSEvent << ((value)? "true" : "false");
}

void ViewBinder::Bind(signed int value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(unsigned int value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(signed char value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(unsigned char value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(float value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(double value)
{
	InsertSeparator();
	m_JSEvent << value;
}

void ViewBinder::Bind(const char* value)
{
	InsertSeparator();
	BindString(value);
}

void ViewBinder::ArrayBegin(size_t size)
{
	(void)size;
	InsertSeparator();
	m_JSEvent.put('[');
	m_Separator = false;
	m_InObject.push_back(false);
}
void ViewBinder::ArrayEnd()
{
	m_JSEvent.put(']');
	m_InObject.pop_back();
}

void ViewBinder::MapBegin(size_t size)
{
	(void)size;
	InsertSeparator();
	m_JSEvent.put('{');
	m_Separator = false;
	m_InObject.push_back(true);
}

void ViewBinder::MapEnd()
{
	m_JSEvent.put('}');
	m_InObject.pop_back();
}

void ViewBinder::SetError(ScriptCallErrorType error, const char* message)
{
	m_Errors.push_back(std::make_pair(error, message));
}

void ViewBinder::InsertSeparator()
{
	if (m_Separator)
	{
		if (m_InObject.back() && m_Separator == 2)
		{
			m_JSEvent.put(':');
		}
		else
		{
			m_JSEvent.put(',');
		}
		m_Separator = 3 - m_Separator; // alternating 2,1,2,1...
	}
	else
	{
		m_Separator = 2;
	}
}

class Utf16Converter
{
public:
	template <typename Stream>
	unsigned EncodeUtf16(const char* begin, unsigned length, Stream& stream) const
	{
		length = utf8::internal::sequence_length(begin);
		uint16_t utf16[2];
		auto end = utf8::unchecked::utf8to16(begin, begin + length, utf16);
		for (auto i = utf16; i != end; ++i)
		{
			stream << "\\u" << std::setw(4) << std::setfill(typename Stream::char_type('0')) << std::hex << *i << std::dec;
		}
		return length - 1;
	}

	template <typename Stream>
	unsigned EncodeUtf16(const wchar_t* begin, unsigned length, Stream& stream) const
	{
		length = 0;
		uint16_t utf16[2];
		if (sizeof(wchar_t) == 2)
		{
			utf16[0] = *begin;
			length = 1 + int(utf8::internal::is_surrogate(*begin));
			if (length == 2) {
				utf16[1] = begin[1];
			}
		}
		else
		{
			unsigned int codepoint = *begin;
			if (codepoint <= 0xffff)
			{ 
				utf16[0] = static_cast<uint16_t>(codepoint);
			}
			else
			{
				utf16[0] = static_cast<uint16_t>((codepoint >> 10) + 0xd7c0);
				utf16[1] = static_cast<uint16_t>((codepoint & 0x3ff) | 0xdc00);
			}
		}
		for (unsigned i = 0; i != length; ++i)
		{
			stream << "\\u" << std::setw(4) << std::setfill(typename Stream::char_type('0')) << std::hex << begin[i] << std::dec;
		}
		return length - 1;
	}
};

class BlobToEventData : public Coherent::Blob::BlobToJSON<BlobToEventData, std::stringstream, Utf16Converter>
{
public:
	BlobToEventData(std::stringstream& stream)
		: BlobToJSON(stream)
	{
	}

};

void ViewBinder::BindString(const char* value)
{
	m_JSEvent << '"';
	BlobToEventData converter(m_JSEvent);
	auto length = std::strlen(value);
	converter.ConvertStringBufferToJSONNoQuotes(value, value + length);
	m_JSEvent << '"';
}

void ViewBinder::Bind(const wchar_t* value)
{
	InsertSeparator();

	m_JSEvent << '"';
	BlobToEventData converter(m_JSEvent);
	auto length = std::wcslen(value);
	converter.ConvertStringBufferToJSONNoQuotes(value, value + length);
	m_JSEvent << '"';
}


#if defined(COHERENT_DOT_NET)

void ViewBinder::BlobsToJSON(size_t count, const unsigned char* data, size_t size)
{
	if (count)
	{
		BlobToEventData converter(m_JSEvent);
		auto current = data;
		auto end = data + size;
		while (count-- && current < end)
		{
			m_JSEvent.put(',');
			auto value = reinterpret_cast<const Coherent::Blob::Value*>(current);
			value->Invoke(converter);
			current += value->GetSize();
		}
	}
}

void ViewBinder::RawEvent(const char* name, size_t arguments, size_t size, const unsigned char* data)
{
	EventBegin(name, arguments);
	BlobsToJSON(arguments, data, size);
	EventEnd();
}

void ViewBinder::RawResult(size_t size, const unsigned char* data)
{
	if (ResultBegin())
	{
		BlobsToJSON(1, data, size);
		ResultEnd();
	}
}

void ViewBinder::SetBindingsReleasedCallback(BindingsCallback callback, unsigned int binderId)
{
	m_BindingsReleased = callback;
	m_BinderIdRelease = binderId;
}

void ViewBinder::SetBindingsReadyCallback(BindingsCallback callback, unsigned int binderId)
{
	m_BindingsReady = callback;
	m_BinderIdReady = binderId;
}

#endif

}
}
}
