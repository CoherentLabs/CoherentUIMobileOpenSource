/* -----------------------------------------------------------------------------
 * coherent_handle.i
 *
 * Typemaps for the Coherent::UI::CoherentHandle type
 *
 * ----------------------------------------------------------------------------- */

%typemap(ctype)  Coherent::UI::CoherentHandle "Coherent::UI::CoherentHandle"
// We can get away with IntPtr for imtype and cstype because currently CoherentHandle
// is used in the public part of the C# library only when it's void*
%typemap(imtype) Coherent::UI::CoherentHandle "IntPtr"
%typemap(cstype) Coherent::UI::CoherentHandle "IntPtr"
%typemap(csin)   Coherent::UI::CoherentHandle "$csinput"
%typemap(in)     Coherent::UI::CoherentHandle %{ $1 = $input; %}
%typemap(out)    Coherent::UI::CoherentHandle %{ $result = $1; %}
%typemap(csout)  Coherent::UI::CoherentHandle { return $imcall; }
%typemap(csdirectorin) Coherent::UI::CoherentHandle "$iminput"
%typemap(csdirectorout) Coherent::UI::CoherentHandle "$cscall"
%typemap(directorin) Coherent::UI::CoherentHandle "$input = (Coherent::UI::CoherentHandle)$1;"
%typemap(directorout) Coherent::UI::CoherentHandle "$result = $input;"