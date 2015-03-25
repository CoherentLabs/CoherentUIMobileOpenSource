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

#if defined(COHERENTUI_STATIC) || defined(COHERENT_DOT_NET)
	#define COHERENTUI_API
#else
	#ifdef COHERENTUI_EXPORTS
		#define COHERENTUI_API COHERENT_EXPORT_SYMBOL
	#else
		#define COHERENTUI_API COHERENT_IMPORT_SYMBOL
	#endif
#endif

#define COHERENT_UI_MAJOR_VERSION 1
#define COHERENT_UI_MINOR_VERSION 8
#define COHERENT_UI_PATCH_VERSION 9
#define COHERENT_UI_BUILD_VERSION 2

#define COHERENT_BITS_PER_VERSION 4
#define COHERENT_UI_SDK_VER ( \
	(COHERENT_UI_MAJOR_VERSION << (COHERENT_BITS_PER_VERSION * 3)) | \
	(COHERENT_UI_MINOR_VERSION << (COHERENT_BITS_PER_VERSION * 2)) | \
	(COHERENT_UI_PATCH_VERSION << (COHERENT_BITS_PER_VERSION    )) | \
	COHERENT_UI_BUILD_VERSION)

#define COHERENT_UNITY3D_BUILD
