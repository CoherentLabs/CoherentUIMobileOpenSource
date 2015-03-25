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

#include <Binding/Blob/BlobDeclarations.h>
#include <Declarations.h>

#include <cassert>

namespace Coherent
{
namespace Blob
{
class BlobIStream;

template <typename T>
void CoherentFromBlob(BlobIStream& stream, T& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, bool& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, signed int& value);
COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, unsigned int& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, signed char& value);
COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, unsigned char& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, signed long long& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, float& value);
COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, double& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, void*& value);

COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, char* value, size_t length);
COHERENTUI_API void CoherentFromBlob(BlobIStream& stream, wchar_t* value, size_t length);

COHERENTUI_API void CoherentBufferFromBlob(BlobIStream& stream, const char*& buffer, size_t& size);
COHERENTUI_API void CoherentBufferFromBlob(BlobIStream& stream, const wchar_t*& buffer, size_t& size);

template <typename Char, typename CharTraits, typename Allocator>
void CoherentFromBlob(BlobIStream& stream, std::basic_string<Char, CharTraits, Allocator>& value)
{
	const Char* buffer = nullptr;
	size_t size = 0;
	
	CoherentBufferFromBlob(stream, buffer, size);
	
	value.assign(buffer, size);
}

// TODO: Better ASSERT macro :/
COHERENTUI_API void CoherentArraySizeFromBlob(BlobIStream& stream, size_t& size);

template <typename T, typename A>
void CoherentFromBlob(BlobIStream& stream, std::vector<T, A>& value)
{
	size_t size = 0;
	CoherentArraySizeFromBlob(stream, size);
	value.resize(size);
	for (typename std::vector<T, A>::iterator i = value.begin(); i != value.end(); ++i)
	{
		CoherentFromBlob(stream, *i);
	}
}

template <typename T, int size>
void CoherentFromBlob(BlobIStream& stream, T (&a)[size])
{
	size_t actualSize = 0;
	CoherentArraySizeFromBlob(stream, actualSize);
	assert(size == actualSize);
	for (int i = 0; i < size; ++i)
	{
		CoherentFromBlob(stream, a[i]);
	}
}

COHERENTUI_API void CoherentObjectSizeFromBlob(BlobIStream& stream, size_t& size);

template <typename Key, typename Value, typename Comparator, typename Allocator>
void CoherentFromBlob(BlobIStream& stream, std::map<Key, Value, Comparator, Allocator>& value)
{
	typedef std::map<Key, Value, Comparator, Allocator> MapType;
	
	size_t size = 0;
	CoherentObjectSizeFromBlob(stream, size);
	
	typename MapType::value_type keyValue;
	for (size_t i = 0; i < size; ++i)
	{
		CoherentFromBlob(stream, const_cast<Key&>(keyValue.first));
		CoherentFromBlob(stream, keyValue.second);

#if defined(COHERENT_PLATFORM_IOS)
		value.insert(keyValue);
#endif
	}
}
}
}
