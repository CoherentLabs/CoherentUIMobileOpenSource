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

#ifdef COHERENT_UNITY3D_BUILD

#include <Logging/ILogHandler.h>

#include <thread>
#include <Android/jni/CriticalSection.h>
namespace this_thread = std::this_thread;
typedef std::thread::id ThreadID;


namespace Coherent
{
namespace UI
{

class UnityLogHandlerDecorator : public Logging::ILogHandler
{
public:
	UnityLogHandlerDecorator(Logging::ILogHandler* handler, bool takeOwnership)
		: m_LogThread(this_thread::get_id())
		, m_Handler(handler)
		, m_Own(takeOwnership)
	{
		s_Instance = this;
	}

	~UnityLogHandlerDecorator();

	virtual void WriteLog(Logging::Severity severity, const char* message, size_t length) COHERENT_OVERRIDE;

	virtual void Assert(const char* message) COHERENT_OVERRIDE;

	static void FetchLogMessages();

	UnityLogHandlerDecorator& operator=(const UnityLogHandlerDecorator&) = delete;

private:

	struct LogEvent
	{
		LogEvent(Logging::Severity severity, const char* message, size_t length);
		LogEvent(const char* message);
		Logging::Severity Severity;
		std::string Message;
		bool IsLogMessage;
	};

	std::queue<LogEvent> m_Messages;
	PlatformCriticalSection m_CriticalSection;

	ThreadID m_LogThread;
	Logging::ILogHandler* m_Handler;
	bool m_Own;

	static UnityLogHandlerDecorator* s_Instance;
	void FetchLogMessagesInstance();

};

}
}

#endif // COHERENT_UNITY3D_BUILD
