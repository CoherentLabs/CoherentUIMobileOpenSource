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

using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	struct Sequence
	{
		private long Size;

		public void Begin(ValueType type, long size, BlobWriter blob)
		{
			Size = blob.WriteRaw((System.UInt64)type);
			blob.WriteRaw(0UL);
			blob.WriteRaw((System.UInt64)size);
		}

		public void Finish(BlobWriter blob)
		{
			var bytes = blob.GetBytesCount() - Size;
			blob.WriteAt((System.UInt64)bytes, Size);
		}
	}

	internal class BlobWriter
	{
		System.IO.MemoryStream Stream;
		System.IO.BinaryWriter Writer;

		System.Collections.Generic.Stack<Sequence> Sequences;

		public BlobWriter()
		{
			Stream = new System.IO.MemoryStream(256);
			Writer = new System.IO.BinaryWriter(Stream);
			Sequences = new System.Collections.Generic.Stack<Coherent.UI.Binding.Sequence>();
		}

		public byte[] GetBytes()
		{
			return Stream.GetBuffer();
		}

		public long GetBytesCount()
		{
			return Stream.Position;
		}

		public void WriteNull()
		{
			Writer.Write((System.UInt64)ValueType.Null);
			Writer.Write((System.UInt64)0);
		}

		public void Write(bool value)
		{
			WritePrimitive(ValueType.Boolean, (value)? 1L : 0L);
		}

		public void Write(int value)
		{
			WritePrimitive(ValueType.Integer, (System.Int64)value);
		}

		public void Write(uint value)
		{
			WritePrimitive(ValueType.UInteger, (System.Int64)value);
		}

		public void Write(sbyte value)
		{
			WritePrimitive(ValueType.SByte, (System.Int64)value);
		}

		public void Write(byte value)
		{
			WritePrimitive(ValueType.Byte, (System.Int64)value);
		}

		public void Write(float value)
		{
			Write((double)value);
		}

		public void Write(double value)
		{
			Writer.Write((System.UInt64)ValueType.Number);
			Writer.Write(value);
		}

		private char[] m_CharBuffer = new char[64];
		private byte[] m_ByteBuffer = new byte[64];
		private byte[] m_Padding = new byte[] { 0, 0, 0, 0, 0, 0, 0, 0 };

		public void Write(string value)
		{
			Writer.Write((System.UInt64)ValueType.String);

			var length = 0L;
			var valueLength = value.Length;

			if (m_CharBuffer.Length < valueLength)
			{
				m_CharBuffer = new char[valueLength];
				m_ByteBuffer = new byte[valueLength];
			}
			bool isAscii = true;
			for (var i = 0; i < valueLength && isAscii; ++i)
			{
				var b = (int)value[i];
				if (b < 0x80)
				{
					m_ByteBuffer[i] = (byte)b;
				}
				else
				{
					isAscii = false;
				}
			}
			if (isAscii)
			{
				Writer.Write((ulong)valueLength);
				length = valueLength;
				Writer.Write(m_ByteBuffer, 0, valueLength);
			}
			else
			{
				var lengthPosition = Stream.Position;
				Writer.Write(0UL);
				value.CopyTo(0, m_CharBuffer, 0, valueLength);
				Writer.Write(m_CharBuffer, 0, valueLength);
				length = Stream.Position - lengthPosition - 8;
				WriteAt((ulong)length, lengthPosition);
			}

			var actualBytes = (((length + 8) - 1) / 8) * 8;
			Writer.Write(m_Padding, 0, (int)(actualBytes - length));
		}

		internal long WriteRaw(System.UInt64 value)
		{
			Writer.Write(value);
			return Stream.Position;
		}

		internal void WriteAt(System.UInt64 value, long position)
		{
			var oldPosition = Stream.Position;
			Writer.Seek((int)position, System.IO.SeekOrigin.Begin);
			Writer.Write(value);
			Writer.Seek((int)oldPosition, System.IO.SeekOrigin.Begin);
		}

		private void WritePrimitive(ValueType type, System.Int64 value)
		{
			Writer.Write((System.UInt64)type);
			Writer.Write(value);
		}

		public void BeginSequence(ValueType type, long size)
		{
			var sequence = new Sequence();
			sequence.Begin(type, size, this);
			Sequences.Push(sequence);
		}

		public void EndSequence()
		{
			var sequence = Sequences.Pop();
			sequence.Finish(this);
		}

		internal void Reset()
		{
			Stream.Position = 0;
		}
	}
}
