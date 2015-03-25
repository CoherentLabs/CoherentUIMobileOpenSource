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

#include <Binding/Binder.h>
#include <Binding/Array.h>

namespace Coherent
{
namespace UI
{
class Binder;

class Property
{
public:
	Property(const char* name) 
		: m_Name(name)
	{
	}

	virtual ~Property()
	{
	}

	const char* GetName() const
	{
		return m_Name;
	}

	virtual void* Bind(Binder* binder, void*) const = 0;
	virtual void* Read(Binder* binder, void*) const = 0;

	virtual Property* Clone() const = 0;

private:
	const char* m_Name;
};

template <typename T>
class FieldProperty;

template <typename Class, typename PropertyType>
class FieldProperty<PropertyType (Class::*)> : public Property
{
public:
	typedef PropertyType (Class::* Pointer);
	FieldProperty(const char* name, Pointer member)
		: Property(name)
		, m_Member(member)
	{
	}

	FieldProperty(const FieldProperty& o)
		: Property(o)
		, m_Member(o.m_Member)
	{
	}

	virtual void* Bind(Binder* binder, void* object) const
	{
		binder->PropertyName(GetName());
		CoherentBindInternal(binder, static_cast<Class*>(object)->*m_Member);
		return object;
	}

	virtual void* Read(Binder* binder, void* object) const
	{
		if (binder->ReadProperty(GetName()))
		{
			CoherentReadInternal(binder, static_cast<Class*>(object)->*m_Member);
		}
		return object;
	}

	virtual FieldProperty* Clone() const
	{
		return new FieldProperty(*this);
	}

private:
	Pointer m_Member;
};

// Array version
template <typename Class, typename PropertyType, size_t ArraySize>
class FieldProperty<PropertyType (Class::*)[ArraySize]> : public Property
{
public:
	typedef PropertyType (Class::* Pointer)[ArraySize];
	FieldProperty(const char* name, Pointer member)
		: Property(name)
		, m_Member(member)
	{
	}

	FieldProperty(const FieldProperty& o)
		: Property(o)
		, m_Member(o.m_Member)
	{
	}

	virtual void* Bind(Binder* binder, void* object) const
	{
		binder->PropertyName(GetName());
		CoherentBindInternal<PropertyType, ArraySize>(binder, static_cast<Class*>(object)->*m_Member);
		return object;
	}

	virtual void* Read(Binder* binder, void* object) const
	{
		if (binder->ReadProperty(GetName()))
		{
			CoherentReadInternal<PropertyType, ArraySize>(binder, static_cast<Class*>(object)->*m_Member);
		}
		return object;
	}

	virtual FieldProperty* Clone() const
	{
		return new FieldProperty(*this);
	}

private:
	Pointer m_Member;
};

template <typename Getter>
struct ClassType;

template <typename Class, typename Property>
struct ClassType<Property (Class::*)()>
{
	typedef Class Type;
};

template <typename Class, typename Property>
struct ClassType<Property (Class::*)() const>
{
	typedef Class Type;
};

template <typename Setter>
struct PropertyType;

template <typename ReturnType, typename Class, typename Property>
struct PropertyType<ReturnType (Class::*)(Property)>
{
	typedef Property Type;
};

template <typename Getter, typename Setter>
class ConcreteProperty : public Property
{
public:
	ConcreteProperty(const char* name, Getter getter, Setter setter)
		: Property(name)
		, m_Getter(getter)
		, m_Setter(setter)
	{
	}

	ConcreteProperty(const ConcreteProperty& o)
		: Property(o)
		, m_Getter(o.m_Getter)
		, m_Setter(o.m_Setter)
	{
	}

	virtual void* Bind(Binder* binder, void* object) const
	{
		typedef typename ClassType<Getter>::Type Class;

		binder->PropertyName(GetName());
		CoherentBindInternal(binder, (static_cast<Class*>(object)->*m_Getter)());
		return object;
	}

	virtual void* Read(Binder* binder, void* object) const
	{
		typedef typename ClassType<Getter>::Type Class;
		typedef typename PropertyType<Setter>::Type PropertyType;

		if (m_Setter && binder->ReadProperty(GetName()))
		{
			PropertyType value;
			CoherentReadInternal(binder, value);
			(static_cast<Class*>(object)->*m_Setter)(value);
		}
		return object;
	}

	virtual ConcreteProperty* Clone() const
	{
		return new ConcreteProperty(*this);
	}

private:
	Getter m_Getter;
	Setter m_Setter;
};

template <typename Getter>
class ConcretePropertyReadOnly : public Property
{
public:
	ConcretePropertyReadOnly(const char* name, Getter getter)
		: Property(name)
		, m_Getter(getter)
	{
	}

	ConcretePropertyReadOnly(const ConcretePropertyReadOnly& o)
		: Property(o)
		, m_Getter(o.m_Getter)
	{
	}

	virtual void* Bind(Binder* binder, void* object) const
	{
		typedef typename ClassType<Getter>::Type Class;

		binder->PropertyName(GetName());
		CoherentBindInternal(binder, (static_cast<Class*>(object)->*m_Getter)());
		return object;
	}

	virtual void* Read(Binder*, void* object) const
	{
		return object;
	}

	virtual ConcretePropertyReadOnly* Clone() const
	{
		return new ConcretePropertyReadOnly(*this);
	}

private:
	Getter m_Getter;
};

template <typename P>
FieldProperty<P> MakeProperty(const char* name, P p)
{
	return FieldProperty<P>(name, p);
}

template <typename G, typename S>
ConcreteProperty<G, S> MakeProperty(const char* name, G g, S s)
{
	return ConcreteProperty<G, S>(name, g, s);
}

template <typename G>
ConcretePropertyReadOnly<G> MakePropertyReadOnly(const char* name, G g)
{
	return ConcretePropertyReadOnly<G>(name, g);
}

class AdjustPointer : public Property
{
public:
	AdjustPointer(const char* name, int offset)
		: Property(name)
		, m_Offset(offset)
	{
	}

	AdjustPointer(const AdjustPointer& o)
		: Property(o)
		, m_Offset(o.m_Offset)
	{
	}

	virtual void* Bind(Binder*, void* object) const
	{
		return static_cast<char*>(object) + m_Offset;
	}

	virtual void* Read(Binder*, void* object) const
	{
		return static_cast<char*>(object) + m_Offset;
	}

	virtual AdjustPointer* Clone() const
	{
		return new AdjustPointer(*this);
	}

private:
	int m_Offset;
};

}
}
