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

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Linq;

using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	internal class UserDefinedTypeExporter
	{
		public UserDefinedTypeExporter(System.Type type)
		{
			RealTypeName = UserDefinedTypeSupport.GetTypeName(type);
			RealType = type;

			var fields = UserDefinedTypeSupport.GetFields(RealType);
			var properties = UserDefinedTypeSupport.GetProperties(RealType);
			var count = fields.Count + properties.Count;
			Names = new string[count];
			Getters = new GetterDelegate[count];
			int i = 0;
			while (i < fields.Count)
			{
				var fieldData = fields[i];
				Names[i] = fieldData.Name;
				Getters[i] = CreateGetter(type, fieldData.Member);
				++i;
			}
			while (i < count)
			{
				var propertyData = properties[i - fields.Count];
				Names[i] = propertyData.Name;
				Getters[i] = CreateGetter(type, propertyData.Member);
				++i;
			}
		}

		private GetterDelegate CreateGetter(Type type, FieldInfo field)
		{
			return DynamicCode.CreateGetter(type, field);
		}

		private GetterDelegate CreateGetter(Type type, PropertyInfo property)
		{
			return DynamicCode.CreateGetter(type, property);
		}

		internal void Export(Exporter exporter, object value)
		{
			exporter.Blob.BeginSequence(Coherent.UI.Mobile.Binding.ValueType.Object, Getters.Length + 1);
			exporter.Export("__Type");
			exporter.Export(RealTypeName);
			for (var i = 0; i < Getters.Length; ++i)
			{
				exporter.Export(Names[i]);
				Getters[i](value, exporter);
			}
			exporter.Blob.EndSequence();
		}

		internal delegate void GetterDelegate(object value, Exporter exporter);

		private Type RealType;
		private string RealTypeName;
		private GetterDelegate[] Getters;
		private string[] Names;
	}

	internal class UserDefinedTypeImporter
	{
		public UserDefinedTypeImporter(System.Type realType)
		{
			RealType = realType;

			RealTypeName = UserDefinedTypeSupport.GetTypeName(realType);
			var fields = realType.GetFields();
			var properties = realType.GetProperties();
			FieldSetters = new System.Collections.Generic.Dictionary<string, SetterDelegate>(fields.Length + properties.Length + 1);
			FieldSetters["__Type"] = this.CheckType;
			foreach (var field in UserDefinedTypeSupport.GetFields(RealType))
			{
				FieldSetters[field.Name] = CreateFieldSetter(realType, field.Member);
			}
			foreach (var property in UserDefinedTypeSupport.GetProperties(RealType))
			{
				FieldSetters[property.Name] = CreatePropertySetter(realType, property.Member);
			}
		}

		internal delegate void SetterDelegate(Importer importer, object value);

		internal object Read(Importer importer)
		{
			var result = Activator.CreateInstance(RealType);
			importer.ReadUserType(FieldSetters, result);
			return result;
		}

		private void CheckType(Importer importer, object value)
		{
			var typename = (string)importer.Read(typeof(string));
			if (RealTypeName != typename && !RealTypeName.StartsWith(typename))
			{
				throw new BindingException(RealTypeName, typename);
			}
		}

		SetterDelegate CreateFieldSetter(Type type, FieldInfo field)
		{
			return DynamicCode.CreateSetter(type, field);
		}

		SetterDelegate CreatePropertySetter(Type type, PropertyInfo property)
		{
			var setter = property.GetSetMethod();
			if (setter != null)
			{
				return DynamicCode.CreateSetter(type, property);
			}
			return null;
		}

		internal string TypeName {
			get { return RealTypeName; }
		}

		private Type RealType;
		private string RealTypeName;
		private System.Collections.Generic.Dictionary<String, SetterDelegate> FieldSetters;
	}
}

