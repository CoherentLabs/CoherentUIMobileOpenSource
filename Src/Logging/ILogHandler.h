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

#ifdef __clang__
#if defined(__APPLE__) && defined(__clang_major__) && defined(__clang_minor__)
	#if __clang_major__ >= 5
		#define __COH_CL_PV__ =0
	#else
		#define __COH_CL_PV__
	#endif
#else
	#define __COH_CL_PV__
#endif
#else
#define __COH_CL_PV__ =0
#endif

namespace Coherent
{
	namespace Logging
	{
		/// Interface to allow custom logging
		///
		class ILogHandler
		{
		public:
			/// Virtual destructor
			///
			virtual ~ILogHandler() __COH_CL_PV__;
			
			/// Called when a log message has to be written
			/// @param severity the severity of the message
			/// @param message the log message itself
			/// @param length the length of the message
			virtual void WriteLog(Severity severity, const char* message, size_t length) = 0;

			/// Called when an assert is triggered
			/// @param message a message that describes the reason for the assertion
			virtual void Assert(const char* message) = 0;
		};
	}
}
