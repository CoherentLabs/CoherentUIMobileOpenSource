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

#include <Binding/Binding.h>

#include <Binding/EventHandler.h>
#include <Binding/EventArguments.h>
#include <Errors.h>

#if defined(COHERENT_DOT_NET)
#include <Binding/DotNetBinder.h>
#define COHERENT_BINDER_EXTRA_BASE , public DotNetBinder
#else
#define COHERENT_BINDER_EXTRA_BASE
#endif

namespace Coherent
{
namespace UI
{

class EventArguments;
class Value;

class JStoNativeBinder : public Binder, public ArgumentsBinder COHERENT_BINDER_EXTRA_BASE
{
public:
	JStoNativeBinder();
	JStoNativeBinder(JStoNativeBinder&& other);
	virtual void OnReadTypeBegin();
	virtual void OnReadTypeEnd();

	virtual void Read(bool& value);
	virtual void Read(signed int& value);
	virtual void Read(unsigned int& value);
	virtual void Read(signed char& value);
	virtual void Read(unsigned char& value);

	virtual void Read(float& value);
	virtual void Read(double& value);
	virtual void Read(const char*& buffer, size_t& size);
	virtual bool ReadProperty(const char* name);
	virtual bool ReadType(const char* name);

	virtual size_t ReadArrayBegin();
	virtual void ReadArrayElement(size_t index);
	virtual void ReadArrayEnd();
	virtual size_t ReadMapBegin();
	virtual void ReadKeyValuePair();
	virtual void ReadMapEnd();
	virtual ValueType PeekValueType();

	virtual Binder* ResultBegin();
	virtual void ResultEnd();
	virtual void ResultVoid();

	virtual Binder* GetArgument(int argument);

	virtual bool HasError() const;
	virtual const EventArguments* GetArguments();

	virtual void EventBegin(const char* name, size_t arguments) = 0;
	virtual void EventEnd() = 0;

#if !defined(COHERENT_PLATFORM_IOS)
	typedef std::unique_ptr<IEventHandler> EventHandlerPtr;
#else
	typedef std::shared_ptr<IEventHandler> EventHandlerPtr;
#endif
	bool AddEventHandler(const char* event, EventHandlerPtr&& handler);
	bool RemoveEventHandler(BoundEventHandle handle, std::string& name);
	bool AddCallHandler(const char* name, EventHandlerPtr&& handler);
	void RemoveCallHandler(BoundEventHandle handle);

	typedef std::vector<std::string> EventNames;
	EventNames RemoveAllHandlers(const void* object);

	bool Call(const std::string& name, const EventArguments& arguments, double requestId);
	bool Trigger(const std::string& name, const EventArguments& arguments);
	void Ready(int frameId, bool isMainFrame);

	void ReleaseBindings(int frameId, const wchar_t* url, bool isMainFrame);
	void ReleaseBindings(int frameId, const char* url, bool isMainFrame);

protected:
	typedef std::stack<Array> ArrayStack;
	ArrayStack m_Arrays;

	typedef std::pair<Object, ObjectIterator> ObjectData;
	typedef std::stack<ObjectData> ObjectStack;
	ObjectStack m_Objects;

	typedef std::deque<Value> StackContainer;
	std::stack<Value, StackContainer> m_CurrentValue;

	void SetArgumentError(ValueType expectedType, ValueType actualType);
	void SetArgumentError(const std::string& expectedType, const std::string& actualType, int index);

	int m_CurrentArgument;

	const std::string* m_CurrentEventName;
	double m_RequestId;
	
	typedef std::pair<int, std::string> ErrorData;
	typedef std::vector<ErrorData> Errors;
	Errors m_Errors;

	const EventArguments* m_CurrentArguments;

	// JS -> C++ binding
	typedef std::multimap<std::string, EventHandlerPtr, std::less<std::string>> EventHandlers;

	EventHandlers m_EventHandlers;

	typedef std::map<std::string, EventHandlerPtr, std::less<std::string>> CallHandlers;

	CallHandlers m_CallHandlers;

	void CheckForErrors();


#if defined(COHERENT_DOT_NET)
	DotNetBinder::BindingsCallback m_BindingsReleased;
	DotNetBinder::BindingsCallback m_BindingsReady;
	unsigned int m_BinderIdRelease;
	unsigned int m_BinderIdReady;
#endif

// for testing purposes
protected:
	EventNames RemoveAllEventHandlers(const void* target);
private:
	void RemoveAllCallHandlers(const void* target);
	bool RemoveEventHandler(EventHandlers::iterator position, std::string& name);
	void ReleaseBindings(int frameId, bool isMainFrame);
};

	static const char* JSEvt_RegisterForEvent    = "_Register";
	static const char* JSEvt_UnregisterFromEvent = "_Unregister";
	static const char* JSEvt_OnReady    = "_OnReady";
	static const char* JSEvt_OnError    = "_OnError";
	static const char* JSEvt_CallResult = "_Result";


}
}
