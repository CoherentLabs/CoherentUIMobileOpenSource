/* -----------------------------------------------------------------------------
 * wchar.i
 *
 * Typemaps for the wchar_t type
 * These are mapped to a C# String and are passed around by value.
 *
 * Support code for wide strings can be turned off by defining SWIG_CSHARP_NO_WSTRING_HELPER
 *
 * ----------------------------------------------------------------------------- */

#if !defined(SWIG_CSHARP_NO_WSTRING_HELPER)
#if !defined(SWIG_CSHARP_WSTRING_HELPER_)
#define SWIG_CSHARP_WSTRING_HELPER_
%insert(runtime) %{
/* Callback for returning strings to C# without leaking memory */
// COHERENT BEGIN
//typedef void * (SWIGSTDCALL* SWIG_CSharpWStringHelperCallback)(const wchar_t *);
typedef wchar_t * (SWIGSTDCALL* SWIG_CSharpWStringHelperCallback)(const wchar_t *);
// COHERENT END
static SWIG_CSharpWStringHelperCallback SWIG_csharp_wstring_callback = NULL;
%}

%pragma(csharp) imclasscode=%{
  internal class SWIGWStringHelper {

    public delegate string SWIGWStringDelegate(IntPtr message);
// COHERENT BEGIN
    //static SWIGWStringDelegate wstringDelegate = new SWIGWStringDelegate(CreateWString);
	static SWIGWStringDelegate wstringDelegate = (System.Environment.OSVersion.Platform == PlatformID.Unix)
        ? new SWIGWStringDelegate(CreateWStringUTF32)
        : new SWIGWStringDelegate(CreateWStringUnicode);
// COHERENT END

    [DllImport("$dllimport", EntryPoint="SWIGRegisterWStringCallback_$module")]
    public static extern void SWIGRegisterWStringCallback_$module(SWIGWStringDelegate wstringDelegate);

// COHERENT BEGIN
	//	static string CreateWString([MarshalAs(UnmanagedType.LPWStr)]IntPtr cString) {
	//	  return System.Runtime.InteropServices.Marshal.PtrToStringUni(cString);
	//  }
    static string CreateWStringUnicode(IntPtr cString)
    {
        return Marshal.PtrToStringUni(cString);
    }

    static string CreateWStringUTF32(IntPtr cString)
    {
        // Iterate to find string length
        int lengthInBytes = 0;
        while (Marshal.ReadInt32(cString, lengthInBytes) != 0)
        {
            lengthInBytes += 4;
        }
        // Copy bytes
        if (lengthInBytes == 0)
        {
            return string.Empty;
        }
        byte[] stringBytes = new byte[lengthInBytes];

        Marshal.Copy(cString, stringBytes, 0, lengthInBytes);

        return System.Text.Encoding.UTF32.GetString(stringBytes);
    }

	internal static string CreateWString(IntPtr cString) {
        return wstringDelegate(cString);
    }    
// COHERENT END

    static SWIGWStringHelper() {
      SWIGRegisterWStringCallback_$module(wstringDelegate);
    }
  }

  static protected SWIGWStringHelper swigWStringHelper = new SWIGWStringHelper();
%}

%insert(runtime) %{
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterWStringCallback_$module(SWIG_CSharpWStringHelperCallback callback) {
  SWIG_csharp_wstring_callback = callback;
}
%}
#endif // SWIG_CSHARP_WSTRING_HELPER_
#endif // SWIG_CSHARP_NO_WSTRING_HELPER


// wchar_t
%typemap(ctype) wchar_t "wchar_t"
%typemap(imtype) wchar_t "char"
%typemap(cstype) wchar_t "char"

%typemap(csin) wchar_t "$csinput"
%typemap(csout, excode=SWIGEXCODE) wchar_t {
    char ret = $imcall;$excode
    return ret;
  }
%typemap(csvarin, excode=SWIGEXCODE2) wchar_t %{
    set {
      $imcall;$excode
    } %}
%typemap(csvarout, excode=SWIGEXCODE2) wchar_t %{
    get {
      char ret = $imcall;$excode
      return ret;
    } %}

%typemap(in) wchar_t %{ $1 = ($1_ltype)$input; %}
%typemap(out) wchar_t %{ $result = (wchar_t)$1; %}

%typemap(typecheck) wchar_t = char;

// wchar_t *
%typemap(ctype) wchar_t * "wchar_t *"
%typemap(imtype, inattributes="[MarshalAs(UnmanagedType.CHANGEME_TO_LPWSTR_FOR_WIN_OR_CUSTOM_MARSHALER_FOR_POSIX)]", out="CHANGEME_TO_INTPTR_IF_RETVAL_STRING_IF_ARG" ) wchar_t * "string"
%typemap(cstype) wchar_t * "string"

%typemap(csin) wchar_t * "$csinput"
// COHERENT BEGIN
%typemap(csout, excode=SWIGEXCODE) wchar_t * {
	//string ret = System.Runtime.InteropServices.Marshal.PtrToStringUni($imcall);$excode
	System.IntPtr coherentUnmanagedStringPtr = $imcall;
    string ret = $modulePINVOKE.SWIGWStringHelper.CreateWString(coherentUnmanagedStringPtr);
	return ret;
  }
// COHERENT END
%typemap(csvarin, excode=SWIGEXCODE2) wchar_t * %{
    set {
      $imcall;$excode
    } %}
%typemap(csvarout, excode=SWIGEXCODE2) wchar_t * %{
    get {
	// COHERENT BEGIN
      //string ret = $imcall;$excode
	  string ret = $modulePINVOKE.SWIGWStringHelper.CreateWString($imcall);$excode
	// COHERENT END
      return ret;
    } %}

%typemap(in) wchar_t * %{ $1 = ($1_ltype)$input; %}
%typemap(out) wchar_t * %{ $result = (wchar_t *)$1; %}
// COHERENT BEGIN
%typemap(directorin) wchar_t * %{ $input = SWIG_csharp_wstring_callback((const wchar_t *)$1); %}
%typemap(csdirectorin) wchar_t * "$iminput"
%typemap(csdirectorout) wchar_t * "$cscall"
// COHERENT END

%typemap(typecheck) wchar_t * = char *;

