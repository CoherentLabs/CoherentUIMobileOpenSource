#Samples

The samples provide a starting point for you. They are located in
`Assets/CoherentUI/Samples` and the required scripts are already
configured.

##Mobile Input

This sample demonstrates how you can control what part of the input is forwarded
to Unity3D by using JavaScript. The sample starts with an overlay that has 3
buttons on the left side, area for testing input forwarding on the right, and a
red box in the world. The buttons enable/disable the input forwarding for touches
on the right side of the overlay and the third one bumps the box upwards.
When you touch the box a little force is applied and it should move forward as if
you pushed it. If you touch the box in the right area of the overlay the force is
applied only if input forwarding is enabled.

There are also buttons for toggling the overlay and mouse look controller in the
top left corner for convenience.

**Warning:** The HTML code for the sample is not designed for small displays so it may
look out of proportion on a phone or a phone simulator. You can try lowering the
font size in the accompanying *css* file.

**Note:** The Coherent UI View used has its input state set to "Transparent". If you
set it to "Take all" input is consumed before reaching Unity3D and if you set it
to "Take none" it is sent directly to Unity3D.

**Warning:** Unfortunately, at the moment you can't see the actual behavior of the
sample in the Unity3D Editor. Input is handled differently for the standalone
and mobile versions so it recommended that you test your project either on a
mobile device or an emulator.

Code-wise speaking, when you make a touch on a "Transparent" Coherent UI View,
the `engine.checkClickThrough` function is called. You can check its code in
`coherent.js`. Basically it obtains the DOM element from the touch coordinates
and begins a series of checks. First, if the element is the *document body* or
has the **coui-noinput** class, input is directly sent to the client (Unity3D).
Otherwise, the element is checked for having a **coui-inputcallback** class.
If it does, the element's `couiInputCallback` is invoked which determines
whether the input is forwarded or consumed; if it doesn't have such class,
input is consumed.

The sample enables/disables the input forwarding on the right area by
removing/adding the **coui-noinput** class to the corresponding DOM element.
In this sample, only the DOM element for the right area has the class
**coui-inputcallback** and its `couiInputCallback` function is set when the
engine has signaled it's ready. The function itself doesn't do anything special,
it just always `returns true`, meaning that the input is consumed in JavaScript
and never sent to the client (Unity3D).

You can also bump the box upwards which is a demonstration for the binding
for Coherent UI Mobile. It's very much the same as binding for .NET/Unity3D standalone so
we'll not go in detail here.

Summing up:

   - if you want the input forwarded to the client when touching an element,
   simply add the **coui-noinput** class to the element.
   - if you want an element to consume input, ensure that it does
   **not** have the **coui-noinput**.
   - if you want to have custom processing over an element, ensure that it
   does **not** have the **coui- noinput**, add a **coui-inputcallback**,
   and add a function `couiInputCallback` which ultimately returns true if
   you want to consume the input or false if you want to forward it to the client.

Note that for obtaining the element below the touch point we're currently using
`document.elementFromPoint`. In the sample, the right area is represented by a
`&lt;div&gt;` and there's some text inside it as a child element. Only the
`&lt;div&gt;` has the `coui-inputcallback` class.

If you touch the text its
element will be checked for the `coui-inputcallback` class and since it doesn't
have one input will be consumed. Since we want to apply the logic for all
touches inside the area this presents a problem.

One solution is to add the
`coui-inputcallback` class to all child elements and set their
`couiInputCallback` functions to the same variable. Another solution is to use
the **pointer-events** CSS property on the children elements, e.g.:

 	#menu-right > *
	{
		pointer-events: none;
	}

This is how the sample solves the problem. Note that this is an experimental
CSS property and prevents elements from being the target of pointer events.
This is fine in the sample but may have adverse effects in your code
so use it with caution.

This is basically all you need to draw the HUD. Hit play to see the result

##Mobile Surface

The Mobile Surfaces sample shows how to use Coherent UI views on surfaces for
mobile platforms.

The sample shows a texture with a Coherent UI view on it that
is rotating. Every 100 frames the view is changed and the texture is updated by
calling the `UpdateView` method of the view.

**Note:** To achive transparency of the surface **both** - the *IsTransparent*
property of the view should be checked **and** the shader of the material should
be set to *Transparent/Diffuse*.
