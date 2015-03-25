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

#include <Binding/EventArguments.h>

#include <Binding/Declarations.h>
#include <Binding/Blob/Blob.h>
#include <Binding/Blob/BlobStream.h>

namespace Coherent
{
namespace UI
{


Value::Value(const Coherent::Blob::Value* value)
	: m_Value(value)
{
}

Value::~Value()
{

}

ValueType Value::GetType() const
{
	return m_Value ? ValueType(m_Value->GetType()) : VT_Null;
}

bool Value::ToBoolean() const
{
	return m_Value->As<Coherent::Blob::Boolean>().GetValue();
}

bool Value::ToBooleanChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Boolean, "Value is not of expected type", "expected type ", GetTypeName(VT_Boolean), ", got type ", GetTypeName(type)))
	{
		return ToBoolean();
	}
	return false;
}

int Value::ToInteger() const
{
	return m_Value->As<Coherent::Blob::Integer>().GetValue();
}

int Value::ToIntegerChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Integer, "Value is not of expected type", "expected type ", GetTypeName(VT_Integer), ",  got type ", GetTypeName(type)))
	{
		return ToInteger();
	}
	return 0;
}

void Value::ToCharBufferChecked(const char*& string, size_t& size) const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_String, "Value is not of expected type", "expected type ", GetTypeName(VT_String), ", got type ", GetTypeName(type)))
	{
		ToCharBuffer(string, size);
	}
}

bool Value::ToStringChecked(char* string, size_t size) const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_String, "Value is not of expected type", "expected type ", GetTypeName(VT_String), ", got type ", GetTypeName(type)))
	{
		return ToString(string, size);
	}
	return false;
}

void Value::ToString(const char*& string, size_t& size) const
{
	ToCharBuffer(string, size);
}

Array Value::ToArray() const
{
	return Array(&m_Value->As<Coherent::Blob::Array>());
}

Array Value::ToArrayChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Array, "Value is not of expected type", "expected type ", GetTypeName(VT_Array), ", got type ", GetTypeName(type)))
	{
		return ToArray();
	}
	return Array(nullptr);
}

Object Value::ToObjectChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Object, "Value is not of expected type", "expected type ", GetTypeName(VT_Object), ", got type ", GetTypeName(type)))
	{
		return ToObject();
	}
	return Object(nullptr);
}

Object Value::ToObject() const
{
	return Object(&m_Value->As<Coherent::Blob::Object>());
}

unsigned int Value::ToUInteger() const
{
	return m_Value->As<Coherent::Blob::UInteger>().GetValue();
}

unsigned int Value::ToUIntegerChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_UInteger, "Value is not of expected type", "expected type ", GetTypeName(VT_Number), ",  got type ", GetTypeName(type)))
	{
		return ToUInteger();
	}
	return 0;
}

signed char Value::ToByte() const
{
	return m_Value->As<Coherent::Blob::Byte>().GetValue();
}

signed char Value::ToByteChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Byte, "Value is not of expected type", "expected type ", GetTypeName(VT_Byte), ",  got type ", GetTypeName(type)))
	{
		return ToByte();
	}
	return 0;
}

unsigned char Value::ToUByte() const
{
	return m_Value->As<Coherent::Blob::UByte>().GetValue();
}

unsigned char Value::ToUByteChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_UByte, "Value is not of expected type", "expected type ", GetTypeName(VT_UByte), ",  got type ", GetTypeName(type)))
	{
		return ToUByte();
	}
	return 0;
}

double Value::ToNumber() const
{
	return m_Value->As<Coherent::Blob::Number>().GetValue();
}

double Value::ToNumberChecked() const
{
	auto type = GetType();
	if (COHERENT_ASSERT(type == VT_Number, "Value is not of expected type", "expected type ", GetTypeName(VT_Number), ", got type ", GetTypeName(type)))
	{
		return ToNumber();
	}
	return 0;
}

void Value::ToCharBuffer(const char*& string, size_t& size) const
{
	const Coherent::Blob::String& stringBlob = m_Value->As<Coherent::Blob::String>();
	string = stringBlob.GetBuffer();
	size = stringBlob.GetLength();
}

bool Value::ToString(char* string, size_t size) const
{
	const Coherent::Blob::String& stringBlob = m_Value->As<Coherent::Blob::String>();
	const size_t stringSize = stringBlob.GetLength();
	const bool truncated = stringSize + 1 > size;
	const size_t bytesToCopy = (truncated)? size - 1 : stringSize;
	
	std::memcpy(string, stringBlob.GetBuffer(), bytesToCopy);
	string[bytesToCopy] = '\0';
	
	return !truncated;
}


bool Value::operator==(const char* rhs)
{
	if (GetType() == VT_String)
	{
		const char* buffer = nullptr;
		size_t length = 0;
		ToString(buffer, length);
		return std::strncmp(buffer, rhs, length) == 0;
	}
	return false;
}

const char* Value::GetTypeName(Coherent::UI::ValueType type)
{
	static const char* names[] = {
		"Null",
		"Boolean",
		"Integer",
		"UInteger",
		"Number",
		"String",
		"Array",
		"Object"
	};
	return (type >= VT_Null && type <= VT_Object)? names[type] : "Unknown";
}

Array::Array(const Coherent::Blob::Array* values)
	: m_Values(values)
	, m_Indexes(nullptr)
{
	if (m_Values)
	{
		const size_t size = m_Values->GetSize();

		if (size == 0)
		{
			return;
		}

		auto it = m_Values->Begin();
		m_Indexes = new size_t[size];
		m_Indexes[0] = 0;
		
		for (size_t i = 1; i < size; ++i, ++it)
		{
			m_Indexes[i] = m_Indexes[i - 1] + it->GetSize();
		}
	}
}

