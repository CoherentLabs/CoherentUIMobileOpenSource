%module(directors="1") CoherentUI_Native
%apply void *VOID_INT_PTR { void * }
%{
#include <Errors.h>
#include <ViewInfo.h>
#include <View.h>
#include <ViewListener.h>
#include <FileHandler.h>
#include <UISystem.h>
struct HTTPHeaderHelper
{
	void* headers;
	unsigned count;
};
%}

%feature("director") ViewListenerBase;
%feature("director") ViewListener;
%feature("director") EventListener;
%feature("director") FileHandler;
%feature("director") ILogHandler;

#define COHERENTUI_API __declspec(dllexport)
#define COHERENT_LOGGING_PUBLIC __declspec(dllexport)
#define COHERENT_DOT_NET
#define COHERENT_OVERRIDE
#define COHERENT_CDECL __cdecl

#ifdef COHERENT_PLATFORM_WIN
#define _WINDOWS_
#endif

%include <csharp/voidptr.i>
%include <voidptr.i>
%include <arrays_csharp.i>
//%include <coherent_handle.i>
%include <windows.i>
%include <typemaps.i>
%include <wchar.i>

// Disable attempts to generate wrapper methods for these operators
%ignore Coherent::UI::CoherentHandle::operator<;
%ignore Coherent::UI::CoherentHandle::operator=;
%ignore Coherent::UI::CoherentHandle::operator==;
%ignore Coherent::UI::CoherentHandle::operator HANDLE;
%ignore Coherent::UI::CoherentHandle::operator size_t;

// Rename the CheckUISDKVersion* methods since the mobile SWIG does not add a "CSharp_" prefix
// to the wrapper methods and a redefinition error occurs
%rename(CheckMobileUISDKVersionExact) CheckUISDKVersionExact;
%rename(CheckMobileUISDKVersionCompatible) CheckUISDKVersionCompatible;

// Disable warnings
%warnfilter(451) Coherent::UI::ViewError::Error;
%warnfilter(451) Coherent::UI::SystemError::Error;
%warnfilter(514) Coherent::UI::EventListener;

//--------------------------------------------------------------------------------------
// Typemap for int array
//--------------------------------------------------------------------------------------

%typemap(ctype)  int * "int *"
%typemap(imtype) int * "IntPtr"
%typemap(cstype) int * "int[]"
%typemap(csin)   int * "$csinput"
%typemap(in)     int * %{ $1 = $input; %}
%typemap(directorin)     int * %{ $input = $1; %}
%typemap(out)    int * %{ $result = $1; %}
%typemap(csout)  int * { return $imcall; }
%typemap(csdirectorin) int * "$iminput"
%typemap(csdirectorout) int * "$cscall"

//--------------------------------------------------------------------------------------
// Typemap for string arrays
//--------------------------------------------------------------------------------------

%define CSHARP_STRING_ARRAY_FROM_CPP(CHARTYPE, SIZETYPE)

%typemap(ctype) CHARTYPE** "CHANGEME_ARRAY_CTYPE_$CHARTYPE**$SIZETYPE$"
%typemap(imtype) CHARTYPE** "CHANGEME_ARRAY_IMTYPE_$IntPtr$SIZETYPE$"
%typemap(cstype) CHARTYPE** "string[]"
%typemap(csdirectorin) (CHARTYPE** CHARPTRARRAY, SIZETYPE ARRAYLENGTH) "$modulePINVOKE.CoherentWCharArrayToStringArrayHelper.Convert($1, (int)$2)"
%typemap(directorin) (CHARTYPE** CHARPTRARRAY, SIZETYPE ARRAYLENGTH) "$input = (CHARTYPE**)$1;"
%typemap(csin, pre="\tint $csinput_Length = $csinput.Length;") (CHARTYPE** CHARPTRARRAY, SIZETYPE ARRAYLENGTH) "$1, $1_Length"
%typemap(in) (CHARTYPE** CHARPTRARRAY, SIZETYPE ARRAYLENGTH) "$input = (const wchar_t**)$1;"

