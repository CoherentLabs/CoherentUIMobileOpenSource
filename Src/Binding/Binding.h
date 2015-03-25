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
#include <Binding/Binder.h>
#include <Binding/Property.h>

#include <Binding/StaticAssert.h>

namespace Coherent
{
namespace UI
{
template <typename T>
struct Overload_CoherentBind_For_Your_Type;
}
}

template <typename T>
void CoherentBind(Coherent::UI::Binder*, T*)
{
	// if compilation fails here, either there is no
	// overload of CoherentBind for your type T or it is not seen
	// at the point of instantiation
	COHERENT_STATIC_ASSERT(Coherent::StaticAssertFail<T>::Value, Coherent::UI::Overload_CoherentBind_For_Your_Type<T*>);
}

namespace Coherent
{
namespace UI
{

template <typename Member>
TypeDescription& TypeDescription::Property(const char* name, Member member)
{
	m_Binder->AddProperty(m_Object, MakeProperty(name, member));
	return *this;
}

template <typename Getter, typename Setter>
TypeDescription& TypeDescription::Property(const char* name, Getter getter, Setter setter)
{
	m_Binder->AddProperty(m_Object, MakeProperty(name, getter, setter));
	return *this;
}

template <typename Getter>
TypeDescription& TypeDescription::PropertyReadOnly(const char* name, Getter getter)
{
	m_Binder->AddProperty(m_Object, MakePropertyReadOnly(name, getter));
	return *this;
}

COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, bool);
	
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, signed int);
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, unsigned int);
	
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, signed char);
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, unsigned char);
	
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, float);
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, double);
	
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, const char*);
COHERENTUI_API void COHERENT_CDECL CoherentBindInternal(Binder*, const wchar_t*);

COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, bool&);

COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, signed int&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, unsigned int&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, signed long long int&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, unsigned long long int&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, signed char&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, unsigned char&);

COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, float&);
COHERENTUI_API void COHERENT_CDECL CoherentReadInternal(Binder*, double&);

template <typename T>
void CoherentBindInternal(Binder* binder, T& object)
{
	CoherentBindInternalPointer(binder, object);
}

template <typename T>
void CoherentBindInternal(Binder* binder, const T& object)
{
	CoherentBindInternalPointer(binder, object);
}


template <typename T>
void CoherentBindInternalPointer(Binder* binder, T& object)
{
	binder->BeginType();
	CoherentBind(binder, &object);
	binder->EndType(&object);
}

template <typename T>
void CoherentBindInternalPointer(Binder* binder, const T& object)
{
	binder->BeginType();
	CoherentBind(binder, const_cast<T*>(&object));
	binder->EndType((void*)&object);
}

template <typename T>
void CoherentBindInternalPointer(Binder* binder, T* object)
{
	binder->BeginType();
	CoherentBind(binder, object);
	binder->EndType(object);
}


template <typename T>
void CoherentBindInternalPointer(Binder* binder, const T* object)
{
	binder->BeginType();
	CoherentBind(binder, const_cast<T*>(object));
	binder->EndType(const_cast<T*>(object));
}

template <typename T>
void CoherentReadInternal(Binder* binder, T& object)
{
	binder->ReadType();
	CoherentBind(binder, &object);
	binder->EndType(&object);
}

}
}
