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

#include <Config.h>

#if !defined(COHERENT_USE_STATIC_ASSERT)
	#if defined(COHERENT_COMPILER_MSC)
		#define COHERENT_USE_STATIC_ASSERT _MSC_VER >= 1600
	#elif defined(COHERENT_COMPILER_GCC)
		#define COHERENT_USE_STATIC_ASSERT (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
	#elif defined(COHERENT_COMPILER_CLANG)
		#define COHERENT_USE_STATIC_ASSERT __has_feature(cxx_static_assert)
	#else
		#define COHERENT_USE_STATIC_ASSERT 0
	#endif
#endif

#if COHERENT_USE_STATIC_ASSERT == 1
	#define COHERENT_STATIC_ASSERT(Expression, Message) static_assert(Expression, #Message)
#else
	namespace Coherent
	{
		template <bool, typename Message>
		struct StaticAssertFailure;

		template <typename Message>
		struct StaticAssertFailure<true, Message>
		{};

		template <int result>
		struct StaticAssertTest
		{};
	}
	#define COHERENT_STATIC_ASSERT(Expression, Message) \
		typedef Coherent::StaticAssertTest<sizeof(Coherent::StaticAssertFailure<Expression, Message>)> \
			__StaticAssertResult ## __LINE__;
#endif

namespace Coherent
{
	template <typename T>
	struct StaticAssertFail
	{
		enum { Value = 0 };
	};
}

#undef COHERENT_USE_STATIC_ASSERT

