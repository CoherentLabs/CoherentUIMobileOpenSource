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
#include <Errors.h>

namespace Coherent
{
namespace UI
{

class View;
class EventArguments;

class URLResponse
{
public:
	/// Allow the URL request to continue
	///
	virtual void AllowRequest() = 0;
	
	/// Deny the URL request
	///
	virtual void DenyRequest() = 0;
	
	/// Redirect the URL request to another URL
	virtual void RedirectRequest(const CharType* newURL) = 0;
};

/// Encapsulates a HTTP header field with it's content
///
class HTTPHeader
{
public:
	HTTPHeader(const char* field, const char* content)
		: Field(field)
		, Content(content)
	{}
	/// The name (key) of the HTTP header field
	///
	const char* Field;
	/// The value of the HTTP header field
	///
	const char* Content;
};

/// Interface all view listeners inherit. For an easier to use interface inherit instead - @see Coherent::UI::ViewListener
///
class ViewListenerBase
{
public:
	virtual ~ViewListenerBase() {}
	virtual void Release() = 0;

	virtual void OnViewCreated(View* view) = 0;
	virtual void OnFinishLoad(int frameId, const CharType* validatedPath, bool isMainFrame, int statusCode, HTTPHeader* headers, unsigned headersCount) = 0;
	virtual void OnFailLoad(int frameId, const CharType* validatedPath, bool isMainFrame, const char* error) = 0;
	virtual void OnURLRequest(const CharType* url, URLResponse* response) = 0;
	virtual void OnReadyForBindings(int frameId, const CharType* path, bool isMainFrame) = 0;
	virtual void OnBindingsReleased(int frameId, const CharType* path, bool isMainFrame) = 0;
	virtual void OnError(const ViewError& error) = 0;
	
	/// Levels of script messages
	///
	enum MessageLevel
	{
		ML_TIP,
		ML_INFO, 
		ML_WARNING,
		ML_ERROR,
	};

	/// Enum containing the possible types of a javascript message
	enum JavaScriptMessageType
	{
		JSMT_Alert,
		JSMT_Confirm,
		JSMT_Prompt,
	};
	
	virtual void OnScriptMessage(MessageLevel level, const CharType* message, const CharType* sourceId, int line) = 0;
	virtual void OnCallback(const char* eventName, const EventArguments& arguments) = 0;
	virtual void OnJavaScriptMessage(const CharType* message, const CharType* defaultPrompt, const CharType* frameUrl, int messageType) = 0;

	/// Possible touch phases
	enum TouchEventPhase
	{
		TEP_Began = 0,
		TEP_Moved,
		TEP_Stationary,
		TEP_Ended,
		TEP_Canceled,
	};

	virtual void OnNewTouchEvent(int fingerId, int phase, int x, int y) = 0;

	virtual void OnBitmapReady(const void* buffer, int size) = 0;
};

/// Interface that allows clients to listen to View events
///
class ViewListener : public ViewListenerBase
{
public:
	virtual ~ViewListener() {}
	
	/// Called when the listener is no longer needed by the UI system
	/// 
	virtual void Release() COHERENT_OVERRIDE  {}
	
	/// Called when the requested View has been created and is ready to use
	/// @param view the instance of the view containing all manipulation methods
	virtual void OnViewCreated(View* view) = 0;
	
	/// Called when a frame has been successfully loaded
	/// @param frameId the id of the loaded frame
	/// @param validatedPath the path loaded in the frame
	/// @param isMainFrame true if this is the main frame of the View
	/// @param statusCode the status code of the response
	/// @param headers an array of header fields
	/// @param headersCount the count of items in the headers array
	virtual void OnFinishLoad(int frameId, const CharType* validatedPath, bool isMainFrame, int statusCode, HTTPHeader* headers, unsigned headersCount) COHERENT_OVERRIDE {}
	
	/// Called when a frame has been failed loading
	/// @param frameId the id of the frame
	/// @param validatedPath the path in the frame
	/// @param isMainFrame true if this is the main frame of the View
	/// @param error error message for the failure
	virtual void OnFailLoad(int frameId, const CharType* validatedPath, bool isMainFrame, const char* error) COHERENT_OVERRIDE {}

	/// Called before an URL request is made. The default implementation allows all requests. **NOTE:** this method will be called only when `View::InterceptURLRequests(true)` has been called
	/// @param url the request URL
	/// @param response response whether to allow the request and may be redirect it. This pointer will be valid only for this call
	virtual void OnURLRequest(const CharType* url, URLResponse* response) COHERENT_OVERRIDE {}
	
	/// Called when a frame is ready for bindings
	/// @param frameId the id of the frame
	/// @param path the path in the frame
	/// @param isMainFrame true if this is the main frame of the view
	virtual void OnReadyForBindings(int frameId, const CharType* path, bool isMainFrame) COHERENT_OVERRIDE {}

	/// Called when the bindings for frame are released
	/// @param frameId the id of the frame
	/// @param path the path in the frame
	/// @param isMainFrame true if this is the main frame of the view
	virtual void OnBindingsReleased(int frameId, const CharType* path, bool isMainFrame) COHERENT_OVERRIDE {}
	
	/// Called when an error occurs for this specific View
	/// @param error error description
	virtual void OnError(const ViewError& error) COHERENT_OVERRIDE {}

	/// Called when a message is sent from a script running in this specific View
	/// @param level message level
	/// @param message the text of the message
	/// @param sourceId id of the script (usually file name)
	/// @param line the number of the line in which the message was sent
	virtual void OnScriptMessage(MessageLevel level, const CharType* message, const CharType* sourceId, int line) COHERENT_OVERRIDE {}

	/// Called by the UI when there is no registered handler for this event
	/// @param eventName name of the event
	/// @param arguments arguments of the event invocation
	virtual void OnCallback(const char* eventName, const EventArguments& arguments) COHERENT_OVERRIDE {}

	/// Called when the view triggered a javascript message box, i.e. an alert, confirmation dialog or a prompt dialog
	/// @param message the JavaScript message
	/// @param defaultPrompt the default value of the prompt text box, in case the message type is prompt
	/// @param frameUrl the URL which created the message
	/// @param messageType the type of the message (alert/confirm/prompt)
	virtual void OnJavaScriptMessage(const CharType* message, const CharType* defaultPrompt, const CharType* frameUrl, int messageType) COHERENT_OVERRIDE {}

	virtual void OnNewTouchEvent(int fingerId, int phase, int x, int y) COHERENT_OVERRIDE {}

	/// Called when the view has been drawn to a bitmap buffer
	/// @param buffer the buffer that contains the bitmap
	/// @param size the number of bytes actually written
	/// @note buffer is the same buffer that has been given to
	/// `View::GetAsBitmap`
	virtual void OnBitmapReady(const void* buffer, int size) COHERENT_OVERRIDE
	{
	}
};
}
}
