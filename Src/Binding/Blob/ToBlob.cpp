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
#include "precompiled.h"

#include <Binding/Blob/BlobStream.h>
#include <Binding/Blob/ToBlob.h>
#include <Binding/Blob/Blob.h>

namespace Coherent
{
namespace Blob
{
	void CoherentToBlobNull(BlobOStream& stream)
	{
		stream.Write(Coherent::Blob::Null::Type);
		stream.Write(Coherent::Blob::AlignedType(0));
	}
	
	void CoherentToBlob(bool value, BlobOStream& stream)
	{
		stream.Write(BT_Boolean);
		stream.Write(value);
	}

	void CoherentToBlob(signed int value, BlobOStream& stream)
	{
		stream.Write(BT_Integer);
		stream.Write(value);
	}

	void CoherentToBlob(unsigned int value, BlobOStream& stream)
	{
		stream.Write(BT_UInteger);
		stream.Write(value);
	}

	void CoherentToBlob(signed char value, BlobOStream& stream)
	{
		stream.Write(BT_Byte);
		stream.Write(value);
	}

	void CoherentToBlob(unsigned char value, BlobOStream& stream)
	{
		stream.Write(BT_UByte);
		stream.Write(value);
	}

	void CoherentToBlob(long long value, BlobOStream& stream)
	{
		stream.Write(BT_Integer64);
		stream.Write(value);
	}

	void CoherentToBlob(float value, BlobOStream& stream)
	{
		double dValue = value;
		CoherentToBlob(dValue, stream);
	}

	void CoherentToBlob(double value, BlobOStream& stream)
	{
		stream.Write(BT_Number);
		stream.Write(*reinterpret_cast<AlignedType*>(&value));
	}

	void CoherentToBlob(const char* value, BlobOStream& stream)
	{
		size_t length = std::strlen(value);
		CoherentToBlob(value, length, stream);
	}

	void CoherentToBlob(const char* value, size_t length, BlobOStream& stream)
	{
		stream.Write(BT_String);
		stream.Write(length);
		stream.Write(value, length);
	}

	void CoherentToBlob(const wchar_t* value, BlobOStream& stream)
	{
		size_t length = std::wcslen(value);
		CoherentToBlob(value, length, stream);
	}

	void CoherentToBlob(const wchar_t* value, size_t length, BlobOStream& stream)
	{
		stream.Write(BT_WString);
		stream.Write(length);
		stream.Write(value, length * sizeof(wchar_t));
	}

	void CoherentToBlob(void* value, BlobOStream& stream)
	{
		stream.Write(BT_Integer);
		stream.Write(value);
	}

	SequenceWriter::SequenceWriter()
		: m_Stream(nullptr)
		, m_IndexBytesSize(0)
		, m_IndexElementsCount(0)
	{
	}

	SequenceWriter::SequenceWriter(BlobOStream& stream)
		: m_Stream(&stream)
		, m_IndexBytesSize(0)
		, m_IndexElementsCount(0)
	{
	}

	SequenceWriter::SequenceWriter(const SequenceWriter& other)
		: m_Stream(other.m_Stream)
		, m_IndexBytesSize(other.m_IndexBytesSize)
		, m_IndexElementsCount(other.m_IndexElementsCount)
	{
	}

	SequenceWriter::~SequenceWriter()
	{
	}

	SequenceWriter& SequenceWriter::operator=(const SequenceWriter& other)
	{
		m_Stream = other.m_Stream;
		m_IndexBytesSize = other.m_IndexBytesSize;
		m_IndexElementsCount = other.m_IndexElementsCount;

		return *this;
	}

	void SequenceWriter::BeginSequence(BlobType type)
	{
		m_Stream->Write(AlignedType(type));
		m_IndexBytesSize = m_Stream->GetSize();
		m_Stream->Write(0);
		m_IndexElementsCount = m_Stream->GetSize();
		m_Stream->Write(0);
	}

	void SequenceWriter::BeginSequence(BlobType type, size_t size)
	{
		m_Stream->Write(AlignedType(type));
		m_IndexBytesSize = m_Stream->GetSize();
		m_Stream->Write(0);
		m_IndexElementsCount = m_Stream->GetSize();
		m_Stream->Write(AlignedType(size));
	}

	void SequenceWriter::SetSize(size_t size)
	{
		m_Stream->WriteAt(AlignedType(size), m_IndexElementsCount);
	}

	void SequenceWriter::EndSequence()
	{
		AlignedType sequenceSize = m_Stream->GetSize() - m_IndexBytesSize;
		m_Stream->WriteAt(sequenceSize, m_IndexBytesSize);
	}

}
}
