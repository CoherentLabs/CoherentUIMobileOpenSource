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

#include <Binding/Blob/Blob.h>

namespace Coherent
{
namespace Blob
{
	const Value* GetNextValue(const Value* value)
	{
		return reinterpret_cast<const Value*>(reinterpret_cast<const char*>(value) + value->GetSize());
	}

	template <typename T>
	T& Value::As()
	{
		return *reinterpret_cast<T*>(m_Storage);
	}

	template Null& Value::As<Null>();
	template Boolean& Value::As<Boolean>();
	template Integer& Value::As<Integer>();
	template UInteger& Value::As<UInteger>();
	template Number& Value::As<Number>();
	template String& Value::As<String>();
	template Array& Value::As<Array>();
	template Object& Value::As<Object>();
	template Integer64& Value::As<Integer64>();
	template WString& Value::As<WString>();
	template Byte& Value::As<Byte>();
	template UByte& Value::As<UByte>();

	template <typename T>
	const T& Value::As() const
	{
		return *reinterpret_cast<const T*>(m_Storage);
	}
	
	template const Null& Value::As<Null>() const;
	template const Boolean& Value::As<Boolean>() const;
	template const Integer& Value::As<Integer>() const;
	template const UInteger& Value::As<UInteger>() const;
	template const Number& Value::As<Number>() const;
	template const String& Value::As<String>() const;
	template const Array& Value::As<Array>() const;
	template const Object& Value::As<Object>() const;
	template const Integer64& Value::As<Integer64>() const;
	template const WString& Value::As<WString>() const;
	template const Byte& Value::As<Byte>() const;
	template const UByte& Value::As<UByte>() const;

	struct SizeGetter
	{
		typedef size_t ResultType;

		template <typename T>
		ResultType Invoke(T& value) const
		{
			return value.GetSizeInBytes();
		}
	};

	size_t Value::GetSize() const
	{
		return sizeof(AlignedType) + Invoke(SizeGetter());
	}

	Array::ConstIterator::ConstIterator(const Value* value) : m_Value(value)
	{

	}

	Array::ConstIterator::~ConstIterator()
	{

	}

	const Value& Array::ConstIterator::operator*() const
	{
		return *m_Value;
	}

	const Value* Array::ConstIterator::operator->() const
	{
		return m_Value;
	}

	Array::ConstIterator& Array::ConstIterator::operator++()
	{
		m_Value = GetNextValue(m_Value);
		return *this;
	}

	bool Array::ConstIterator::operator==(const ConstIterator& rhs) const
	{
		return m_Value == rhs.m_Value;
	}

	bool Array::ConstIterator::operator!=(const ConstIterator& rhs) const
	{
		return m_Value != rhs.m_Value;
	}

	Array::ConstIterator Array::Begin() const
	{
		return ConstIterator(reinterpret_cast<const Value*>(m_Storage));
	}

	Array::ConstIterator Array::End() const
	{
		return ConstIterator(reinterpret_cast<const Value*>(m_Storage + m_Size - 2 * sizeof(AlignedType)));
	}

	const Value* Object::EndOfStorage() const
	{
		return reinterpret_cast<const Value*>(m_Storage + m_Size - 2 * sizeof(AlignedType));
	}

	Object::ConstIterator Object::Begin() const
	{
		return ConstIterator(reinterpret_cast<const Value*>(m_Storage), EndOfStorage());
	}

	Object::ConstIterator Object::End() const
	{
		const Value* end = EndOfStorage();
		return ConstIterator(end, end);
	}


	Object::ConstIterator::ConstIterator(const Value* begin, const Value* end)
		: m_Value(begin, nullptr)
		, m_End(end)
	{
		if (m_Value.first != m_End)
		{
			m_Value.second = GetNextValue(m_Value.first);
		}
	}

	Object::ConstIterator::~ConstIterator()
	{
	}


	Object::ConstIterator& Object::ConstIterator::operator++()
	{
		m_Value.first = GetNextValue(m_Value.second);
		if (m_Value.first != m_End)
		{
			m_Value.second = GetNextValue(m_Value.first);
		}
		return *this;
	}

	const Object::ConstIterator::value_type& Object::ConstIterator::operator*() const
	{
		return m_Value;
	}

	const Object::ConstIterator::value_type* Object::ConstIterator::operator->() const
	{
		return &m_Value;
	}

	bool Object::ConstIterator::operator==(const Object::ConstIterator& rhs) const
	{
		return m_Value.first == rhs.m_Value.first && m_End == rhs.m_End;
	}

	bool Object::ConstIterator::operator!=(const Object::ConstIterator& rhs) const
	{
		return m_Value.first != rhs.m_Value.first || m_End != rhs.m_End;
	}



}
}
