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

#include <Declarations.h>
#include <Binding/ValueType.h>

namespace Coherent
{

namespace Blob
{
	class BlobIStream;
	class Value;
	class Array;
	class Object;
}

namespace UI
{
class Value;

/// A bindable array structure
///
class Array
{
public:
	Array(const Coherent::Blob::Array* values);
	virtual ~Array();

	/// Getter for the size of the array
	///	@return The size of the array
	size_t GetSize() const;
	/// Getter for the value on a specific index
	/// @param index the index to access
	/// @return the value bound on the index
	Value operator[](size_t index) const;

#if defined(COHERENT_DOT_NET)
	const void* GetData() const
	{
		return m_Values;
	}
#endif

	Array(const Array&);
	Array& operator=(const Array&);

private:
	const Coherent::Blob::Array* m_Values;
	size_t* m_Indexes;
};

class PropertyCache;

class ObjectIterator;

class Object
{
public:
	Object(const Coherent::Blob::Object* object);
	virtual ~Object();

	Value operator[](const char* name) const;
	size_t GetSize() const;

	ObjectIterator begin() const;
	ObjectIterator end() const;
	
	Object(const Object&);
	Object& operator=(const Object&);
    
private:
	const Coherent::Blob::Object* m_Object;
	PropertyCache* m_Cache;

	const Coherent::Blob::Value* Find(const char* name) const;
};

class Value
{
public:
	Value(const Coherent::Blob::Value* value);

	/// Virtual destructor of the Value
	///
	virtual ~Value();

	/// Getter for the type encapsulated in the Value
	///
	ValueType GetType() const;

	/// Interpret the value as a boolean
	///
	bool ToBoolean() const;
	
	/// Interpret the value as an integer
	///
	int ToInteger() const;
	
	/// Interpret the value as an unsigned integer
	///
	unsigned int ToUInteger() const;

	/// Interpret the value as a byte
	///
	signed char ToByte() const;
	
	/// Interpret the value as an unsigned byte
	///
	unsigned char ToUByte() const;

	/// Interpret the value as a floating point number
	///
	double ToNumber() const;

	/// Interpret the value as an array
	///
	Array ToArray() const;

	/// Interpret the value as an object
	///
	Object ToObject() const;

	/// Assert that the value is a boolean and return it
	///
	bool ToBooleanChecked() const;
	
	/// Assert that the value is a integer and return it
	///
	int ToIntegerChecked() const;
	
	/// Assert that the value is an unsigned integer and return it
	///
	unsigned int ToUIntegerChecked() const;

	/// Assert that the value is a byte and return it
	///
	signed char ToByteChecked() const;
	
	/// Assert that the value is an unsigned byte and return it
	///
	unsigned char ToUByteChecked() const;
	
	/// Assert that the value is a floating point number and return it
	///
	double ToNumberChecked() const;

	/// Assert that the value is an array and return it
	///
	Array ToArrayChecked() const;

	/// Assert that the value is an object and return it
	///
	Object ToObjectChecked() const;
	
	/// Interpret the value as a string and return it
	///
	template <typename StringType>
	StringType ToString() const
	{
		const char* string = nullptr;
		size_t size = 0;
		ToCharBuffer(string, size);

		return (string)? StringType(string, size) : StringType();
	}

	/// Assert that the value is a string and return it
	///
	template <typename StringType>
	StringType ToStringChecked() const
	{
		const char* string = nullptr;
		size_t size = 0;
		ToCharBufferChecked(string, size);

		return (string)? StringType(string, size) : StringType();
	}

	/// Interpret the value as a string
	///
	bool ToString(char* string, size_t size) const;

	/// Assert that the value is a string and return it
	///
	bool ToStringChecked(char* string, size_t size) const;
	
	bool operator==(const char* rhs);

	void ToString(const char*& string, size_t& size) const;
	static const char* GetTypeName(Coherent::UI::ValueType type);

protected:
	const Blob::Value* m_Value;

private:
	void ToCharBuffer(const char*& string, size_t& size) const;
	void ToCharBufferChecked(const char*& string, size_t& size) const;
};

class ObjectIterator
{
public:
	ObjectIterator(const Coherent::Blob::Value* value, const Coherent::Blob::Value* end);

	Value Key() const;
	const Value& operator*() const;
	const Value* operator->() const;
	ObjectIterator& operator++();

	bool operator==(const ObjectIterator& rhs) const;
	bool operator!=(const ObjectIterator& rhs) const;

private:
	Value m_Key;
	Value m_Value;
	const Coherent::Blob::Value* m_RealValue;
	const Coherent::Blob::Value* m_End;
};

class EventArguments
{
public:
	EventArguments(Coherent::Blob::BlobIStream& stream);

	size_t GetCount() const
	{
		return m_Arguments.GetSize();
	}

	Value operator[](size_t index) const
	{
		return m_Arguments[index];
	}

#if defined(COHERENT_DOT_NET)
	void* GetData() const;
#endif

private:
	Array m_Arguments;
};

}
}
