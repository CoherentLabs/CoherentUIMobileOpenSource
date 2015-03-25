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

#include <Binding/Blob/Blob.h>
#include <Binding/Blob/FromBlob.h>
#include <Binding/Blob/BlobStream.h>

namespace Coherent
{
namespace Blob
{
namespace
{
	void FromBlobValue(const Value* blob, bool& value)
	{
		value = blob->As<Boolean>().GetValue();
	}

	void FromBlobValue(const Value* blob, signed int& value)
	{
		value = blob->As<Integer>().GetValue();
	}

	void FromBlobValue(const Value* blob, unsigned int& value)
	{
		value = blob->As<UInteger>().GetValue();
	}

	void FromBlobValue(const Value* blob, signed char& value)
	{
		value = blob->As<Byte>().GetValue();
	}

	void FromBlobValue(const Value* blob, unsigned char& value)
	{
		value = blob->As<UByte>().GetValue();
	}

	void FromBlobValue(const Value* blob, signed long long& value)
	{
		value = blob->As<Integer64>().GetValue();
	}

	void FromBlobValue(const Value* blob, float& value)
	{
		value = float(blob->As<Number>().GetValue());
	}

	void FromBlobValue(const Value* blob, double& value)
	{
		value = blob->As<Number>().GetValue();
	}

	void FromBlobValue(const Value* blob, void*& value)
	{
		value = reinterpret_cast<void*>(blob->As<Integer>().GetValue());
	}

	template <typename T>
	void FromBlobImpl(BlobIStream& stream, BlobType type, T& value)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == type, "blob value is not of expected type", "expected value of type ", type, ", but got value of type ", blobValue->GetType());

		FromBlobValue(blobValue, value);

		stream.Advance(blobValue->GetSize());
	}
}


	void CoherentFromBlob(BlobIStream& stream, bool& value)
	{
		FromBlobImpl(stream, BT_Boolean, value);
	}

	void CoherentFromBlob(BlobIStream& stream, signed int& value)
	{
		FromBlobImpl(stream, BT_Integer, value);
	}

	void CoherentFromBlob(BlobIStream& stream, unsigned int& value)
	{
		FromBlobImpl(stream, BT_UInteger, value);
	}

	void CoherentFromBlob(BlobIStream& stream, signed long long& value)
	{
		FromBlobImpl(stream, BT_Integer64, value);
	}

	void CoherentFromBlob(BlobIStream& stream, signed char& value)
	{
		FromBlobImpl(stream, BT_Byte, value);
	}

	void CoherentFromBlob(BlobIStream& stream, unsigned char& value)
	{
		FromBlobImpl(stream, BT_UByte, value);
	}

	void CoherentFromBlob(BlobIStream& stream, float& value)
	{
		FromBlobImpl(stream, BT_Number, value);
	}

	void CoherentFromBlob(BlobIStream& stream, double& value)
	{
		FromBlobImpl(stream, BT_Number, value);
	}

	void CoherentFromBlob(BlobIStream& stream, void*& value)
	{
		FromBlobImpl(stream, BT_Integer, value);
	}

	void CoherentFromBlob(BlobIStream& stream, char* value, size_t length)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_String, "blob value is not string", "expected value of type string, but got value of type ", blobValue->GetType());

		const String& string = blobValue->As<String>();
		length = std::min(string.GetLength(), length);

		std::memcpy(value, string.GetBuffer(), length);

		stream.Advance(blobValue->GetSize());
	}

	void CoherentFromBlob(BlobIStream& stream, wchar_t* value, size_t length)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_WString, "blob value is not a wide string", "expected value of type wide string, but got value of type ", blobValue->GetType());

		const WString& string = blobValue->As<WString>();
		length = std::min(string.GetSize(), length);

		std::memcpy(value, string.GetBuffer(), length);

		stream.Advance(blobValue->GetSize());
	}

	void CoherentBufferFromBlob(BlobIStream& stream, const char*& buffer, size_t& size)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_String, "blob value is not string", "expected value of type string, but got value of type ", blobValue->GetType());

		const String& string = blobValue->As<String>();

		buffer = string.GetBuffer();
		size = string.GetLength();

		stream.Advance(blobValue->GetSize());
	}

	void CoherentBufferFromBlob(BlobIStream& stream, const wchar_t*& buffer, size_t& size)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_WString, "blob value is not a wide string", "expected value of type wide string, but got value of type ", blobValue->GetType());

		const WString& string = blobValue->As<WString>();

		buffer = string.GetBuffer();
		size = string.GetLength();

		stream.Advance(blobValue->GetSize());
	}

	void CoherentArraySizeFromBlob(BlobIStream& stream, size_t& size)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_Array, "blob value is not array", "expected value of type array, but got value of type ", blobValue->GetType());

		const Array& array = blobValue->As<Array>();
		COHERENT_ASSERT_RETURN(stream.PeekBytes(blobValue->GetSize()) != nullptr, "end of blob stream reached prematurely", "could not read ", array.GetSizeInBytes(), " more bytes from stream");
		size = array.GetSize();

		stream.Advance(3 * sizeof(AlignedType));
	}

	void CoherentObjectSizeFromBlob(BlobIStream& stream, size_t& size)
	{
		const Value* blobValue = reinterpret_cast<const Value*>(stream.PeekBytes(sizeof(Value)));
		COHERENT_ASSERT_RETURN(blobValue != nullptr, "end of blob stream reached prematurely", "could not read ", sizeof(Value), " more bytes from stream");
		COHERENT_ASSERT_RETURN(blobValue->GetType() == BT_Object, "blob value is not array", "expected value of type object, but got value of type ", blobValue->GetType());

		const Object& object = blobValue->As<Object>();
		COHERENT_ASSERT_RETURN(stream.PeekBytes(blobValue->GetSize()) != nullptr, "end of blob stream reached prematurely", "could not read ", object.GetSizeInBytes(), " more bytes from stream");
		size = object.GetSize();

		stream.Advance(3 * sizeof(AlignedType));
	}

}
}
