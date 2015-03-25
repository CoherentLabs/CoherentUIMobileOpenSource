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

#include <Android/jni/UnityLogHandlerDecorator.h>

#ifdef COHERENT_UNITY3D_BUILD

namespace Coherent
{
namespace UI
{
UnityLogHandlerDecorator* UnityLogHandlerDecorator::s_Instance;

UnityLogHandlerDecorator::~UnityLogHandlerDecorator()
{
	s_Instance = nullptr;
	if (m_Own)
	{
		delete m_Handler;
	}
}

void UnityLogHandlerDecorator::WriteLog(Logging::Severity severity, const char* message, size_t length)
{
	if (this_thread::get_id() == m_LogThread)
	{
		m_Handler->WriteLog(severity, message, length);
	}
	else
	{
		CriticalSectionLock lock(m_CriticalSection);
		m_Messages.push(LogEvent(severity, message, length));
	}
}

void UnityLogHandlerDecorator::Assert(const char* message)
{
	if (this_thread::get_id() == m_LogThread)
	{
		m_Handler->Assert(message);
	}
	else
	{
		CriticalSectionLock lock(m_CriticalSection);
		m_Messages.push(LogEvent(message));
	}
}

void UnityLogHandlerDecorator::FetchLogMessages()
{
	s_Instance->FetchLogMessagesInstance();
}

void UnityLogHandlerDecorator::FetchLogMessagesInstance()
{
	assert(m_LogThread == this_thread::get_id());

	if (!m_Messages.empty())
	{
		CriticalSectionLock lock(m_CriticalSection);
		while (!m_Messages.empty())
		{
			auto& message = m_Messages.front();
			if (message.IsLogMessage)
			{
				m_Handler->WriteLog(message.Severity, message.Message.c_str(), message.Message.size());
			}
			else
			{
				m_Handler->Assert(message.Message.c_str());
			}
			m_Messages.pop();
		}
	}
}

UnityLogHandlerDecorator::LogEvent::LogEvent(Logging::Severity severity, const char* message, size_t length)
	: Severity(severity)
	, Message(message, length)
	, IsLogMessage(true)
{
}

UnityLogHandlerDecorator::LogEvent::LogEvent(const char* message)
	: Severity(Logging::AssertFailure)
	, Message(message)
	, IsLogMessage(false)
{
}

}
}

#endif // COHERENT_UNITY3D_BUILD
