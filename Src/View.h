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
#include <ViewInfo.h>
#include <ScriptEventWriter.h>
#include <Errors.h>


namespace Coherent
{
namespace UI
{
	
class IEventHandler;

/// Class that encapsulates a UI View
/// Provides all manipulation and query functions for the view
class View
{
public:
	/// Destroys this view.
	/// @warning After a call to Destroy calling any other method except the
	/// destructor results in undefined behavior
	virtual void Destroy() = 0;
	
	/// Gets the width of the View
	/// @return the width of the View
	virtual int GetWidth() const = 0;
	
	/// Gets the height of the View
	/// @return the height of the View
	virtual int GetHeight() const = 0;
	
	/// Shows/hides an overlay view
	///
	virtual void SetShow(bool show) = 0;
	
	/// Tells if an overlay view is currently showing
	///
	virtual bool IsShowing() const = 0;
	
	/// Sends a move event to the UI renderer
	/// @note move will only work for overlay views
	/// @param x the new x coordinate of the origin of the view (in pixels)
	/// @param y the new y coordinate of the origin of the view (in pixels)
	virtual void Move(unsigned x, unsigned y) = 0;
	
	/// Send a resize event to the UI renderer.
	/// @note Resize will result in multiple calls to CreateBuffers and DestoryBuffers in the listener
	/// @param width the new width of the View (in pixels)
	/// @param height the new height of the View (in pixels)
	virtual void Resize(unsigned width, unsigned height) = 0;
							
	/// Tells if this view is transparent or not
	/// @return true if the view is transparent
	virtual bool IsTransparent() const = 0;
	
	/// Tells if this view supports click-through queries or not
	/// @return true if click-through is supported
	virtual bool SupportsClickThrough() const = 0;
				
	/// Loads a new path in the View
	/// @param path the path to load
	virtual void Load(const CharType* path) = 0;
	
	/// Reloads the last requested or navigated-to path
	/// @param ignoreCache whether to ignore the cache for the reload
	virtual void Reload(bool ignoreCache) = 0;
	
	/// Returns if the path has been completely loaded with all resources
	/// @return if everything loaded
	virtual bool HasLoadedEverything() const = 0;
	
	/// Get the last requested or navigated-to path
	/// @return the path as null-terminated string
	virtual const CharType* GetCurentViewPath() const = 0;
	
	/// Get the last requested path to load for the View
	/// @return the path as null-terminated string
	/// @warning the pointer is guaranteed to be valid only until the next UI System update
	virtual const CharType* GetLastRequestedPath() const = 0;
	
	/// Get the last successfully loaded path for the View
	/// @return the path as null-terminated string
	/// @warning the pointer is guaranteed to be valid only until the next UI System update
	virtual const CharType* GetLastLoadedPath() const = 0;
	
	/// Get the last failed path for the View
	/// @return the path as null-terminated string
	/// @warning the pointer is guaranteed to be valid only until the next UI System update
	virtual const CharType* GetLastFailedPath() const = 0;

	/// Get the view as bitmap
	/// @param buffer to hold the bitmap
	/// @param size of the buffer in bytes
	/// @return true if it is possible to draw the bitmap, false otherwise
	/// @note calling GetAsBitmap again before OnBitmapReady has been called
	/// will result in error and false will be returned from the method
	virtual bool GetAsBitmap(void* buffer, int size) const = 0;

	/// Expose C++ handler to be called when a specific event occurs
	/// @param name name of the event
	/// @param handler handler to be executed
	virtual void RegisterForEvent(const char* name, IEventHandler* handler) = 0;
	
	/// Expose C++ handler to be called from UI
	/// @param name name for the handler in the UI
	/// @param handler handler to be executed
	virtual void BindCall(const char* name, IEventHandler* handler) = 0;
	
	virtual const unsigned int& GetId() const = 0;
	
	/// Execute arbitrary JavaScript code
	/// @param script code to be evaluated in the context of the main frame of the view
	virtual void ExecuteScript(const char* script) = 0;
				
	/// Controls intercepting URL requests. For each request `ViewListener::OnURLRequest` will be called.
	/// @param intercept whether to intercept all URL requests
	/// @warning Intercepting all URL requests will have negative impact of the loading speed of the view
	virtual void InterceptURLRequests(bool intercept) = 0;
	
	/// Sets an error for the currently executing handler
	/// @param error the type of the error
	/// @param message the error message
	virtual void SetScriptError(ScriptCallErrorType error,
								const char* message) = 0;

	/// Navigates back to the previous patj, if any
	///
	virtual void GoBack() = 0;
	
	/// Navigates to the next path, if any (i.e. after navigating back)
	///
	virtual void GoForward() = 0;
	
	/// Sets the input state of the view (for overlay views only)
	/// @param state the new state of the input
	virtual void SetInputState(ViewInputState state) = 0;
	
	/// Gets the current state of the input
	/// @return the current input state of the view
	virtual ViewInputState GetInputState() const = 0;
       
    /// @name Triggering events
	/// @{
	/// These overloads trigger an event in JavaScript with the supplied arguments
	
	/// Triggers a UI event
	///
	virtual void TriggerEvent(const char* name) = 0;
    
	/// Triggers a UI event
	///
	template <typename T1>
	void TriggerEvent(const char* name, const T1& argument1)
	{
		ScriptEventWriter* writer = GetScriptEventWriter(this);
		Binder* binder = writer->GetBinder();
        
		writer->BeginEvent(name, 1);
		
		CoherentBindInternal(binder, argument1);
		
		writer->EndEvent();
	}
    
	/// Triggers a UI event
	///
	template <typename T1, typename T2>
	void TriggerEvent(const char* name, const T1& argument1, const T2& argument2)
	{
		ScriptEventWriter* writer = GetScriptEventWriter(this);
		Binder* binder = writer->GetBinder();
        
		writer->BeginEvent(name, 2);
		
		CoherentBindInternal(binder, argument1);
		CoherentBindInternal(binder, argument2);
		
		writer->EndEvent();
	}
    
	/// Triggers a UI event
	///
	template <typename T1, typename T2, typename T3>
	void TriggerEvent(const char* name, const T1& argument1, const T2& argument2, const T3& argument3)
	{
		ScriptEventWriter* writer = GetScriptEventWriter(this);
		Binder* binder = writer->GetBinder();
        
		writer->BeginEvent(name, 3);
		
		CoherentBindInternal(binder, argument1);
		CoherentBindInternal(binder, argument2);
		CoherentBindInternal(binder, argument3);
		
		writer->EndEvent();
	}
    
	/// Triggers a UI event
	///
	template <typename T1, typename T2, typename T3, typename T4>
	void TriggerEvent(const char* name, const T1& argument1, const T2& argument2, const T3& argument3, const T4& argument4)
	{
		ScriptEventWriter* writer = GetScriptEventWriter(this);
		Binder* binder = writer->GetBinder();
        
		writer->BeginEvent(name, 4);
        
		CoherentBindInternal(binder, argument1);
		CoherentBindInternal(binder, argument2);
		CoherentBindInternal(binder, argument3);
		CoherentBindInternal(binder, argument4);
		
		writer->EndEvent();
	}
	/// @}

	
};
}
	
}
