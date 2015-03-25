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
using System.Runtime.InteropServices;

using Coherent.UI;
namespace Coherent.UI.Mobile.Binding
{
	/// <summary>
	/// Holds the arguments for the generic callback not handled by a registered delegate
	/// </summary>
	public class CallbackArguments
	{
		internal CallbackArguments(IntPtr data)
		{
            m_Data = data;

            int size = (int)Marshal.ReadInt64(data);
			var bytes = new byte[size];

			Marshal.Copy(data, bytes, 0, size);
			var reader = new Coherent.UI.Binding.BlobReader(bytes);
			var importer = new Coherent.UI.Binding.Importer(reader);
			Arguments = importer.ReadArguments();
		}

		/// <summary>
		/// Arguments provided by JavaScript
		/// </summary>
		public Value[] Arguments;

        internal IntPtr Data
        {
            get { return m_Data; }
        }

        private IntPtr m_Data;
	}

	/// <summary>
	/// Holds .Net extension methods to Coherent.UI.View
	/// </summary>
	public static class ViewExtensions
	{
		/// <summary>
		/// Trigger event in view
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		public static void TriggerEvent<T1>(this View view, string name, T1 argument1)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);

			view.Binder.RawBinder.RawEvent(name, 1, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		public static void TriggerEvent<T1, T2>(this View view, string name, T1 argument1, T2 argument2)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);

			view.Binder.RawBinder.RawEvent(name, 2, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3>(this View view, string name, T1 argument1, T2 argument2, T3 argument3)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);

			view.Binder.RawBinder.RawEvent(name, 3, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <typeparam name="T4">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		/// <param name="argument4">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3, T4>(this View view, string name, T1 argument1, T2 argument2, T3 argument3, T4 argument4)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);
			exporter.Export(argument4);

			view.Binder.RawBinder.RawEvent(name, 4, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <typeparam name="T4">Type for the event argument</typeparam>
		/// <typeparam name="T5">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		/// <param name="argument4">argument for the event</param>
		/// <param name="argument5">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3, T4, T5>(this View view, string name, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);
			exporter.Export(argument4);
			exporter.Export(argument5);

			view.Binder.RawBinder.RawEvent(name, 5, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <typeparam name="T4">Type for the event argument</typeparam>
		/// <typeparam name="T5">Type for the event argument</typeparam>
		/// <typeparam name="T6">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		/// <param name="argument4">argument for the event</param>
		/// <param name="argument5">argument for the event</param>
		/// <param name="argument6">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3, T4, T5, T6>(this View view, string name, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);
			exporter.Export(argument4);
			exporter.Export(argument5);
			exporter.Export(argument6);

			view.Binder.RawBinder.RawEvent(name, 6, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <typeparam name="T4">Type for the event argument</typeparam>
		/// <typeparam name="T5">Type for the event argument</typeparam>
		/// <typeparam name="T6">Type for the event argument</typeparam>
		/// <typeparam name="T7">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		/// <param name="argument4">argument for the event</param>
		/// <param name="argument5">argument for the event</param>
		/// <param name="argument6">argument for the event</param>
		/// <param name="argument7">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3, T4, T5, T6, T7>(this View view, string name, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);
			exporter.Export(argument4);
			exporter.Export(argument5);
			exporter.Export(argument6);
			exporter.Export(argument7);

			view.Binder.RawBinder.RawEvent(name, 7, exporter.GetBytesCount(), exporter.GetBytes());
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T1">Type for the event argument</typeparam>
		/// <typeparam name="T2">Type for the event argument</typeparam>
		/// <typeparam name="T3">Type for the event argument</typeparam>
		/// <typeparam name="T4">Type for the event argument</typeparam>
		/// <typeparam name="T5">Type for the event argument</typeparam>
		/// <typeparam name="T6">Type for the event argument</typeparam>
		/// <typeparam name="T7">Type for the event argument</typeparam>
		/// <typeparam name="T8">Type for the event argument</typeparam>
		/// <param name="view">the view</param>
		/// <param name="name">name of the event</param>
		/// <param name="argument1">argument for the event</param>
		/// <param name="argument2">argument for the event</param>
		/// <param name="argument3">argument for the event</param>
		/// <param name="argument4">argument for the event</param>
		/// <param name="argument5">argument for the event</param>
		/// <param name="argument6">argument for the event</param>
		/// <param name="argument7">argument for the event</param>
		/// <param name="argument8">argument for the event</param>
		public static void TriggerEvent<T1, T2, T3, T4, T5, T6, T7, T8>(this View view, string name, T1 argument1, T2 argument2, T3 argument3, T4 argument4, T5 argument5, T6 argument6, T7 argument7, T8 argument8)
		{
			var exporter = view.Binder.GetExporter();

			exporter.Export(argument1);
			exporter.Export(argument2);
			exporter.Export(argument3);
			exporter.Export(argument4);
			exporter.Export(argument5);
			exporter.Export(argument6);
			exporter.Export(argument7);
			exporter.Export(argument8);

			view.Binder.RawBinder.RawEvent(name, 8, exporter.GetBytesCount(), exporter.GetBytes());
		}
	}
}
