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
using System;
using System.Collections.Generic;
using System.Reflection;

using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	internal class Exporter
	{
		public BlobWriter Blob;

		internal delegate void ExportDelegate(Exporter e, object x);

		public Exporter(BlobWriter blob)
		{
			Blob = blob;

			Exporters[typeof(bool)] = (ExportDelegate)Exporter.ExportBool;
			Exporters[typeof(int)] = (ExportDelegate)Exporter.ExportInt;
			Exporters[typeof(uint)] = (ExportDelegate)Exporter.ExportUInt;
			Exporters[typeof(sbyte)] = (ExportDelegate)Exporter.ExportSByte;
			Exporters[typeof(byte)] = (ExportDelegate)Exporter.ExportByte;
			Exporters[typeof(float)] = (ExportDelegate)Exporter.ExportFloat;
			Exporters[typeof(double)] = (ExportDelegate)Exporter.ExportDouble;
			Exporters[typeof(string)] = (ExportDelegate)Exporter.ExportString;
			Exporters[typeof(Coherent.UI.Mobile.Binding.Value)] = (ExportDelegate)Exporter.ExportValue;
		}

		private static void ExportBool(Exporter exporter, object value)
		{
			exporter.Blob.Write((bool)value);
		}
		
		private static void ExportInt(Exporter exporter, object value)
		{
			exporter.Blob.Write((int)value);
		}
		
		private static void ExportUInt(Exporter exporter, object value)
		{
			exporter.Blob.Write((uint)value);
		}
		
		private static void ExportSByte(Exporter exporter, object value)
		{
			exporter.Blob.Write((sbyte)value);
		}
		
		private static void ExportByte(Exporter exporter, object value)
		{
			exporter.Blob.Write((byte)value);
		}
		
		private static void ExportFloat(Exporter exporter, object value)
		{
			exporter.Blob.Write((float)value);
		}
		
		private static void ExportDouble(Exporter exporter, object value)
		{
			exporter.Blob.Write((double)value);
		}
		
		private static void ExportString(Exporter exporter, object value)
		{
			exporter.Blob.Write((string)value);
		}
	
		private static void ExportValue(Exporter exporter, object value)
		{
			var realValue = (Coherent.UI.Mobile.Binding.Value)value;
			realValue.Export(exporter);
		}
		
		internal static void ExportObject(Exporter exporter, object value)
		{
			exporter.ExecuteExporter(value, value.GetType());
		}

		void ExecuteExporter(object x, System.Type type)
		{
			ExportDelegate v;			
			if (!Exporters.TryGetValue(type, out v))
			{
				if (type.IsArray)
				{
					v = ExportArray;
				}
				else if (type.GetInterface("System.Collections.IList") != null)
				{
					v = ExportIList;
				}
				else if (type.GetInterface("System.Collections.IDictionary") != null)
				{
					v = ExportIDictionary;
				}
				else if (!type.IsPrimitive) // handle user defined types
				{
					v = CreateUserTypeExporter(x, type);
				}
				else
				{
					throw new UnsupportedPrimitiveTypeException(type);
				}
				Exporters[type] = v;
			}
			v(this, x);
		}
		
		public void Export(object x)
		{
			if (x == null)
			{
				Blob.WriteNull();
				return;
			}
			var type = x.GetType();
			ExecuteExporter(x, type);
		}

		private static ExportDelegate CreateUserTypeExporter(object x, System.Type type)
		{
			var exporter = new UserDefinedTypeExporter(type);
			return exporter.Export;

		}

		private static void ExportArray(Exporter ex, object array)
		{
			var realArray = (Array)array;
			ex.Blob.BeginSequence(Coherent.UI.Mobile.Binding.ValueType.Array, realArray.LongLength);
			foreach (var e in realArray)
			{
				ex.Export(e);
			}
			ex.Blob.EndSequence();
		}

		private static void ExportIList(Exporter exporter, object list)
		{
			var realList = (System.Collections.IList)list;
			exporter.Blob.BeginSequence(Coherent.UI.Mobile.Binding.ValueType.Array, realList.Count);
			foreach (var e in realList)
			{
				exporter.Export(e);
			}
			exporter.Blob.EndSequence();
		}

		private static void ExportIDictionary(Exporter exporter, object dict)
		{
			var realDict = (System.Collections.IDictionary)dict;
			exporter.Blob.BeginSequence(Coherent.UI.Mobile.Binding.ValueType.Object, realDict.Count);
			foreach (System.Collections.DictionaryEntry item in realDict)
			{
				exporter.Export(item.Key);
				exporter.Export(item.Value);
			}
			exporter.Blob.EndSequence();
		}

		internal void ResetData()
		{
			Blob.Reset();
		}

		internal byte[] GetBytes()
		{
			return Blob.GetBytes();
		}

		internal uint GetBytesCount()
		{
			return (uint)Blob.GetBytesCount();
		}

		internal void AddExporter(Type type, ExportDelegate exporter)
		{
			Exporters[type] = exporter;
		}
		
		private Dictionary<Type, ExportDelegate> Exporters = new Dictionary<Type, ExportDelegate>();
	}
}

#endif
