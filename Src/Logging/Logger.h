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

#include <Logging/Severity.h>

#include <sstream>
namespace Coherent
{
namespace Logging
{
namespace Detail
{
	class StreamCache;
}

class ILogHandler;

class Logger
{
	public:
		Logger(Severity severity, ILogHandler* handler, bool takeLogHandlerOwnership = false);
		~Logger();

		static Logger& Get();

		template <typename Argument0>
		bool Log(Severity severity, const Argument0& argument0)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << '\0';

				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << argument3
					<< '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3, typename Argument4>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3, const Argument4& argument4)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << argument3
					<< argument4 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3, typename Argument4, typename Argument5>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3, const Argument4& argument4, const Argument5& argument5)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << argument3
					<< argument4 << argument5 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3, typename Argument4, typename Argument5, typename Argument6>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3, const Argument4& argument4, const Argument5& argument5, const Argument6& argument6)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << argument3
					<< argument4 << argument5 << argument6 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3, typename Argument4, typename Argument5, typename Argument6, typename Argument7>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3, const Argument4& argument4, const Argument5& argument5, const Argument6& argument6, const Argument7& argument7)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;
				ss << argument0 << argument1 << argument2 << argument3
					<< argument4 << argument5 << argument6
					<< argument7 << '\0';
				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		template <typename Argument0, typename Argument1, typename Argument2, typename Argument3, typename Argument4, typename Argument5, typename Argument6, typename Argument7, typename Argument8>
		bool Log(Severity severity, const Argument0& argument0, const Argument1& argument1, const Argument2& argument2, const Argument3& argument3, const Argument4& argument4, const Argument5& argument5, const Argument6& argument6, const Argument7& argument7, const Argument8& argument8)
		{
			if (severity >= m_Severity)
			{
				std::stringstream ss;

				ss << argument0 << argument1 << argument2 << argument3
					<< argument4 << argument5 << argument6 << argument7
					<< argument8 << '\0';

				
				std::string message = ss.str();
				WriteLog(severity, message.c_str(), message.size() + 1);
			}

			return true;
		}

		bool Assert(const char* message);

	private:			
		Logger(const Logger&);
		Logger& operator=(const Logger&);

		Severity m_Severity;
		ILogHandler* m_Handler;
		bool m_IsLogHandlerOwned;

		void WriteLog(Severity severity, const char* message, size_t size);
};

void Initialize(Severity severity, ILogHandler* handler, bool takeLogHandlerOwnership = false);
void Uninitialize();
}
}

#define COHERENT_ERROR(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::Error, __VA_ARGS__)
#define COHERENT_ASSERT_FAILURE(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::AssertFailure, __VA_ARGS__)
#define COHERENT_WARNING(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::Warning, __VA_ARGS__)
#define COHERENT_INFO(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::Info, __VA_ARGS__)
#define COHERENT_DEBUG(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::Debug, __VA_ARGS__)
#define COHERENT_TRACE(...) Coherent::Logging::Logger::Get().Log(Coherent::Logging::Trace, __VA_ARGS__)

#define COHERENT_VA_ARGS(...) , ## __VA_ARGS__

#define COHERENT_ASSERT(Expression, Message, ... ) \
	((Expression) || (!COHERENT_ASSERT_FAILURE("Assert failure: ", Message, ' ' COHERENT_VA_ARGS(__VA_ARGS__) ) || Coherent::Logging::Logger::Get().Assert(Message)))

#define COHERENT_ASSERT_RETURN(Expression, Message, ... ) \
	if ((Expression) || (!COHERENT_ASSERT_FAILURE("Assert failure: ", Message, ' ' COHERENT_VA_ARGS(__VA_ARGS__) ) || Coherent::Logging::Logger::Get().Assert(Message))) {} else { return; }

#define COHERENT_ASSERT_RETURN_VALUE(Expression, Value, Message, ... ) \
	if ((Expression) || (!COHERENT_ASSERT_FAILURE("Assert failure: ", Message, ' ' COHERENT_VA_ARGS(__VA_ARGS__) ) || Coherent::Logging::Logger::Get().Assert(Message))) {} else { return (Value); }