Array::Array(const Array& rhs)
	: m_Values(rhs.m_Values)
	, m_Indexes(nullptr)
{
	if (m_Values)
	{
		const size_t size = m_Values->GetSize();
		if(size)
		{
			m_Indexes = new size_t[size];
			::memcpy(m_Indexes, rhs.m_Indexes, sizeof(size_t) * size);
		}
	}
}

Array::~Array()
{
	delete m_Indexes;
}

size_t Array::GetSize() const
{
	return m_Values? m_Values->GetSize() : 0;
}

Value Array::operator[](size_t index) const
{
	if (index < GetSize())
	{
		return Value(reinterpret_cast<const Coherent::Blob::Value*>(static_cast<const char*>(m_Values->GetElementsBuffer()) + m_Indexes[index]));
	}
	return Value(nullptr);
}

class PropertyCache
{
public:
	size_t Find(const std::string& key)
	{
		auto it = m_Offsets.find(key);
		return it != m_Offsets.end()? it->second : size_t(-1);
	}

	void Add(std::string&& key, size_t offset)
	{
		m_Offsets.insert(PropertyOffsets::value_type(std::move(key), offset));
	}

	PropertyCache& operator=(const PropertyCache& other) 
	{
		if(this != &other)
		{
			m_Offsets = other.m_Offsets;
		}
		return *this;
	}

private:
	typedef std::map<std::string, size_t> PropertyOffsets;

	PropertyOffsets m_Offsets;
};

Object::Object(const Blob::Object* object)
	: m_Object(object)
	, m_Cache(nullptr)
{
	if (m_Object)
	{
		m_Cache = new PropertyCache;
	}
}

Object::Object(const Object& other)
	: m_Object(other.m_Object)
{
	if(m_Object)
	{
		m_Cache = new PropertyCache(*other.m_Cache);
	}
}

Object::~Object()
{
	delete m_Cache;
}

const Coherent::Blob::Value* Object::Find(const char* key) const
{
	typedef Coherent::Blob::Object::ConstIterator::value_type KeyValue;
	for (auto it = m_Object->Begin(), end = m_Object->End(); it != end; ++it)
	{
		if (it->first->GetType() != Coherent::Blob::BT_String)
		{
			continue;
		}
		const Coherent::Blob::String& keyString = it->first->As<Coherent::Blob::String>();
		if (!std::strncmp(key, keyString.GetBuffer(), keyString.GetLength()))
		{
			return it->second;
		}
	}
	return nullptr;
}

Value Object::operator[](const char* key) const
{
	std::string keyString(key);

	size_t offset = m_Cache->Find(keyString);
	if (offset != size_t(-1))
	{
		return Value(reinterpret_cast<const Coherent::Blob::Value*>(reinterpret_cast<const char*>(m_Object) + offset));
	}

	const Coherent::Blob::Value* pointer = Find(key);
	if (pointer != nullptr)
	{
		m_Cache->Add(std::move(keyString), reinterpret_cast<const char*>(pointer) - reinterpret_cast<const char*>(m_Object));
	}

	return Value(pointer);
}

size_t Object::GetSize() const
{
	return m_Object->GetSize();
}

ObjectIterator::ObjectIterator(const Blob::Value* value, const Blob::Value* end)
	: m_Key(nullptr)
	, m_Value(nullptr)
	, m_RealValue(value)
	, m_End(end)
{
	if (m_RealValue != m_End)
	{
		m_Key = Value(value);
		m_RealValue = reinterpret_cast<const Blob::Value*>(reinterpret_cast<const char*>(value) + value->GetSize());
		m_Value = Value(m_RealValue);
	}
}

Value ObjectIterator::Key() const
{
	return m_Key;
}

const Value& ObjectIterator::operator*() const
{
	return m_Value;
}

const Value* ObjectIterator::operator->() const
{
	return &m_Value;
}

ObjectIterator& ObjectIterator::operator++()
{
	auto key = reinterpret_cast<const Blob::Value*>(reinterpret_cast<const char*>(m_RealValue) + m_RealValue->GetSize());
	if (key != m_End)
	{
		m_Key = Value(key);
		m_RealValue = reinterpret_cast<const Blob::Value*>(reinterpret_cast<const char*>(key) + key->GetSize());
		m_Value = Value(m_RealValue);
	}
	else
	{
		m_RealValue = key;
	}
	return *this;
}

bool ObjectIterator::operator==(const ObjectIterator& rhs) const
{
	return m_RealValue == rhs.m_RealValue;
}

bool ObjectIterator::operator!=(const ObjectIterator& rhs) const
{
	return m_RealValue != rhs.m_RealValue;
}

ObjectIterator Object::begin() const
{
	return ObjectIterator(m_Object->StartOfStorage(), m_Object->EndOfStorage());
}

ObjectIterator Object::end() const
{
	return ObjectIterator(m_Object->EndOfStorage(), m_Object->EndOfStorage());
}

EventArguments::EventArguments(Coherent::Blob::BlobIStream& stream)
	: m_Arguments(&reinterpret_cast<const Coherent::Blob::Value*>(stream.PeekBytes(sizeof(Coherent::Blob::Value)))->As<Coherent::Blob::Array>())
{
}

#if defined(COHERENT_DOT_NET)
void* EventArguments::GetData() const
{
	return const_cast<void*>(m_Arguments.GetData());
}
#endif

}
}