%enddef

CSHARP_STRING_ARRAY_FROM_CPP(const wchar_t, unsigned int)

%apply (const wchar_t** CHARPTRARRAY, unsigned int ARRAYLENGTH) { (const wchar_t** previous, unsigned int previousLength), (const wchar_t** next, unsigned int nextLength) }


//--------------------------------------------------------------------------------------
// Typemap for EventArguments
//--------------------------------------------------------------------------------------

%typemap(cstype) const Coherent::UI::EventArguments& "Mobile.Binding.CallbackArguments"
%typemap(imtype) const Coherent::UI::EventArguments& "IntPtr"
%typemap(csin) const Coherent::UI::EventArguments& "$csinput.Data"
%typemap(csout) const Coherent::UI::EventArguments& "csout"
%typemap(in) const Coherent::UI::EventArguments& %{ $input = nullptr; %}
%typemap(out) const Coherent::UI::EventArguments& "out"
%typemap(directorin) const Coherent::UI::EventArguments& %{ $input = (&$1)->GetData(); %}
%typemap(directorout) const Coherent::UI::EventArguments& "directorout"
%typemap(csdirectorin) const Coherent::UI::EventArguments& "new Mobile.Binding.CallbackArguments($iminput)"
%typemap(csdirectorout) const Coherent::UI::EventArguments& "csdirectorout"

#ifdef COHERENT_PLATFORM_IOS
%begin %{
#include <Binding/EventArguments.h>
#include <iOS/Binding/EventHandlerNet.h>
#include <Binding/DotNetBinder.h>
#include <iOS/ViewImpl.h>
typedef Coherent::UI::DotNetBinder::BindingsCallback BindingsCallback;
%}
#elif defined(COHERENT_PLATFORM_ANDROID)
%begin %{
#include <Binding/EventArguments.h>
#include <Android/jni/Binding/EventHandlerNet.h>
#include <Binding/DotNetBinder.h>
#include <Android/jni/ViewImpl.h>
typedef Coherent::UI::DotNetBinder::BindingsCallback BindingsCallback;
%}
#endif

%typemap(ctype)			Coherent::UI::ManagedCallbackAOT "Coherent::UI::ManagedCallbackAOT"
%typemap(cstype)		Coherent::UI::ManagedCallbackAOT "Coherent.UI.Binding.ManagedCallbackAOT"
%typemap(imtype)		Coherent::UI::ManagedCallbackAOT "Coherent.UI.Binding.ManagedCallbackAOT"
%typemap(csin)			Coherent::UI::ManagedCallbackAOT "$csinput"
%typemap(csout)			Coherent::UI::ManagedCallbackAOT "csout"
%typemap(in)			Coherent::UI::ManagedCallbackAOT %{ $1 = $input; %}
%typemap(out)			Coherent::UI::ManagedCallbackAOT "out"
%typemap(directorin)	Coherent::UI::ManagedCallbackAOT %{ $input = $1; %}
%typemap(directorout)	Coherent::UI::ManagedCallbackAOT "directorout"
%typemap(csdirectorin)	Coherent::UI::ManagedCallbackAOT "csdirectorin"
%typemap(csdirectorout) Coherent::UI::ManagedCallbackAOT "csdirectorout"

// hide normal BindCall and RegisterForEvent methods and replace them
%ignore BindCall(const char*, IEventHandler*);

%ignore RegisterForEvent(const char*, IEventHandler*);

%typemap(csclassmodifiers) Coherent::UI::DotNetBinder "internal class"
%typemap(ctype) BindingsCallback "BindingsCallback"
%typemap(cstype) BindingsCallback "BindingsCallback"
%typemap(imtype) BindingsCallback "DotNetBinder.BindingsCallback"
%typemap(csin) BindingsCallback "$csinput"
%typemap(in) BindingsCallback %{ $1 = $input; %}
%typemap(cscode) Coherent::UI::DotNetBinder %{
    public delegate void BindingsCallback(uint binderId, int frameId, int isMainFrame);
%}

