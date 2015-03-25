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

#include <Binding/Declarations.h>
#include <Binding/ValueType.h>

namespace Coherent
{
namespace UI
{

class Binder;
/// Helper class for describing types
///
class TypeDescription
{
public:
	TypeDescription(Binder* binder, void* object);
	COHERENTUI_API TypeDescription(const TypeDescription&);
	
	COHERENTUI_API ~TypeDescription();
	
	/// Bind a base class
	/// @param object pointer to the base part of the object
	/// @return reference to the same object to allow chaining calls
	template <typename Base>
	TypeDescription& BaseClass(Base* object)
	{
		CoherentBind(m_Binder, object);
		return *this;
	}
	
	/// Bind a base class
	/// @param object pointer to the base part of the object
	/// @return reference to the same object to allow chaining calls
	template <typename Base>
	TypeDescription& BaseClass(const Base* object)
	{
		CoherentBind(m_Binder, object);
		return *this;
	}
	
	/// Bind a member property
	/// @param name name of the property (this string is NOT copied and must be alive till the view is closed)
	/// @param member pointer to member of the class that the property represents
	/// @return reference to the same object to allow chaining calls
	template <typename Member>
	TypeDescription& Property(const char* name, Member member);
	
	/// Bind a member property
	/// @param name name of the property (this string is NOT copied and must be alive till the view is closed)
	/// @param getter pointer to getter method of the class that the property represents
	/// @param setter pointer to setter method of the class that the property represents
	/// @return reference to the same object to allow chaining calls
	template <typename Getter, typename Setter>
	TypeDescription& Property(const char* name, Getter getter, Setter setter);
	
	/// Bind a read-only member property
	/// @param name name of the property (this string is NOT copied and must be alive till the view is closed)
	/// @param getter pointer to getter method of the class that the property represents
	/// @return reference to the same object to allow chaining calls
	template <typename Getter>
	TypeDescription& PropertyReadOnly(const char* name, Getter getter);
	
private:
	typedef Binder* TypeDescription::* SafeBoolType;
	
public:
	operator SafeBoolType() const
	{
		return m_Binder? &TypeDescription::m_Binder : nullptr;
	}
	
private:
	
	TypeDescription& operator=(const TypeDescription&);
	Binder* m_Binder;
	void* m_Object;
	
};

class BinderImpl;
class Property;
/// Provides binding between C++ and the UI
///
class Binder
{
public:
	COHERENTUI_API Binder();
	COHERENTUI_API virtual ~Binder();
	
	/// Register type in the binding
	/// @param name name for the type
	/// @param object instance of this type
	/// @return TypeDescription object to be used to describe the properties of object
	COHERENTUI_API TypeDescription RegisterType(const char* name, void* object);
	
	/// Register type in the binding
	/// @param name name for the type
	/// @param object instance of this type
	/// @return TypeDescription object to be used to describe the properties of object
	COHERENTUI_API TypeDescription RegisterType(const char* name, const void* object);
	
	COHERENTUI_API void AddProperty(void* object, const Property& property);
	
	COHERENTUI_API void BeginType();
	COHERENTUI_API void ReadType();
	COHERENTUI_API void EndType(void*);
	
	COHERENTUI_API void Bind(void* binderFunction, void* object);
	COHERENTUI_API void Bind(void* binderFunction, const void* object);
	
	COHERENTUI_API void Read(void* binderFunction, void* object);
	
	virtual void TypeName(const char* name) = 0;
	virtual void PropertyName(const char* name) = 0;
	
	virtual void BindNull() = 0;
	
	virtual void Bind(bool value) = 0;
	virtual void Bind(signed int value) = 0;
	virtual void Bind(unsigned int value) = 0;
	virtual void Bind(signed char value) = 0;
	virtual void Bind(unsigned char value) = 0;
	
	virtual void Bind(float value) = 0;
	virtual void Bind(double value) = 0;
	virtual void Bind(const char* value) = 0;
	virtual void Bind(const wchar_t* value) = 0;
	
	/// Start exporting an array
	/// @param size size of the array
	virtual void ArrayBegin(size_t size) = 0;
	
	/// End exporting an array
	///
	virtual void ArrayEnd() = 0;
	
	/// Start exporting a map (object)
	/// @param size size of the map
	virtual void MapBegin(size_t size) = 0;
	
	/// End exporting a map
	///
	virtual void MapEnd() = 0;
	
	virtual void OnTypeBegin() = 0;
	virtual void OnTypeEnd() = 0;
	
	virtual void OnReadTypeBegin() = 0;
	virtual void OnReadTypeEnd() = 0;
	
	virtual void Read(bool& value) = 0;
	virtual void Read(signed int& value) = 0;
	virtual void Read(unsigned int& value) = 0;
	virtual void Read(signed char& value) = 0;
	virtual void Read(unsigned char& value) = 0;
	
	virtual void Read(float& value) = 0;
	virtual void Read(double& value) = 0;
	virtual void Read(const char*& buffer, size_t& size) = 0;
	virtual bool ReadProperty(const char* name) = 0;
	virtual bool ReadType(const char* name) = 0;
	
	/// Start reading an array
	/// @return the size of the array
	virtual size_t ReadArrayBegin() = 0;
	
	/// Read element from an array
	/// @param index the index of the element in the array
	virtual void ReadArrayElement(size_t index) = 0;
	
	/// End reading an array
	///
	virtual void ReadArrayEnd() = 0;
	
	/// Start reading a map (object)
	/// @return the size of the map
	virtual size_t ReadMapBegin() = 0;
	
	/// Read next key-value pair from a map
	///
	virtual void ReadKeyValuePair() = 0;
	
	/// End reading a map
	///
	virtual void ReadMapEnd() = 0;
	
	/// Peek the type of the current value
	/// @return the type of the current value
	virtual ValueType PeekValueType() = 0;
	
private:
	Binder(const Binder&);
	Binder& operator=(const Binder&);
	
	BinderImpl* m_Impl;
	
	friend class TypeDescription;
};

}
}