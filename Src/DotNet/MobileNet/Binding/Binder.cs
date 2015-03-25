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
using System.Linq;

using Coherent.UI.Mobile;
using Coherent.UI.Mobile.Binding;

namespace Coherent.UI.Binding
{
	internal delegate void ManagedCallback(IntPtr data, double id);

	class Invoker
	{
		internal Invoker(Binder binder, System.Delegate callback)
		{
			Binder = binder;
			Callback = callback;
			var parameters = callback.Method.GetParameters();
			DelegateIsMethodStub = parameters.Length > 1 && parameters[0].ParameterType == typeof(MethodBinding);

			var skipParameters = (DelegateIsMethodStub) ? 1 : 0;
			Arguments = new object[parameters.Length - skipParameters];

			ArgumentGetters = (from parameter in parameters.Skip(skipParameters)
							   select Importer.CreateArgumentGetter(parameter.ParameterType))
							   .ToArray<Importer.ReadDelegate>();
		}

		internal void Invoke(IntPtr data, double id)
		{
			var importer = Binder.GetImporter(data);

			bool executeHandler = true;
			var arguments = importer.BeginArgumentList();
			var argumentsCount = Arguments.Length;
			if (arguments < argumentsCount)
			{
				for (var i = arguments; i < argumentsCount; ++i)
				{
					Binder.InvalidArgument(ScriptCallErrorType.SCE_ArgumentType, String.Format("Missing argument {0}", i));
				}
				executeHandler = false;
			}

			if (executeHandler)
			{
				var argumentsOffest = (!DelegateIsMethodStub) ? 0 : 1;
				for (var i = 0; i < argumentsCount; ++i)
				{
					try
					{
						Arguments[i] = ArgumentGetters[i](importer);
					}
					catch (BindingException e)
					{
						Binder.InvalidArgument(
							e.Error, String.Format(e.Message, i));
						executeHandler = false;
					}
				}
			}

			ExecuteAndSendResult(Binder, Callback, Arguments, id, executeHandler, DelegateIsMethodStub);
		}

		static internal void ExecuteAndSendResult(Binder binder, System.Delegate callback, object[] arguments, double id, bool execute, bool isDynamic)
		{
			object result = null;
			if (execute)
			{
				if (!isDynamic)
				{
					foreach (var invocation in callback.GetInvocationList())
					{
						result = invocation.Method.Invoke(invocation.Target, arguments);
					}
				}
				else
				{
					result = callback.DynamicInvoke(arguments);
				}
				if (result != null && id != 0)
				{
					binder.Return(id, result);
				}
			}
			else
			{
				binder.SendError();
			}
		}

		private object[] Arguments;
#if COHERENT_PLATFORM_IOS || COHERENT_PLATFORM_ANDROID
		private Importer.ReadDelegate[] ArgumentGetters;
#else
		private Importer.ReadDelegate<object>[] ArgumentGetters;
#endif
		private System.Delegate Callback;
		private Binder Binder;
		private bool DelegateIsMethodStub;
	}

	internal class GenericInvoker
	{
		internal GenericInvoker(Binder binder, System.Delegate callback)
		{
			Binder = binder;
			Callback = callback;
			Arguments = new object[2];

			if (GenericSignature == null)
			{
				GenericSignature = new System.Type[] { typeof(string), typeof(Value[]) };
			}

			var parameterTypes = from parameter in Callback.Method.GetParameters() select parameter.ParameterType;
			if (!parameterTypes.SequenceEqual(GenericSignature))
			{
				throw new UnsupportedCallbackException("Generic callback must have signature (string, Coherent.UI.Binding.Value[])");
			}
		}


		internal void Invoke(IntPtr data, double id)
		{
			var importer = Binder.GetImporter(data);
			bool executeHandler = true;
			var argumentsCount = importer.BeginArgumentList();
			try {
				Arguments[0] = importer.Read<string>();
			}
			catch (BindingException e)
			{
				Binder.InvalidArgument(e.Error, String.Format(e.Message, "1"));
				executeHandler = false;
			}

			Value[] arguments = new Value[argumentsCount - 1];
			var ReadValue = (System.Func<Value>)importer.Read<Value>;
			for (var i = 1; i < argumentsCount; ++i)
			{
				try {
					arguments[i - 1] = ReadValue();
				}
				catch (BindingException e)
				{
					Binder.InvalidArgument(e.Error, String.Format(e.Message, i));
					executeHandler = false;
				}
			}
			Arguments[1] = arguments;

			Invoker.ExecuteAndSendResult(Binder, Callback, Arguments, id, executeHandler, false);
		}

		private System.Delegate Callback;
		private Binder Binder;
		private object[] Arguments;
		private static System.Type[] GenericSignature;

	}

	internal partial class Binder: IDisposable
	{
		internal Binder()
		{
			RawBinder = View.GetBinder();
			PlatformInitialize();
		}

		internal Binder(View view)
		{
			View = view;
			RawBinder = View.GetBinder();

			PlatformInitialize();
		}

		private ManagedCallback CreateGenericCallback(System.Delegate callback)
		{
			var invoker = new GenericInvoker(this, callback);
			return invoker.Invoke;
		}
		private ManagedCallback CreateCallback(System.Delegate callback)
		{
			var invoker = new Invoker(this, callback);
			return invoker.Invoke;
		}

		internal Importer GetImporter(IntPtr data)
		{
			Importer.SetData(data);
			return Importer;
		}

		internal Exporter GetExporter()
		{
			Exporter.ResetData();
			return Exporter;
		}

		internal void SendError()
		{
			RawBinder.RawResult(0, null);
		}

		internal void Return(double id, object result)
		{
			var exporter = GetExporter();
			exporter.Export(result);

			RawBinder.RawResult(exporter.GetBytesCount(), exporter.GetBytes());
		}

		internal void InvalidArgument(ScriptCallErrorType error, string message)
		{
			View.SetScriptError(error, message);
		}

		private Importer Importer = new Importer(new BlobReader());
		private Exporter Exporter = new Exporter(new BlobWriter());

		private View View;
		internal DotNetBinder RawBinder;
		private DotNetBinder.BindingsCallback BindingsReleased;
		private DotNetBinder.BindingsCallback BindingsReadyDelegate;
		private MethodBinding m_MethodBinding = new MethodBinding();
	}
}
