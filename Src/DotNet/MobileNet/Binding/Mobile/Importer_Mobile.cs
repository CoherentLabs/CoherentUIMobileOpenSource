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
#if COHERENT_PLATFORM_IOS || COHERENT_PLATFORM_ANDROID
using System.Collections.Generic;

using Coherent.UI.Mobile;
using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	internal struct UserDefinedType
	{
		internal string TypeName;
		internal System.Type Type;
	}

	internal struct UserDefinedTypeReader
	{
		internal Importer.ReadDelegate Reader;
		internal System.Type RealType; // the most derived type
	}

	internal partial class Importer
	{
		public Importer(BlobReader reader)
		{
			Reader = reader;

			Readers[typeof(bool)] = (ReadDelegate)Importer.ReadBool;
			Readers[typeof(int)] = (ReadDelegate)Importer.ReadInt;
			Readers[typeof(uint)] = (ReadDelegate)Importer.ReadUInt;
			Readers[typeof(sbyte)] = (ReadDelegate)Importer.ReadSByte;
			Readers[typeof(byte)] = (ReadDelegate)Importer.ReadByte;
			Readers[typeof(float)] = (ReadDelegate)Importer.ReadFloat;
			Readers[typeof(double)] = (ReadDelegate)Importer.ReadDouble;
			Readers[typeof(string)] = (ReadDelegate)Importer.ReadString;
			Readers[typeof(Value)] = (ReadDelegate)Importer.ReadValue;

			UserTypeReaders = new Dictionary<UserDefinedType, UserDefinedTypeReader>();
		}

		internal static object ReadValue(Importer importer)
		{
			return Value.Import(importer);
		}


		internal static object ReadBool(Importer importer)
		{
			return importer.Reader.ReadBool();
		}

		internal static object ReadInt(Importer importer)
		{
			return importer.Reader.ReadInt();
		}

		internal static object ReadUInt(Importer importer)
		{
			return importer.Reader.ReadUInt();
		}

		internal static object ReadSByte(Importer importer)
		{
			return importer.Reader.ReadSByte();
		}

		internal static object ReadByte(Importer importer)
		{
			return importer.Reader.ReadByte();
		}

		internal static object ReadFloat(Importer importer)
		{
			return importer.Reader.ReadFloat();
		}

		internal static object ReadDouble(Importer importer)
		{
			return importer.Reader.ReadDouble();
		}

		internal static object ReadString(Importer importer)
		{
			return importer.Reader.ReadString();
		}

		internal void SetData(System.IntPtr data)
		{
			Reader.SetData(data);
		}

		internal ValueType Peek()
		{
			return Reader.PeekType();
		}

		internal Value[] ReadArguments()
		{
			Reader.Read(); // skip size
			var size = Reader.Read();
			var result = new Coherent.UI.Mobile.Binding.Value[size];
			for (var i = 0UL; i < size; ++i)
			{
				result[i] = Coherent.UI.Mobile.Binding.Value.Import(this);
			}
			return result;
		}

		internal int BeginArgumentList()
		{
			Reader.Read(); // skip size
			return (int)Reader.Read();
		}
		
		private class ArgumentReader
		{
			public ArgumentReader(System.Type type)
			{
				m_Type = type;
			}
			
			public object Read(Importer importer)
			{
				return importer.Read(m_Type);
			}
			
			private System.Type m_Type;
		}

		internal static ReadDelegate CreateArgumentGetter(System.Type type)
		{
			var reader = new ArgumentReader(type);
			return reader.Read;
		}
		
		private ReadDelegate GetReader(System.Type type)
		{
			ReadDelegate d;
			if (!Readers.TryGetValue(type, out d))
			{
				System.Type ilist;
				System.Type idictionary;
				if (type.IsArray)
				{
					var reader = new ArrayReader(type);
					d = reader.Read;
				}
				else if ((ilist = type.GetInterface("System.Collections.Generic.IList`1")) != null)
				{
					var reader = new ListReader(type, ilist.GetGenericArguments());
					d = reader.Read;
				}
				else if ((idictionary = type.GetInterface("System.Collections.Generic.IDictionary`2")) != null)
				{
					var reader = new DictionaryReader(type, idictionary.GetGenericArguments());
					d = reader.Read;
				}
				else if (!type.IsPrimitive) // handle user defined types
				{
					d = CreateUserDefinedReader(ref type);
				}
				else
				{
					throw new UnsupportedPrimitiveTypeException(type);
				}
				Readers[type] = d;
			}
			return d;
		}

		public object Read(System.Type type)
		{
			var reader = GetReader(type);
			return reader(this);
		}

		private ReadDelegate CreateUserDefinedReader(ref System.Type realType)
		{
			var typename = Reader.ReadStringProperty("__Type");
			if (typename.Length > 0)
			{
				UserDefinedTypeReader reader;
				var typeid = new UserDefinedType { Type = realType, TypeName = typename };
				if (!UserTypeReaders.TryGetValue(typeid, out reader))
				{
					var type = System.Type.GetType(typename, false, false);
					if (type == null)
					{
						// try to locate type in the assembly of T
						var assembly = realType.Assembly;
						type = assembly.GetType(typename, false, false);
						if (type == null)
						{
							// try to locate the type in all loaded assemblies
							var assemblies = System.AppDomain.CurrentDomain.GetAssemblies();
							for (var i = 0; i < assemblies.Length && type == null; ++i)
							{
								type = assemblies[i].GetType(typename, false, false);
							}
						}
					}
					if (type != null)
					{
						var userTypeReader = RegisterType(type);
						realType = userTypeReader.RealType;
						return userTypeReader.Reader;
					}
					throw new BindingException(ScriptCallErrorType.SCE_ArgumentType, string.Format("Could not determine managed type from __Type {0}", typename));
				}
				realType = reader.RealType;
				return reader.Reader;
			}
			throw new BindingException(ScriptCallErrorType.SCE_ArgumentType, string.Format("Could not determine managed type from JavaScript value"));
		}

		internal UserDefinedTypeReader RegisterType(System.Type type)
		{
			var instance = new UserDefinedTypeImporter(type);
			var typedReader = new UserDefinedTypeReader { Reader = instance.Read, RealType = type };
			var typeId = new UserDefinedType { Type = type, TypeName = instance.TypeName };
			UserTypeReaders[typeId] = typedReader;
			return typedReader;
		}

		internal void ReadUserType(Dictionary<string, UserDefinedTypeImporter.SetterDelegate> setters, object value)
		{
			var properties = (int)Reader.BeginSequence(Coherent.UI.Mobile.Binding.ValueType.Object);
			UserDefinedTypeImporter.SetterDelegate setter;
			for (var i = 0; i < properties; ++i)
			{
				var name = Reader.ReadString();
				if (setters.TryGetValue(name, out setter))
				{
					if (setter != null)
					{
						setter(this, value);
					}
					else
					{
						Reader.SkipValue();
					}
				}
				else
				{
					throw new BindingException(ScriptCallErrorType.SCE_ArgumentType, string.Format("Type {0} has no property {1}", value.GetType().Name, name));
				}
			}
		}

		internal void SkipValue()
		{
			Reader.SkipValue();
		}

		internal bool IsNull()
		{
			return Peek() == ValueType.Null;
		}

		internal delegate object ReadDelegate(Importer importer);

		internal BlobReader Reader;
		private Dictionary<System.Type, ReadDelegate> Readers = new Dictionary<System.Type, ReadDelegate>();
		private Dictionary<UserDefinedType, UserDefinedTypeReader> UserTypeReaders;
	}
}

#endif
