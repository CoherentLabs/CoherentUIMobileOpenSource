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
using System.Reflection;

using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	static internal class DynamicCode
	{
	
		public static UserDefinedTypeImporter.SetterDelegate CreateSetter(Type type, FieldInfo field)
		{
			return (Importer importer, object value) =>
			{
				var fieldValue = importer.Read(field.FieldType);
				field.SetValue(value, fieldValue);
			};
		}

		public static UserDefinedTypeImporter.SetterDelegate CreateSetter(Type type, PropertyInfo property)
		{
			return (Importer importer, object value) =>
			{
				var fieldValue = importer.Read(property.PropertyType);
				property.SetValue(value, fieldValue, null);
			};
		}

		public static UserDefinedTypeExporter.GetterDelegate CreateGetter(Type type, FieldInfo @field)
		{
			return (object value, Exporter exporter) =>
			{
				object fieldValue = field.GetValue(value);
				exporter.Export(fieldValue);
			};
		}

		public static UserDefinedTypeExporter.GetterDelegate CreateGetter(Type type, PropertyInfo property)
		{
			return (object value, Exporter exporter) =>
			{
				object propertyValue = property.GetGetMethod().Invoke(value, new object[]{});
				exporter.Export(propertyValue);
			};
		}

        internal delegate TResult StubDelegate<TResult>(int index);
        internal delegate TResult StubDelegate<TResult, T1>(int index, T1 argument1);
        internal delegate TResult StubDelegate<TResult, T1, T2>(int index, T1 argument1, T2 argument2);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3>(int index, T1 argument1, T2 argument2, T3 argument3);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3, T4>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3, T4, T5>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3, T4, T5, T6>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3, T4, T5, T6, T7>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7);
        internal delegate TResult StubDelegate<TResult, T1, T2, T3, T4, T5, T6, T7, T8>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7, T8 argument8);

        internal delegate void StubVoidDelegate(int index);
        internal delegate void StubVoidDelegate<T1>(int index, T1 argument1);
        internal delegate void StubVoidDelegate<T1, T2>(int index, T1 argument1, T2 argument2);
        internal delegate void StubVoidDelegate<T1, T2, T3>(int index, T1 argument1, T2 argument2, T3 argument3);
        internal delegate void StubVoidDelegate<T1, T2, T3, T4>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4);
        internal delegate void StubVoidDelegate<T1, T2, T3, T4, T5>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5);
        internal delegate void StubVoidDelegate<T1, T2, T3, T4, T5, T6>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6);
        internal delegate void StubVoidDelegate<T1, T2, T3, T4, T5, T6, T7>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7);
        internal delegate void StubVoidDelegate<T1, T2, T3, T4, T5, T6, T7, T8>(int index, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7, T8 argument8);

        private static Type[] StubTypes = new Type[] {
            typeof(StubDelegate<>),
            typeof(StubDelegate<,>),
            typeof(StubDelegate<,,>),
            typeof(StubDelegate<,,,>),
            typeof(StubDelegate<,,,,>),
            typeof(StubDelegate<,,,,,>),
            typeof(StubDelegate<,,,,,,>),
            typeof(StubDelegate<,,,,,,,>),
            typeof(StubDelegate<,,,,,,,,>),
        };

        private static Type[] VoidStubTypes = new Type[] {
            typeof(StubVoidDelegate),
            typeof(StubVoidDelegate<>),
            typeof(StubVoidDelegate<,>),
            typeof(StubVoidDelegate<,,>),
            typeof(StubVoidDelegate<,,,>),
            typeof(StubVoidDelegate<,,,,>),
            typeof(StubVoidDelegate<,,,,,>),
            typeof(StubVoidDelegate<,,,,,,>),
            typeof(StubVoidDelegate<,,,,,,,>),
        };

        internal static Type CreateStubType(Type result, Type[] parameters)
        {

            if (parameters.Length - 2 > StubTypes.Length)
            {
                throw new BindingException("CoherentUI doesn't support binding more than 8 arguments");
            }

            System.Type genericStub;
            int offset;
            if (result != typeof(void))
            {
                genericStub = StubTypes[parameters.Length - 2];
                offset = 1;
            }
            else
            {
                genericStub = VoidStubTypes[parameters.Length - 2];
                offset = 2;
            }

            var genericParameters = new Type[parameters.Length - offset];
            genericParameters[0] = result;
            for (var i = 2; i < parameters.Length; ++i)
            {
                genericParameters[i - offset] = parameters[i];
            }

            return genericStub.MakeGenericType(genericParameters);
        }

        internal static Type[] GetStubParameterTypes(MethodInfo method, Type targetType)
        {
            var methodParameters = method.GetParameters();
            var stubParameters = new Type[2 + methodParameters.Length];
            stubParameters[0] = targetType;
            stubParameters[1] = typeof(int);
            for (var i = 0; i < methodParameters.Length; ++i)
            {
                stubParameters[i + 2] = methodParameters[i].ParameterType;
            }
            return stubParameters;
        }

        public static Delegate CreateStubDelegate(MethodInfo method, MethodBinding.GetObjectDelegate getObject)
        {
			throw new Exception("not implemented");
        }
	}
}
#endif
