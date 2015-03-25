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
		class Value;
		
		class Null
		{
		public:
			static const AlignedType Type = BT_Null;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Storage; // just to be sure the size is same across platforms
		};
		
		class Boolean
		{
		public:
			bool GetValue() const
			{
				return m_Value != 0;
			}
			
			void SetValue(bool value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_Boolean;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class Integer
		{
		public:
			int GetValue() const
			{
				return int(m_Value);
			}
			
			void SetValue(int value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_Integer;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class Integer64
		{
		public:
			long long int GetValue() const
			{
				return m_Value;
			}
			
			void SetValue(long long int value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_Integer64;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class UInteger
		{
		public:
			unsigned int GetValue() const
			{
				return (unsigned int)(m_Value);
			}
			
			void SetValue(unsigned int value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_UInteger;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class Number
		{
		public:
			double GetValue() const
			{
				return m_Value;
			}
			
			void SetValue(double value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_Number;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(double);
			}
			
		private:
			double m_Value;
		};
		
		class Byte
		{
		public:
			signed char GetValue() const
			{
				return (signed char)(m_Value);
			}
			
			void SetValue(signed char value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_Byte;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class UByte
		{
		public:
			unsigned char GetValue() const
			{
				return (unsigned char)(m_Value);
			}
			
			void SetValue(unsigned char value)
			{
				m_Value = value;
			}
			
			static const AlignedType Type = BT_UByte;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType);
			}
			
		private:
			AlignedType m_Value;
		};
		
		class String
		{
		public:
			size_t GetLength() const
			{
				return size_t(m_Length);
			}
			
			const char* GetBuffer() const
			{
				return m_Buffer;
			}
			
			static const AlignedType Type = BT_String;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType) + GetAlignedSize(size_t(m_Length));
			}
			
		private:
			String(String&);
			
			AlignedType m_Length;
			char m_Buffer[sizeof(AlignedType)];
		};
		
		class WString
		{
		public:
			size_t GetLength() const
			{
				return size_t(m_Length);
			}
			
			size_t GetSize() const
			{
				return size_t(m_Length) * sizeof(wchar_t);
			}
			
			const wchar_t* GetBuffer() const
			{
				return (wchar_t*)m_Buffer;
			}
			
			static const AlignedType Type = BT_WString;
			
			size_t GetSizeInBytes() const
			{
				return sizeof(AlignedType) + GetAlignedSize(size_t(m_Length * sizeof(wchar_t)));
			}
			
		private:
			AlignedType m_Length;
			char m_Buffer[sizeof(AlignedType)];
		};
		
		class Array
		{
		public:
			size_t GetSize() const
			{
				return size_t(m_ElementsCount);
			}
			
			static const AlignedType Type = BT_Array;
			
			size_t GetSizeInBytes() const
			{
				return size_t(m_Size);
			}
			
			const void* GetElementsBuffer() const
			{
				return m_Storage;
			}
			
			size_t GetElementsBufferSize() const
			{
				return size_t(m_Size - sizeof(AlignedType) * 2);
			}
			
			class ConstIterator
			{
			public:
				explicit ConstIterator(const Value* value);
				
				COHERENTUI_API ~ConstIterator();
				
				COHERENTUI_API const Value& operator*() const;
				
				COHERENTUI_API const Value* operator->() const;
				
				COHERENTUI_API ConstIterator& operator++();
				
				COHERENTUI_API bool operator==(const ConstIterator& rhs) const;
				
				COHERENTUI_API bool operator!=(const ConstIterator& rhs) const;
				
			private:
				const Value* m_Value;
			};
			
			COHERENTUI_API ConstIterator Begin() const;
			
			COHERENTUI_API ConstIterator End() const;
			
		private:
			AlignedType m_Size;
			AlignedType m_ElementsCount;
			char m_Storage[sizeof(AlignedType)];
		};
		
		class Object
		{
		public:
			static const AlignedType Type = BT_Object;
			
			size_t GetSizeInBytes() const
			{
				return size_t(m_Size);
			}
			
			class ConstIterator
			{
			public:
				explicit ConstIterator(const Value* begin, const Value* end);
				
				COHERENTUI_API ~ConstIterator();
				
				typedef std::pair<const Value*, const Value*> value_type;
				
				const value_type& operator*() const;
				
				COHERENTUI_API const value_type* operator->() const;
				
				COHERENTUI_API ConstIterator& operator++();
				
				COHERENTUI_API bool operator==(const ConstIterator& rhs) const;
				
				COHERENTUI_API bool operator!=(const ConstIterator& rhs) const;
				
			private:
				value_type m_Value;
				const Value* m_End;
			};
			
			COHERENTUI_API ConstIterator Begin() const;
			
			COHERENTUI_API ConstIterator End() const;
			
			size_t GetSize() const
			{
				return size_t(m_ElementsCount);
			}
			
			COHERENTUI_API const Value* EndOfStorage() const;
			const Value* StartOfStorage() const
			{
				return reinterpret_cast<const Value*>(m_Storage);
			}
		private:
			AlignedType m_Size;
			AlignedType m_ElementsCount;
			char m_Storage[sizeof(AlignedType)];
			
		};
		
		class Value
		{
		public:
			AlignedType GetType() const
			{
				return m_Type;
			}
			
			template <typename T>
			T& As();
			
			template <typename T>
			COHERENTUI_API const T& As() const;
			
			template <typename Functor>
			typename Functor::ResultType
			Invoke(const Functor& functor) const
			{
				switch (m_Type)
				{
					case BT_Null:
						return functor.Invoke(As<Null>());
					case BT_Boolean:
						return functor.Invoke(As<Boolean>());
					case BT_Integer:
						return functor.Invoke(As<Integer>());
					case BT_UInteger:
						return functor.Invoke(As<UInteger>());
					case BT_Number:
						return functor.Invoke(As<Number>());
					case BT_String:
						return functor.Invoke(As<String>());
					case BT_Array:
						return functor.Invoke(As<Array>());
					case BT_Object:
						return functor.Invoke(As<Object>());
					case BT_Integer64:
						return functor.Invoke(As<Integer64>());
					case BT_WString:
						return functor.Invoke(As<WString>());
					case BT_Byte:
						return functor.Invoke(As<Byte>());
					case BT_UByte:
						return functor.Invoke(As<UByte>());
					default:
						COHERENT_ASSERT(false, "blob value of unknown type", "the blob value has unknown type ", m_Type);
						return typename Functor::ResultType();
				}
			}
			
			COHERENTUI_API size_t GetSize() const;
			
		private:
			AlignedType m_Type;
			char m_Storage[sizeof(AlignedType)];
		};
	}
}

