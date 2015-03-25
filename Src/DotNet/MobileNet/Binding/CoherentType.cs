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
using System.Reflection;

#if COHERENT_PLATFORM_IOS || COHERENT_PLATFORM_ANDROID
using Coherent.UI.Binding;
namespace Coherent.UI.Mobile.Binding
#else
namespace Coherent.UI.Binding
#endif
{
	/// <summary>
	/// Determines the set of the bound properties for this type
	/// </summary>
	public enum PropertyBindingFlags
	{
		/// <summary>Bind only properties and fields with CoherentProperty attribute</summary>
		Explicit = 0,
		/// <summary>Bind only instance properties and fields</summary>
		Instance = 1,
		/// <summary>Bind only static properties and fields</summary>
		Static = 2,
		/// <summary>Bind all instance and static properties and fields</summary>
		All = 3,
	}

	/// <summary>
	/// Specify which properties and fields of a type are visible to Coherent UI
	/// </summary>
	[AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
	public class CoherentType : Attribute
	{
		public CoherentType()
		{
			Flags = PropertyBindingFlags.Explicit;
		}

		public CoherentType(PropertyBindingFlags flags)
		{
			Flags = flags;
		}

		internal BindingFlags GetBindingFlags()
		{
			var binding = BindingFlags.Public;
			switch (Flags)
			{
				case PropertyBindingFlags.Explicit:
				case PropertyBindingFlags.All:
					binding |= BindingFlags.Instance | BindingFlags.Static;
					break;

				case PropertyBindingFlags.Instance:
					binding |= BindingFlags.Instance;
					break;

				case PropertyBindingFlags.Static:
					binding |= BindingFlags.Static;
					break;

			}
			return binding;
		}

		internal PropertyBindingFlags Flags;
	}
}
