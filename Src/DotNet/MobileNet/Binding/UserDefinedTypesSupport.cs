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

#if COHERENT_PLATFORM_IOS || COHERENT_PLATFORM_ANDROID
using Coherent.UI.Mobile;
using Coherent.UI.Mobile.Binding;
#endif

namespace Coherent.UI.Binding
{
	internal struct MemberData<T> where T : MemberInfo
	{
		internal MemberData(T member)
		{
			Member = member;
			MemberExposedName = null;
		}

		internal MemberData(T member, string name)
		{
			Member = member;
			MemberExposedName = name;
		}

		public string Name {
			get { return (MemberExposedName == null) ? Member.Name : MemberExposedName; }
			set { MemberExposedName = value; }
		}

		private string MemberExposedName;
		internal T Member;
	}

	internal class UserDefinedTypeSupport
	{
		internal static List<MemberData<FieldInfo>> GetFields(System.Type type)
		{
			return GetMembers<FieldInfo>(type, type.GetFields, BindingFlags.Public | BindingFlags.Instance);
		}

		internal static List<MemberData<PropertyInfo>> GetProperties(System.Type type)
		{
			var properties = GetMembers<PropertyInfo>(type, type.GetProperties, BindingFlags.Public | BindingFlags.Instance);
			foreach (var property in properties)
			{
				if (IsIndexedProperty(property.Member))
				{
					throw new BindingException(ScriptCallErrorType.SCE_ArgumentType, "Indexed properties are not supported. Please use an array or list field");
				}
			}
			return properties;
		}

		private static bool IsIndexedProperty(PropertyInfo property)
		{
			return property.GetIndexParameters().Length > 0;
		}

		internal static string GetTypeName(System.Type type)
		{
			return type.AssemblyQualifiedName;
		}

		private static List<MemberData<T>> GetMembers<T>(System.Type type, Func<BindingFlags, T[]> getter, BindingFlags defaultFlags) where T : MemberInfo
		{
			var attributes = type.GetCustomAttributes(typeof(CoherentType), true);
			if (attributes.Length > 0)
			{
				var coherentType = (CoherentType)attributes[0];
				var members = getter(coherentType.GetBindingFlags());
				var result = new List<MemberData<T>>(members.Length);
				foreach (var member in members)
				{
					var memberAttributes = member.GetCustomAttributes(typeof(CoherentProperty), false);
					if (memberAttributes.Length > 0)
					{
						var attribute = (CoherentProperty)memberAttributes[0];
						result.Add(new MemberData<T>(member, attribute.PropertyName));
					}
					else if (coherentType.Flags != PropertyBindingFlags.Explicit)
					{
						result.Add(new MemberData<T>(member));
					}
				}
				return result;
			}
			var defaultMembers = getter(defaultFlags);
			var defaultResult = new List<MemberData<T>>(defaultMembers.Length);
			foreach (var member in defaultMembers)
			{
				defaultResult.Add(new MemberData<T>(member));
			}
			return defaultResult;
		}

		internal static List<MemberData<MethodInfo>> GetMethods(Type type)
		{
			var methods = GetMembers<MethodInfo>(type, type.GetMethods, BindingFlags.Public | BindingFlags.Instance);
			var names = new HashSet<string>();
			foreach (var name in  methods.Select(m => m.Name))
			{
				if (!names.Add(name))
				{
					throw new BindingException("Binding of overloaded methods is not supported. Use CoherentProperty attribute to rename your method");
				}
			}

			return methods;
		}
	}
}
