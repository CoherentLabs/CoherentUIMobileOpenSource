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

#include <cstddef>
#include <cassert>
#include <cstring> // std::memcpy
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

#include <Declarations.h>

#include <Logging/Logging.h>

#include <CharType.h>

#include <utf8cpp/utf8.h>

#include <jni.h>
#include <android/log.h>

#define COHERENT_ANDROID_LOG_TAG "CoherentAndroidNative"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, COHERENT_ANDROID_LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , COHERENT_ANDROID_LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , COHERENT_ANDROID_LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , COHERENT_ANDROID_LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , COHERENT_ANDROID_LOG_TAG, __VA_ARGS__)
#define ANDR_ASSERT(cond) ((cond) ? (void)0 : __android_log_assert("##cond", COHERENT_ANDROID_LOG_TAG, "Assert in %s:%d", __FILE__, __LINE__))

extern JavaVM* g_JavaVM;
extern jclass cls_CoherentView;

#define GET_JNIENV(env) \
	ANDR_ASSERT(g_JavaVM != nullptr); \
	JNIEnv* env = nullptr; \
	g_JavaVM->AttachCurrentThread(&env, nullptr)

#define override
