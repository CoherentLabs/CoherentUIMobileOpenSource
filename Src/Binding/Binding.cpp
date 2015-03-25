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

#include <Binding/Binding.h>
#include <Binding/Property.h>
#include <Binding/Detail/TypeInfo.h>

namespace Coherent
{
namespace UI
{

class BinderImpl
{
public:
	BinderImpl()
		: m_CurrentObject(nullptr)
	{
	}

	TypeDescription RegisterType(const char* name, void* object)
	{
		if (!m_CurrentObject)
		{
			return (BeginType(name, object))? TypeDescription(m_Public, object) : TypeDescription(nullptr, nullptr);
		}
		else if (m_CurrentObject != object)
		{
			ptrdiff_t offset = reinterpret_cast<size_t>(object) - reinterpret_cast<size_t>(m_CurrentObject);
			assert(offset == (int)offset);
			m_CurrentType->AddProperty(new AdjustPointer(name, (int)offset));
			m_CurrentObject = object;
		}
		return TypeDescription(m_Public, object);
	}

	void AddProperty(void* object, const Property& property)
	{
		if (m_CurrentObject != object)
		{
			ptrdiff_t offset = reinterpret_cast<size_t>(object) - reinterpret_cast<size_t>(m_CurrentObject);
			assert(offset == (int)offset);
			m_CurrentType->AddProperty(new AdjustPointer(nullptr, (int)offset));
			m_CurrentObject = object;
		}
		m_CurrentType->AddProperty(property.Clone());
	}

	void SetPublic(Binder* binder)
	{
		m_Public = binder;
	}

	bool BeginType(const char* name, void* object)
	{
		// refuse to register a type via nullptr because a dynamic_cast (or some virtual inheritance)
		// in user code will result in a crash
		if (object == nullptr)
		{
			if (m_IsBinding)
			{
				m_Public->BindNull();
			}
			return false;
		}
		// TODO: remove the temporary std::string(name)
#if !defined(COHERENT_COMPILER_GXX)
		auto newType = m_TypeData.emplace(TypeData::value_type(name, TypeInfo()));
#else
		auto newType = m_TypeData.insert(TypeData::value_type(name, TypeInfo()));
#endif
		if (newType.second)
		{
			m_CurrentObject = object;
			m_CurrentType = &newType.first->second;
			m_CurrentType->SetName(name);
			return true;
		}

		if(m_IsBinding)
		{
			newType.first->second.Bind(m_Public, object);
		}
		else
		{
			newType.first->second.Read(m_Public, object);
		}

		return false;
	}

	void SetBinding(bool isBinding)
	{
		m_IsBinding = isBinding;
		m_CurrentObject = nullptr;
		m_CurrentType = nullptr;
	}
	
	void EndType(void* object)
	{
		if (m_CurrentType)
		{
			if (m_IsBinding)
			{
				m_CurrentType->Bind(m_Public, object);
			}
			else
			{
				m_CurrentType->Read(m_Public, object);
			}
			m_CurrentType = nullptr;
		}
	}

private:
	BinderImpl& operator=(const BinderImpl&) = delete;
	
	// TODO: remove this O(log(N)) look up
	typedef std::map<std::string, TypeInfo, std::less<std::string>> TypeData;
	
	TypeData m_TypeData;

	void* m_CurrentObject;
	TypeInfo* m_CurrentType;
	
	Binder* m_Public;

	bool m_IsBinding;
};

TypeDescription::TypeDescription(Binder* binder, void* object)
	: m_Binder(binder)
	, m_Object(object)
{
}

TypeDescription::TypeDescription(const TypeDescription& other)
	: m_Binder(other.m_Binder)
	, m_Object(other.m_Object)
{
}

TypeDescription::~TypeDescription()
{
}

Binder::Binder()
	: m_Impl(new BinderImpl)
{
	m_Impl->SetPublic(this);
}


Binder::~Binder()
{
	delete m_Impl;
}

TypeDescription Binder::RegisterType(const char* name, void* object)
{
	return m_Impl->RegisterType(name, object);
}

TypeDescription Binder::RegisterType(const char* name, const void* object)
{
	return m_Impl->RegisterType(name, const_cast<void*>(object));
}

void Binder::AddProperty(void* object, const Property& property)
{
	m_Impl->AddProperty(object, property);
}

void Binder::BeginType()
{
	m_Impl->SetBinding(true);
}

void Binder::ReadType()
{
	m_Impl->SetBinding(false);
}

void Binder::EndType(void* object)
{
	m_Impl->EndType(object);
}

void CoherentBindInternal(Binder* binder, bool value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, signed int value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, unsigned int value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, signed char value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, unsigned char value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, float value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, double value)
{
	binder->Bind(value);
}

void CoherentBindInternal(Binder* binder, const char* value)
{
	binder->Bind(value);
}


void CoherentBindInternal(Binder* binder, const wchar_t* value)
{
	binder->Bind(value);
}

void CoherentReadInternal(Binder* binder, bool& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, signed int& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, unsigned int& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, signed char& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, unsigned char& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, float& value)
{
	binder->Read(value);
}

void CoherentReadInternal(Binder* binder, double& value)
{
	binder->Read(value);
}

}
}
