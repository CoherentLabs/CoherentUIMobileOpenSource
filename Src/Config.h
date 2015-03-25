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

#if defined(__linux__)
	#if defined(__ANDROID__)
		#define COHERENT_PLATFORM_ANDROID 1
	#endif
#elif defined(__APPLE_CC__) || defined(__APPLE_CPP__)
	#include "TargetConditionals.h"
	
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define COHERENT_PLATFORM_IOS 1
	#else
		#error "Could not determine platform"
	#endif
#endif

#if defined(_MSC_VER)
	#define COHERENT_COMPILER_MSC 1
#elif defined(__GNUC__)
	#define COHERENT_COMPILER_GXX 1
	#define COHERENT_GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#elif defined(__clang__)
	#define COHERENT_COMPILER_CLANG 1
#endif

#if defined(COHERENT_PLATFORM_IOS)
	#define COHERENT_EXPORT_SYMBOL
	#define COHERENT_IMPORT_SYMBOL
#endif

#if (COHERENT_COMPILER_MSC && _MSC_VER >= 1600) || (COHERENT_COMPILER_GXX && COHERENT_GCC_VERSION >= 40700)
	#define COHERENT_OVERRIDE override
#else
	#define COHERENT_OVERRIDE
#endif

#define COHERENT_STDCALL 
#define COHERENT_CDECL 