//--------------------------------------------------------------------------------------
// Helper classes
//--------------------------------------------------------------------------------------

%pragma(csharp) imclasscode=%{
  internal static class CoherentWCharArrayToStringArrayHelper {

	internal static string[] Convert(IntPtr ppStrings, int size) {
		if (size == 0)
		{
			return new string[0];
		}
		System.IntPtr[] pStrings = new System.IntPtr[size];
		System.Runtime.InteropServices.Marshal.Copy(ppStrings, pStrings, 0, size);
		string[] actualStrings = new string[size];
		for (int i = 0; i < size; ++i)
		{
			actualStrings[i] = $modulePINVOKE.SWIGWStringHelper.CreateWString(pStrings[i]);
		}
		return actualStrings;
	}
  }
%}
  
#if defined(COHERENT_PLATFORM_POSIX)
%pragma(csharp) imclasscode=%{
  internal class CustomWCharMarshaler : ICustomMarshaler
  {
	  static CustomWCharMarshaler instance = null;

	  private System.Text.Encoding WideEncoding;

	  private CustomWCharMarshaler()
	  {
		WideEncoding = (System.Environment.OSVersion.Platform == PlatformID.Unix) ? System.Text.Encoding.UTF32 : System.Text.Encoding.Unicode;
	  }

	  public static ICustomMarshaler GetInstance(string cookie)
	  {
		  if (instance == null)
		  {
			  instance = new CustomWCharMarshaler();
		  }
		  return instance;
	  }

	  public void CleanUpManagedData(object managedObj)
	  {
		  throw new NotImplementedException();
	  }

	  public void CleanUpNativeData(IntPtr pNativeData)
	  {
		  Marshal.FreeCoTaskMem(pNativeData);
	  }

	  public int GetNativeDataSize()
	  {
		  return -1;
	  }

	  public IntPtr MarshalManagedToNative(object managedObj)
	  {
            string str = managedObj as String;
            if (str == null)
            {
                return IntPtr.Zero;
            }

            byte[] bytes = WideEncoding.GetBytes(str);
            IntPtr nativeObj = Marshal.AllocCoTaskMem(bytes.Length + 4);

            Marshal.Copy(bytes, 0, nativeObj, bytes.Length);
            Marshal.WriteInt32(nativeObj, bytes.Length, 0);

            return nativeObj;
	  }

	  public object MarshalNativeToManaged(IntPtr pNativeData)
	  {
		  throw new NotImplementedException();
	  }
  }
%}
#endif

// ------- HTTPHeaders -------
%typemap(cstype)	(Coherent::UI::HTTPHeader* headers, unsigned headersCount) "HTTPHeader[]"
%typemap(directorin)	(Coherent::UI::HTTPHeader* headers, unsigned headersCount) %{
	HTTPHeaderHelper helper = {$1, $2};
	$input = &helper;
%}
%typemap(csdirectorin)	(Coherent::UI::HTTPHeader* headers, unsigned headersCount) "ViewListenerBase.GetHeaders($1)"

%apply (Coherent::UI::HTTPHeader*, unsigned) { (Coherent::UI::HTTPHeader* headers, unsigned headersCount) }

%typemap(cscode) Coherent::UI::ViewListenerBase %{
	internal static HTTPHeader[] GetHeaders(IntPtr native)
	{
		IntPtr nativeHeaders = Marshal.ReadIntPtr(native);
		int count = Marshal.ReadInt32(native, IntPtr.Size);
		if(nativeHeaders == IntPtr.Zero || count == 0) {
			return null;
		}
		HTTPHeader[] headers = new HTTPHeader[count];
		for (int i = 0; i < count; ++i)
		{
			headers[i].Field = Marshal.PtrToStringAnsi(Marshal.ReadIntPtr(nativeHeaders, i * 2 * System.IntPtr.Size));
            headers[i].Content = Marshal.PtrToStringAnsi(Marshal.ReadIntPtr(nativeHeaders, (i * 2 + 1)* System.IntPtr.Size));
		}
		return headers;
	}
%}

