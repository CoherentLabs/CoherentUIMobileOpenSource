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
		
		class BlobOStream
		{
		public:
			typedef std::function<void* (size_t)> AllocateBufferFunction;
			
			COHERENTUI_API static const AlignedType BadFood = 0x0BADF00D0BADF00D;
			
			BlobOStream()
			: m_Begin(nullptr)
			, m_End(0)
			, m_Size(0)
			{
			}
			
			BlobOStream(const BlobOStream& other)
			: m_Begin(other.m_Begin)
			, m_End(other.m_End)
			, m_Size(other.m_Size)
			, m_Allocator(other.m_Allocator)
			{
			}
			
			BlobOStream(void* begin, size_t size, AllocateBufferFunction allocator)
			: m_Begin(static_cast<char*>(begin))
			, m_End(0)
			, m_Size(size)
			, m_Allocator(allocator)
			{
				
			}
			
			void Write(const void* data, size_t size)
			{
				const size_t alignedSize = GetAlignedSize(size);
				if(!GrowIfNecessary(alignedSize))
				{
					COHERENT_ERROR("Not enough memory for IPC! Losing IPC events!");
					return;
				}
				char* target = m_Begin + m_End;
				std::memcpy(target, data, size);
				if (size != alignedSize)
				{
					std::memcpy(target + size, &BadFood, alignedSize - size);
				}
				m_End += alignedSize;
			}
			
			template <typename T>
			void Write(T x)
			{
				Write(AlignedType(x));
			}
			
			void Write(AlignedType value)
			{
				Write(&value, sizeof(AlignedType));
			}
			
			void WriteAt(AlignedType value, size_t index)
			{
				if (index + sizeof(AlignedType) >= m_End)
				{
					if(!GrowIfNecessary(index - m_End + sizeof(AlignedType)))
					{
						COHERENT_ERROR("Not enough memory for IPC! Losing IPC events!");
						return;
					}
				}
				if(!m_Begin)
				{
					COHERENT_ERROR("Not enough memory for IPC! Losing IPC events!");
					return;
				}
				std::memcpy(m_Begin + index, &value, sizeof(AlignedType));
			}
			
			size_t GetSize()
			{
				return m_End;
			}
			
		private:
			bool GrowIfNecessary(size_t alignedSize)
			{
				if (m_End + alignedSize > m_Size)
				{
					const size_t addedSpace = std::max(m_Size, m_End + alignedSize - m_Size);
					m_Begin = static_cast<char*>(m_Allocator(addedSpace));
					m_Size += addedSpace;
				}
				
				return !!m_Begin;
			}
			
			char* m_Begin;
			size_t m_End;
			size_t m_Size;
			AllocateBufferFunction m_Allocator;
		};
		
		class BlobIStream
		{
		public:
			COHERENTUI_API BlobIStream(const void* buffer, size_t size);
			
			COHERENTUI_API ~BlobIStream();
			
			const char* PeekBytes(size_t bytes) const
			{
				const size_t alignedSize = GetAlignedSize(bytes);
				return (m_Index + alignedSize <= m_Size)? m_Begin + m_Index : nullptr;
			}
			
			void Advance(size_t bytes)
			{
				m_Index += GetAlignedSize(bytes);
			}
			
			const char* GetBytes(size_t bytes)
			{
				const char* buffer = PeekBytes(bytes);
				if (buffer)
				{
					m_Index += GetAlignedSize(bytes);
				}
				return buffer;
			}
			
		private:
			const char* m_Begin;
			size_t m_Index;
			size_t m_Size;
		};
	}
}
