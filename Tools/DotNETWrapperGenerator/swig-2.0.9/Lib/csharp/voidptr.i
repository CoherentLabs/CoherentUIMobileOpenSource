/* -----------------------------------------------------------------------------
 * voidptr.i
 *
 * Typemaps for the void* type
 * These are mapped to a C# IntPtr and are passed around by value.
 *
 * ----------------------------------------------------------------------------- */

%typemap(ctype)  void * "void *"
%typemap(imtype) void * "IntPtr"
%typemap(cstype) void * "IntPtr"
%typemap(csin)   void * "$csinput"
%typemap(in)     void * %{ $1 = $input; %}
%typemap(out)    void * %{ $result = $1; %}
%typemap(csout)  void * { return $imcall; }
%typemap(csdirectorin) void * "$iminput"
%typemap(csdirectorout) void * "$cscall"