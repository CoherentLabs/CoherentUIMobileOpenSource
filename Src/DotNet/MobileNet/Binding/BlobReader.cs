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
using System.IO;

using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	internal class StreamLookAhead : System.IDisposable
	{
		internal StreamLookAhead(System.IO.Stream stream)
		{
			Stream = stream;
			InitialPosition = stream.Position;
			Disposed = false;
		}

		private long InitialPosition;
		private System.IO.Stream Stream;
		private bool Disposed;

		public void Dispose()
		{
			Dispose(true);
			System.GC.SuppressFinalize(this);
		}

		protected virtual void Dispose(bool disposing)
		{
			if (disposing && !Disposed)
			{
				Stream.Position = InitialPosition;
				Disposed = true;
			}
		}
	}

	internal class BlobReader
	{
		public BlobReader()
		{
			Stream = new MemoryStream(256);
			Reader = new BinaryReader(Stream);
		}

		// used for testing purposes
		internal BlobReader(byte[] data)
		{
			Stream = new MemoryStream(data, 0, data.Length, true, true);
			Reader = new BinaryReader(Stream);
		}

		internal void SetData(System.IntPtr data)
		{
			var size = System.Runtime.InteropServices.Marshal.ReadInt64(data);
			Stream.SetLength(size);
			System.Runtime.InteropServices.Marshal.Copy(data, Stream.GetBuffer(), 0, (int)size);
			Stream.Position = 0;
		}

		public bool ReadBool()
		{
			var type = GetValueType();
			if (type == ValueType.Boolean)
			{
				return Reader.ReadInt64() != 0;
			}
			throw new BindingException(ValueType.Boolean, type);
		}

		public int ReadInt()
		{
			var type = GetValueType();
			if (type == ValueType.Integer || type == ValueType.UInteger)
			{
				return (int)Reader.ReadInt64();
			}
			throw new BindingException(ValueType.Integer, type);
		}

		public uint ReadUInt()
		{
			var type = GetValueType();
			if (type == ValueType.UInteger)
			{
				return (uint)Reader.ReadInt64();
			}
			throw new BindingException(ValueType.UInteger, type);
		}

		public sbyte ReadSByte()
		{
			return (sbyte)ReadDouble();
		}

		public byte ReadByte()
		{
			return (byte)ReadDouble();
		}

		public float ReadFloat()
		{
			return (float)ReadDouble();
		}

		public double ReadDouble()
		{
			var type = GetValueType();
			if (type == ValueType.Number)
			{
				return Reader.ReadDouble();
			}
			if (type == ValueType.Integer)
			{
				return (double)Reader.ReadInt64();
			}
			if (type == ValueType.UInteger)
			{
				return (double)Reader.ReadUInt64();
			}
			throw new BindingException(ValueType.Number, type);
		}

		public string ReadString()
		{
			var type = GetValueType();
			if (type == ValueType.String)
			{
				var length = Reader.ReadInt64();
				var s = System.Text.Encoding.UTF8.GetString(Stream.GetBuffer(), (int)Stream.Position, (int)length);
				var actualBytes = ((length + sizeof(System.UInt64) - 1) / sizeof(System.UInt64)) * sizeof(System.UInt64);
				Stream.Seek(actualBytes, SeekOrigin.Current);
				return s;
			}
			else if (type == ValueType.Null)
			{
				Read(); // skip the 0
				return null;
			}
			throw new BindingException(ValueType.String, type);
		}

		public ulong BeginSequence(ValueType sequenceType)
		{
			var type = GetValueType();
			if (type == sequenceType)
			{
				Reader.ReadUInt64();
				return Reader.ReadUInt64();
			}
			throw new BindingException(sequenceType, type);
		}

		private ValueType GetValueType()
		{
			return (ValueType)Reader.ReadUInt64();
		}

		internal ValueType PeekType()
		{
			return (ValueType)Reader.PeekChar();
		}

		internal System.UInt64 Read()
		{
			return Reader.ReadUInt64();
		}

		internal string ReadStringProperty(string propertyName)
		{
			using (var guard = new StreamLookAhead(Stream))
			{
				var properties = (int)BeginSequence(ValueType.Object);
				for (var i = 0; i < properties; ++i)
				{
					string name = ReadString();
					if (name == propertyName)
					{
						return ReadString();
					}
				}
			}
			return "";
		}

		internal void SkipValue()
		{
			var type = GetValueType();
			switch (type)
			{
				case ValueType.Null:
				case ValueType.Boolean:
				case ValueType.Integer:
				case ValueType.UInteger:
				case ValueType.Number:
					Stream.Position += sizeof(System.UInt64);
					break;
				case ValueType.String:
					{
						var length = Reader.ReadInt64();
						Stream.Position += ((length + sizeof(System.UInt64) - 1) / sizeof(System.UInt64)) * sizeof(System.UInt64);
					}
					break;
				case ValueType.Array:
				case ValueType.Object:
					Stream.Position += Reader.ReadInt64();
					break;
			}
		}

		private MemoryStream Stream;
		private BinaryReader Reader;
	}
}

