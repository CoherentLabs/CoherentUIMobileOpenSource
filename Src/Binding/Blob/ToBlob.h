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

namespace Coherent
{
	namespace Blob
	{
		class BlobOStream;
		
		COHERENTUI_API void CoherentToBlobNull(BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(bool value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(signed int value, BlobOStream& stream);
		COHERENTUI_API void CoherentToBlob(unsigned int value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(signed char value, BlobOStream& stream);
		COHERENTUI_API void CoherentToBlob(unsigned char value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(long long value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(float value, BlobOStream& stream);
		COHERENTUI_API void CoherentToBlob(double value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(const char* value, size_t length, BlobOStream& stream);
		COHERENTUI_API void CoherentToBlob(const char* value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(const wchar_t* value, size_t length, BlobOStream& stream);
		COHERENTUI_API void CoherentToBlob(const wchar_t* value, BlobOStream& stream);
		
		COHERENTUI_API void CoherentToBlob(void* value, BlobOStream& stream);
		
		template <typename Char, typename CharTraits, typename Allocator>
		void CoherentToBlob(const std::basic_string<Char, CharTraits, Allocator>& value, BlobOStream& stream)
		{
			CoherentToBlob(value.c_str(), value.length(), stream);
		}
		
		
		class SequenceWriter
		{
		public:
			COHERENTUI_API SequenceWriter();
			COHERENTUI_API SequenceWriter(BlobOStream& stream);
			COHERENTUI_API SequenceWriter(const SequenceWriter& other);
			COHERENTUI_API SequenceWriter& operator=(const SequenceWriter&);
			COHERENTUI_API ~SequenceWriter();
			
			COHERENTUI_API void BeginSequence(BlobType type, size_t size);
			COHERENTUI_API void BeginSequence(BlobType type);
			COHERENTUI_API void SetSize(size_t size);
			COHERENTUI_API void EndSequence();
		private:
			BlobOStream* m_Stream;
			size_t m_IndexBytesSize;
			size_t m_IndexElementsCount;
		};
		
		template <typename T, typename A>
		void CoherentToBlob(const std::vector<T, A>& value, BlobOStream& stream)
		{
			SequenceWriter writer(stream);
			writer.BeginSequence(BT_Array, value.size());
			for (typename std::vector<T, A>::const_iterator i = value.begin(); i != value.end(); ++i)
			{
				CoherentToBlob(*i, stream);
			}
			writer.EndSequence();
		}
		
		template <typename T, int size>
		void CoherentToBlob(const T (&a)[size], BlobOStream& stream)
		{
			SequenceWriter writer(stream);
			writer.BeginSequence(BT_Array, size);
			for (int i = 0; i != size; ++i)
			{
				CoherentToBlob(a[i], stream);
			}
			writer.EndSequence();
		}
		
		template <typename Key, typename Value, typename Comparator, typename Allocator>
		void CoherentToBlob(const std::map<Key, Value, Comparator, Allocator>& value, BlobOStream& stream)
		{
			typedef std::map<Key, Value, Comparator, Allocator> MapType;
			SequenceWriter writer(stream);
			writer.BeginSequence(BT_Object, value.size());
			
			std::for_each(value.begin(), value.end(), [&stream](const typename MapType::value_type& pair) {
				CoherentToBlob(pair.first, stream);
				CoherentToBlob(pair.second, stream);
			});
			
			writer.EndSequence();
		}
	}
}