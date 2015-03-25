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

using Coherent.UI.Binding;

namespace Coherent.UI.Mobile.Binding
{
	public class BindingException : System.Exception
	{
		internal ScriptCallErrorType Error;

		internal BindingException(ScriptCallErrorType error, string message)
			: base(message)
		{
			Error = error;
		}

        internal BindingException(string message)
            : base(message)
        {
        }

		internal BindingException(string expected, string actual)
			: base(GetMessage(expected, actual))
		{
			Error = ScriptCallErrorType.SCE_ArgumentType;
		}

		private static string GetName(ValueType type)
		{
			var index = (int)type;
			var names = new string[] {
				"Null",
				"Boolean",
				"Integer",
				"UInteger",
				"Number",
				"String",
				"Array",
				"Object",
			};
			return (index >= 0 && index < names.Length) ? names[index] : "Unknown";
		}

		internal BindingException(ValueType expected, ValueType actual)
			: base(GetMessage(GetName(expected), GetName(actual)))
		{
			Error = ScriptCallErrorType.SCE_ArgumentType;
		}

		private static string GetMessage(string expected, string actual)
		{
			return string.Format("Wrong type - expected {0}, got {1} while converting argument {{0}}", expected, actual);
		}
	}

	public class UnsupportedCallbackException : System.Exception
	{
		public UnsupportedCallbackException(string message)
			: base(message)
		{

		}
	}
}
