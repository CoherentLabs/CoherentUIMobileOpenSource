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
#include <Binding/JStoNativeBinder.h>

#include <Binding/Vector.h>
#include <Binding/Pair.h>
#include <Binding/String.h>

#include <sstream>

namespace Coherent
{
namespace UI
{
	
class ScopedStringPtr
{
public:
	ScopedStringPtr(const std::string*& pointer, const std::string& value)
		: m_Pointer(pointer)
	{
		m_Pointer = &value;
	}

	~ScopedStringPtr()
	{
		m_Pointer = nullptr;
	}
private:
	ScopedStringPtr& operator=(const ScopedStringPtr&) = delete;

	const std::string *& m_Pointer;
};

JStoNativeBinder::JStoNativeBinder()
	: m_CurrentArgument(0)
	, m_CurrentEventName(nullptr)
	, m_RequestId(0.0)
	, m_CurrentArguments(nullptr)
#if defined(COHERENT_DOT_NET)
	, m_BindingsReleased(nullptr)
	, m_BindingsReady(nullptr)
	, m_BinderIdRelease(0)
	, m_BinderIdReady(0)
#endif
{
}

JStoNativeBinder::JStoNativeBinder(JStoNativeBinder&& other)
	: m_Arrays(std::move(other.m_Arrays))
	, m_Objects(std::move(other.m_Objects))
	, m_CurrentValue(std::move(other.m_CurrentValue))
	, m_CurrentArgument(other.m_CurrentArgument)
	, m_CurrentEventName(other.m_CurrentEventName)
	, m_RequestId(other.m_RequestId)
	, m_Errors(std::move(other.m_Errors))
	, m_EventHandlers(std::move(m_EventHandlers))
	, m_CallHandlers(std::move(m_CallHandlers))
#if defined(COHERENT_DOT_NET)
	, m_BindingsReleased(other.m_BindingsReleased)
	, m_BindingsReady(other.m_BindingsReady)
	, m_BinderIdRelease(other.m_BinderIdRelease)
	, m_BinderIdReady(other.m_BinderIdReady)
#endif
{
}

void JStoNativeBinder::Read(bool& value)
{
	auto jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_Boolean)
	{
		value = m_CurrentValue.top().ToBoolean();
	}
	else
	{
		SetArgumentError(VT_Boolean, jsValue.GetType());
	}
	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(signed int& value)
{
	Value jsValue = m_CurrentValue.top();
	auto type = jsValue.GetType();
	if (type == VT_UInteger)
	{
		value = (signed int)(jsValue.ToUInteger());
	}
	else if (type == VT_Number)
	{
		double number = jsValue.ToNumber();
		value = (signed int)(number);
	}
	else
	{
		SetArgumentError(VT_Integer, type);
	}
	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(unsigned int& value)
{
	Value jsValue = m_CurrentValue.top();
	auto type = jsValue.GetType();
	if (type == VT_UInteger)
	{
		value = jsValue.ToUInteger();
	}
	else if (type == VT_Number)
	{
		double number = jsValue.ToNumber();
		value = (unsigned int)(number);
	}
	else
	{
		SetArgumentError(VT_UInteger, type);
	}

	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(signed char& value)
{
	Value jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_Byte)
	{
		value = m_CurrentValue.top().ToByte();
	}
	else
	{
		SetArgumentError(VT_Byte, jsValue.GetType());
	}
	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(unsigned char& value)
{
	Value jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_UByte)
	{
		value = m_CurrentValue.top().ToUByte();
	}
	else
	{
		SetArgumentError(VT_UByte, jsValue.GetType());
	}
	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(float& value)
{
	double doubleValue;
	JStoNativeBinder::Read(doubleValue);
	value = float(doubleValue);
}

void JStoNativeBinder::Read(double& value)
{
	Value jsValue = m_CurrentValue.top();
	auto type = jsValue.GetType();
	if (type == VT_UInteger)
	{
		value = jsValue.ToUInteger();
	}
	else if (type == VT_Integer)
	{
		value = jsValue.ToInteger();
	}
	else if (type == VT_Number)
	{
		value = jsValue.ToNumber();
	}
	else
	{
		SetArgumentError(VT_Number, type);
	}

	m_CurrentValue.pop();
}

void JStoNativeBinder::Read(const char*& buffer, size_t& size)
{
	Value jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_String)
	{
		jsValue.ToString(buffer, size);
	}
	else
	{
		SetArgumentError(VT_String, jsValue.GetType());
	}

	m_CurrentValue.pop();
}

size_t JStoNativeBinder::ReadArrayBegin()
{
	auto value = m_CurrentValue.top();
	if (value.GetType() == VT_Array)
	{
		Array array = value.ToArray();

		auto size = array.GetSize();
		m_Arrays.push(std::move(array));
		return size;
	}
	else
	{
		SetArgumentError(VT_Array, value.GetType());
	}
	return 0;
}

void JStoNativeBinder::ReadArrayElement(size_t index)
{
	auto& array = m_Arrays.top();
	if (index < array.GetSize())
	{
		m_CurrentValue.push(array[index]);
	}
	else
	{
		m_CurrentValue.push(Value(nullptr));
	}
}

void JStoNativeBinder::ReadArrayEnd()
{
	m_CurrentValue.pop();
	m_Arrays.pop();
}

size_t JStoNativeBinder::ReadMapBegin()
{
	auto value = m_CurrentValue.top();
	if (value.GetType() == VT_Object)
	{
		Object object = m_CurrentValue.top().ToObject();
		auto size = object.GetSize();
		m_Objects.push(ObjectData(std::move(object), object.begin()));

		return size;
	}
	else
	{
		SetArgumentError(VT_Object, value.GetType());
	}
	return 0;
}

void JStoNativeBinder::ReadKeyValuePair()
{
	auto& objectIterator = m_Objects.top();
	m_CurrentValue.push(*objectIterator.second);
	m_CurrentValue.push(objectIterator.second.Key());
	++objectIterator.second;
}

void JStoNativeBinder::ReadMapEnd()
{
	m_CurrentValue.pop();
	m_Objects.pop();
}

ValueType JStoNativeBinder::PeekValueType()
{
	return !m_CurrentValue.empty() ? m_CurrentValue.top().GetType() : VT_Null;
}

Binder* JStoNativeBinder::ResultBegin()
{
	if (m_RequestId != 0 && !HasError())
	{
		EventBegin(JSEvt_CallResult, 2); // request id and result
		Bind(m_RequestId);
		// the result is going to be bound next
		return this;
	} 
	return nullptr;
}

void JStoNativeBinder::ResultEnd()
{
	EventEnd();
}

void JStoNativeBinder::ResultVoid()
{
	if (m_RequestId != 0)
	{
		EventBegin(JSEvt_CallResult, 1);
		Bind(m_RequestId);
		EventEnd();
	}
}

bool JStoNativeBinder::ReadProperty(const char* name)
{
	Value jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_Object)
	{
		Object object = jsValue.ToObject();
		Value value = object[name];
		if (value.GetType() != VT_Null)
		{
			m_CurrentValue.push(value);
			return true;
		}
	}
	return false;
}

bool JStoNativeBinder::ReadType(const char* name)
{
	Value jsValue = m_CurrentValue.top();
	if (jsValue.GetType() == VT_Object)
	{
		Object object = jsValue.ToObject();
		Value value = object["__Type"];

		bool result = value == name;
		if (!result)
		{
			std::string actualType =  (value.GetType() == VT_String)? value.ToString<std::string>() : "Unknown";
			SetArgumentError(name, actualType, m_CurrentArgument);
		}

		return result;
	}
	else
	{
		SetArgumentError(VT_Object, jsValue.GetType());
	}
	return false;
}

void JStoNativeBinder::OnReadTypeBegin()
{
}

void JStoNativeBinder::OnReadTypeEnd()
{
	m_CurrentValue.pop();
}


Binder* JStoNativeBinder::GetArgument(int argument)
{
	m_CurrentArgument = argument;
	m_CurrentValue.push((*m_CurrentArguments)[argument]); 
	return this;
}


void JStoNativeBinder::SetArgumentError(ValueType expectedType, ValueType actualType)
{
	SetArgumentError(Value::GetTypeName(expectedType), Value::GetTypeName(actualType), m_CurrentArgument);
}

void JStoNativeBinder::SetArgumentError(const std::string& expectedType, const std::string& actualType, int index)
{
	std::stringstream stream;
	stream << "Wrong type - expected " << expectedType << ", got " << actualType << " while converting argument " << index;
	if (m_CurrentEventName)
	{
		stream << " for handler " << *m_CurrentEventName;
	}
	auto message = stream.str();
	COHERENT_WARNING("Argument conversion failed: ", message);
	m_Errors.push_back(std::make_pair(SCE_ArgumentType, std::move(message)));
}


bool JStoNativeBinder::AddEventHandler(const char* event, JStoNativeBinder::EventHandlerPtr&& handler)
{
	std::string eventName = event;

	auto position = m_EventHandlers.upper_bound(eventName);

	if (position == m_EventHandlers.begin())
	{
		m_EventHandlers.insert(EventHandlers::value_type(std::move(eventName), std::move(handler)));
		return true;
	}

	position = m_EventHandlers.insert(EventHandlers::value_type(eventName, std::move(handler)));
	--position;
	return (position->first != eventName);
}

bool JStoNativeBinder::RemoveEventHandler(BoundEventHandle handle, std::string& name)
{
	auto handler = static_cast<IEventHandler*>(handle);

	auto i = std::find_if(m_EventHandlers.begin(), m_EventHandlers.end(), [handler](EventHandlers::reference eventHandler)
	{
		return eventHandler.second.get() == handler;
	});

	return RemoveEventHandler(i, name);
}

bool JStoNativeBinder::RemoveEventHandler(EventHandlers::iterator position, std::string& name)
{
	bool removeJSThunk = false;

	auto end = m_EventHandlers.end();

	if (position != end)
	{
		removeJSThunk = true;
		auto next = position;
		++next;

		removeJSThunk = (next == end) || position->first != next->first;

		if (removeJSThunk && position != m_EventHandlers.begin())
		{
			auto previous = position;
			--previous;
			removeJSThunk = position->first != previous->first;
		}

		name = std::move(position->first);

		m_EventHandlers.erase(position);
	}
	return removeJSThunk;
}

JStoNativeBinder::EventNames JStoNativeBinder::RemoveAllEventHandlers(const void* target)
{
	EventNames names;
	auto current = m_EventHandlers.begin();
	auto end = m_EventHandlers.end();
	while (current != end)
	{
		if (current->second->GetTarget() == target)
		{
			std::string name;
			if (RemoveEventHandler(current++, name))
			{
				names.push_back(std::move(name));
			}
		}
		else
		{
			++current;
		}
	}
	return names;
}

bool JStoNativeBinder::AddCallHandler(const char* name, JStoNativeBinder::EventHandlerPtr&& handler)
{
	if (!m_CallHandlers.insert(CallHandlers::value_type(std::string(name), std::move(handler))).second)
	{
		COHERENT_ASSERT(false, "Call handler already registered", "Call handler ", name, " is already registered");
		return false;
	}
	return true;
}

void JStoNativeBinder::RemoveCallHandler(BoundEventHandle handle)
{
	auto handler = static_cast<IEventHandler*>(handle);
	auto i = std::find_if(m_CallHandlers.begin(), m_CallHandlers.end(), [handler](CallHandlers::reference callHandler)
	{
		return callHandler.second.get() == handler;
	});
	if (i != m_CallHandlers.end())
	{
		m_CallHandlers.erase(i);
	}
}

void JStoNativeBinder::RemoveAllCallHandlers(const void* target)
{
	auto current = m_CallHandlers.begin();
	auto end = m_CallHandlers.end();

	while (current != end)
	{
		if (current->second->GetTarget() == target)
		{
			m_CallHandlers.erase(current++);
		}
		else
		{
			++current;
		}
	}
}

JStoNativeBinder::EventNames JStoNativeBinder::RemoveAllHandlers(const void* object)
{
	RemoveAllCallHandlers(object);
	return RemoveAllEventHandlers(object);
}


bool JStoNativeBinder::Call(const std::string& name, const EventArguments& arguments, double requestId)
{
	m_CurrentArguments = &arguments;
	m_RequestId = requestId;
	ScopedStringPtr scopeEventName(m_CurrentEventName, name); 

	auto handler = m_CallHandlers.find(name);
	if (handler != m_CallHandlers.end())
	{
		handler->second->Invoke(this);
		CheckForErrors();
		return true;
	}
	return false;
}


bool JStoNativeBinder::Trigger(const std::string& name, const EventArguments& arguments)
{
	m_CurrentArguments = &arguments;
	m_RequestId = 0;
	ScopedStringPtr scopeEventName(m_CurrentEventName, name); 

	auto handlers = m_EventHandlers.equal_range(name);

	bool executed = handlers.first != handlers.second;
	
	while (handlers.first != handlers.second)
	{
		handlers.first->second->Invoke(this);
		CheckForErrors();
		++handlers.first;
	}

	return executed;
};

void JStoNativeBinder::ReleaseBindings(int frameId, const wchar_t* url, bool isMainFrame)
{
	ReleaseBindings(frameId, isMainFrame);
}
	
void JStoNativeBinder::ReleaseBindings(int frameId, const char* url, bool isMainFrame)
{
	ReleaseBindings(frameId, isMainFrame);
}

void JStoNativeBinder::ReleaseBindings(int frameId, bool isMainFrame)
{
	(void)frameId;
	
	if (isMainFrame)
	{
		m_CallHandlers.clear();
		m_EventHandlers.clear();
	}
	
#if defined (COHERENT_DOT_NET)
	m_BindingsReleased(m_BinderIdRelease, frameId, isMainFrame? 1 : 0);
#endif
}
	
	
	
void JStoNativeBinder::Ready(int frameId, bool isMainFrame)
{
	EventBegin(JSEvt_OnReady, 0);
	EventEnd();

	#if defined (COHERENT_DOT_NET)
		m_BindingsReady(m_BinderIdReady, frameId, isMainFrame? 1 : 0);
	#else
		(void)frameId;
		(void)isMainFrame;
	#endif
}

bool JStoNativeBinder::HasError() const
{
	return !m_Errors.empty();
}

const EventArguments* JStoNativeBinder::GetArguments()
{
	return m_CurrentArguments;
}


void JStoNativeBinder::CheckForErrors()
{
	if (HasError())
	{
		EventBegin(JSEvt_OnError, 2);
		Bind(m_RequestId);
		CoherentBindInternal(this, m_Errors);
		EventEnd();
		Errors().swap(m_Errors);
	}
}

}
}