//--------------------------------------------------------------------------------------
// It is possible to extend the generated classes using the following %typemap directive
//
%typemap(cscode) Coherent::UI::View %{
  internal Coherent.UI.Binding.Binder Binder;
  public void BindCall(string name, System.Delegate handler)
  {
    var entry = Binder.Register(name, handler);
    BindCallInternal(name, entry.thunk, entry.id);
  }

  public void RegisterForEvent(string name, System.Delegate handler)
  {
    var entry = Binder.Register(name, handler);
    RegisterForEventInternal(name, entry.thunk, entry.id);
  }
  
%}

%typemap(cscode) Coherent::UI::ViewListener %{
    public int FailLoadSubscribersCount() {
        if(this.FailLoad == null)
        {
            return 0;
        }

        return this.FailLoad.GetInvocationList().Length;
    }
%}

%typemap(csdestruct, methodname="Dispose", methodmodifiers="public") Coherent::UI::View %{
{
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CoherentUI_NativePINVOKE.delete_View(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
        Binder.Dispose();
      }
      GC.SuppressFinalize(this);
    }
}
%}

extern "C" COHERENTUI_API Coherent::UI::UISystem* InitializeUISystem(
		int sdkVersion
		, const char* licenseKey
		, const Coherent::UI::SystemSettings& settings
		, Coherent::UI::EventListener* listener
		, Coherent::Logging::Severity logSeverity
		, Coherent::Logging::ILogHandler* logHandler
		, Coherent::UI::FileHandler* fileHandler
		);

%apply unsigned char INPUT[] { const unsigned char* data }
%csmethodmodifiers GetBinder "internal"
%csmethodmodifiers InvalidArgumentInternal "internal"

%typemap(csclassmodifiers) ViewListenerBase "internal class"
%typemap(csclassmodifiers) BrowserViewListenerBase "internal class"

%ignore Coherent::UI::ViewInfo::Parent;

%{
	namespace Coherent {
#if defined(COHERENT_UNITY3D_BUILD)
		typedef char CharType;
#else
		typedef wchar_t CharType;
#endif
	}
%}


namespace Coherent {
#if defined(COHERENT_UNITY3D_BUILD)
	typedef char CharType;
#else
	typedef wchar_t CharType;
#endif
}


// Dependencies *must* be included first (e.g. URLResponse.h before ViewListener.h)

%include "Logging/Severity.h"
%include "Logging/ILogHandler.h"
%include "Errors.h"
%include "ViewInfo.h"
%include "View.h"
%include "ViewListener.h"
%include "FileHandler.h"
%include "UISystem.h"

%csmethodmodifiers BindCallInternal "private"
%csmethodmodifiers RegisterForEventInternal "private"
%extend Coherent::UI::View {
    void BindCallInternal(const char* name, Coherent::UI::ManagedCallbackAOT handler, int id) {
        $self->BindCall(name, new Coherent::UI::ManagedEventHandlerAOT(handler, id));
    }
    void RegisterForEventInternal(const char* name, Coherent::UI::ManagedCallbackAOT handler, int id) {
        $self->RegisterForEvent(name, new Coherent::UI::ManagedEventHandlerAOT(handler, id));
    }
};

%include "Binding/DotNetBinder.h"

// extend View with additional methods
%extend Coherent::UI::View {
   Coherent::UI::DotNetBinder* GetBinder() {
        return static_cast<Coherent::UI::ViewImpl*>($self)->GetBinder();
   } 
};

#if defined(COHERENT_PLATFORM_ANDROID)
%extend Coherent::UI::View {
   void DispatchKeyEventInternal(int state, int character) {
        static_cast<Coherent::UI::ViewImpl*>($self)->DispatchKeyEvent(state, character);
   } 
};
#elif defined(COHERENT_PLATFORM_IOS)
// keep the interface the same for mobile
%extend Coherent::UI::View {
   void DispatchKeyEventInternal(int state, int character) {} 
};
#endif
