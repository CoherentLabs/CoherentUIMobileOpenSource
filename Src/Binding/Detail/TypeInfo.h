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

#include <memory>
namespace Coherent
{
namespace UI
{
class Property;
class Binder;

class TypeInfo
{
public:
	TypeInfo()
	{
	}

#if !defined(COHERENT_PLATFORM_IOS)
	TypeInfo(TypeInfo&& o)
		: m_Properties(std::move(o.m_Properties))
	{
	}
#endif
	
	void AddProperty(Property* property)
	{
		m_Properties.push_back(PropertyPtr(property));
	}

	void Bind(Binder* binder, void* object)
	{
		binder->OnTypeBegin();
		binder->TypeName(m_Name.c_str());
		std::for_each(m_Properties.begin(), m_Properties.end(), [binder, &object](const PropertyPtr& property)
		{
			object = property->Bind(binder, object);
		});
		binder->OnTypeEnd();

	}

	void Read(Binder* binder, void* object)
	{
		binder->OnReadTypeBegin();
		if (binder->ReadType(m_Name.c_str()))
		{
			std::for_each(m_Properties.begin(), m_Properties.end(), [binder, &object](const PropertyPtr& property)
			{
				object = property->Read(binder, object);
			});
		}
		binder->OnReadTypeEnd();
	}

	const std::string& GetName() const
	{
		return m_Name;
	}

	void SetName(const char* name)
	{
		m_Name = name;
	}


private:
#if !defined(COHERENT_PLATFORM_IOS)
	TypeInfo(const TypeInfo&);
#endif
	TypeInfo& operator=(const TypeInfo&);

#if defined(COHERENT_PLATFORM_IOS)
	typedef std::shared_ptr<Property> PropertyPtr;
#else
	typedef std::unique_ptr<Property> PropertyPtr;
#endif
	
	std::string m_Name;
	std::vector<PropertyPtr> m_Properties;
};

}
}
