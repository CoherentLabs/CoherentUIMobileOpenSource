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
#pragma once

#include <Declarations.h>

namespace Coherent
{
namespace UI
{
	
enum ViewInputState
{
	VIS_TakeAll = 0, /// Takes all events - no event goes to the client (must still be in the view bounds though)
	VIS_TakeNone, /// Ignores all events - all events go to the client
	VIS_Transparent, /// The client can decide which events to leave on the UI
};

/// Encapsulates the of a View
///
struct ViewInfo
{
	ViewInfo()
		: Parent(nullptr)
		, ShowImmediately(true)
		, InitialInputState(VIS_TakeNone)
		, X(0)
		, Y(0)
		, Width(0)
		, Height(0)
		, SupportClickThrough(true)
		, IsTransparent(false)
		, ScaleToFit(false)
		, EnableWebGLSupport(false)
		, IsSurfaceView(false)
		, ForceSoftwareRendering(false)
		, ResizeOnRotation(false)
		, IgnoreDisplayDensity(false)
		, EnableViewportMetaTag(false)
	{}

	/// The parent UIView* for overlay views. Must be null for texture views.
	///
	void* Parent;
	
	/// Shows the view immediately on load (overlay view only)
	///
	bool ShowImmediately;
	
	/// The initial input state of the view
	///
	ViewInputState InitialInputState;
	
	/// The Width of a View in pixels
	///
	int Width;
	
	/// The Height of a View in pixels
	///
	int Height;
	
	/// The X coordinate of the overlay view in pixels
	///
	int X;
	
	/// The Y coordinate of the overlay view in pixels
	///
	int Y;
	
	/// Defines if the view should support click-through queries. Not supporting click-through increases 
	/// performance slightly.
	bool SupportClickThrough;
	
	/// Defines if a view is transparent or not. A transparent view can be have transparent parts that will
	/// be correctly blended with the client's surfaces. Non-transparent views are always implicitly composited
	/// as if on a white background. Don't use transparent views unless you have transparent parts in the page
	/// because non-transparent view are slightly faster.
	/// @note Do not blend non-transparent views.
	bool IsTransparent;
	
	/// Defines if the view's content will be automatically scaled to fit the size of
	/// the view itself.
	bool ScaleToFit;

	/// If enabled, the view will support WebGL.
	bool EnableWebGLSupport;

	/// Defines whether the view is going to be on a surface
	bool IsSurfaceView;

	/// Force software rendering for the view. Available for Android only.
	bool ForceSoftwareRendering;
	
	/// Defines whether the view will be resized when the device changes
	/// its orientation so as to fit the new screen dimensions. (iOS only)
	bool ResizeOnRotation;

	/// Defines whether the Android device's display density will affect the
	/// scale of the View. When set to *true*, the displayed content in the view
	/// will match its pixel size. For example, a device with 1.5 scale factor
	/// will display a div of 100px in a 200px View on about 75% of surface
	/// if IgnoreDisplayDensity is set to false.
	/// @note Avalable for Android only.
	bool IgnoreDisplayDensity;

	/// Defines whether the <meta name="viewport" ...> tag is enabled
	/// @note Avalable for Android only.
	bool EnableViewportMetaTag;
};
}
}
