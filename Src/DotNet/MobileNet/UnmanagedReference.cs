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
using System.Runtime.InteropServices;

namespace Coherent.UI
{
    internal class UnmanagedReference
    {
        private delegate void ManagedReferenceCallback(IntPtr unmanaged);

#if COHERENT_PLATFORM_ANDROID
        [DllImport("CoherentAndroidNative",
            EntryPoint = "CoherentUI_SetManagedReferenceCallbacks")]
        private static extern void
        CoherentUI_SetManagedReferenceCallbacks_Android(
            ManagedReferenceCallback addReference,
            ManagedReferenceCallback releaseReference);
#else
        [DllImport("__Internal",
            EntryPoint = "CoherentUI_SetManagedReferenceCallbacks")]
        private static extern void CoherentUI_SetManagedReferenceCallbacks(
            ManagedReferenceCallback addReference,
            ManagedReferenceCallback releaseReference);
#endif

#if COHERENT_PLATFORM_IOS
        internal static void Register(IntPtr unmanaged)
        {
            lock (Instance)
            {
                Instance.RegisterImpl(unmanaged);
            }
        }

        internal static void Unregister(IntPtr unmanaged)
        {
            lock (Instance)
            {
                Instance.UnregisterImpl(unmanaged);
            }
        }
#else
        internal static void Register(HandleRef unmanaged)
        {
            lock (Instance)
            {
                Instance.RegisterImpl(unmanaged);
            }
        }

        internal static void Unregister(HandleRef unmanaged)
        {
            lock (Instance)
            {
                Instance.UnregisterImpl(unmanaged.Handle);
            }
        }
#endif

        UnmanagedReference()
        {
            Handles = new Dictionary<IntPtr, Reference>();

            m_AddReferenceCallback = AddReference;
            m_ReleaseReferenceCallback = ReleaseReference;

#if !COHERENT_PLATFORM_ANDROID
            CoherentUI_SetManagedReferenceCallbacks(m_AddReferenceCallback,
                m_ReleaseReferenceCallback);
#else
            CoherentUI_SetManagedReferenceCallbacks_Android(
                m_AddReferenceCallback, m_ReleaseReferenceCallback);
#endif
        }

        private void ChangeReference(IntPtr unmanaged, bool add)
        {
            Reference reference;
            if (!Handles.TryGetValue(unmanaged, out reference))
            {
                return;
            }

            if (add)
            {
                var target = reference.Handle.Target;
                if (target == null)
                {
                    return;
                }

                reference.InUse = true;

                reference.Handle.Free();
                reference.Handle = GCHandle.Alloc(target, GCHandleType.Normal);
            }
            else
            {
                reference.InUse = false;
                reference.Handle.Free();
            }
        }


        private void UnregisterImpl(IntPtr unmanaged)
        {
            Reference reference;
            if (!Handles.TryGetValue(unmanaged, out reference))
            {
                return;
            }

            Handles.Remove(unmanaged);

            if (reference.InUse)
            {
                throw new System.Exception("this instance is still used by " +
                    "native code");
            }
        }

        [MonoPInvokeCallback(typeof(ManagedReferenceCallback))]
        private static void AddReference(IntPtr unmanaged)
        {
            lock (Instance)
            {
                Instance.ChangeReference(unmanaged, true);
            }
        }

        [MonoPInvokeCallback(typeof(ManagedReferenceCallback))]
        private static void ReleaseReference(IntPtr unmanaged)
        {
            lock (Instance)
            {
                Instance.ChangeReference(unmanaged, false);
            }
        }

#if COHERENT_PLATFORM_IOS
        private void RegisterImpl(IntPtr unmanaged)
        {
            if (!Handles.ContainsKey(unmanaged))
            {
                Handles.Add(unmanaged,
                    new Reference(GCHandle.Alloc(null,
                        GCHandleType.Weak)));
            }
        }
#else
        private void RegisterImpl(HandleRef unmanaged)
        {
            if (!Handles.ContainsKey(unmanaged.Handle))
            {
                Handles.Add(unmanaged.Handle,
                    new Reference(GCHandle.Alloc(unmanaged.Wrapper,
                        GCHandleType.Weak)));
            }
        }
#endif

        class Reference
        {
            public Reference(GCHandle handle)
            {
                Handle = handle;
                InUse = false;
            }
            public GCHandle Handle;
            public bool InUse;
        }

        private Dictionary<IntPtr, Reference> Handles;
        private ManagedReferenceCallback m_AddReferenceCallback;
        private ManagedReferenceCallback m_ReleaseReferenceCallback;

        static UnmanagedReference Instance = new UnmanagedReference();
    }
}
