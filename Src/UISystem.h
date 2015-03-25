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

#include <Logging/Severity.h>
#include <Declarations.h>
#include <Errors.h>
#include <CharType.h>

namespace Coherent
{
namespace Logging
{
class ILogHandler;
}

namespace UI
{

namespace Binding
{
	class EventArguments;
}

struct ViewInfo;

/// Abstract interface to listen to System-related events
///
class EventListener
{
public:
	/// Called when the system is fully initialized.
	/// Creating Views is now permitted
	virtual void SystemReady() = 0;
	/// Called when an error in the system occurs
	///
	virtual void OnError(const SystemError& /*error*/) {}
	/// Called when the virtual keyboard is shown or hidden (Android only)
	///
	virtual void OnSoftwareKeyboardVisibilityChanged(bool visible) {}
};

/// Encapsulates the settings of the UISystem
///
struct SystemSettings
{
	SystemSettings()
	: iOS_UseURLCache(true)
	, iOS_URLMemoryCacheSize(4*1024*1024)
	, iOS_URLDiskCacheSize(32*1024*1024)
	{}
	
	/// If Coherent UI should use the URLCache on iOS.
	/// NOTE: It is highly raccomended to let caching on. Disable it only if your application
	/// already sets the URL cache
	bool iOS_UseURLCache;
	
	/// The maximum memory that the URLCache can use
	///
	unsigned iOS_URLMemoryCacheSize;
	
	/// The maximum disk memory the cache is allowed to use
	///
	unsigned iOS_URLDiskCacheSize;
};

class View;
class ViewListenerBase;
class FileHandler;

/// Encapsulates basic UI system functionality
///
class UISystem
{
public:
	/// Frees all resources used by the event system
	/// @warning the UISystem object is unusable after this call
	/// @return true if the system has been properly uninitialized
	virtual bool Uninitialize() = 0;
	
	/// Communicates with the UI system process and performs all updates
	/// @warning Must be called in the thread that owns the system. All registered callbacks will be called in here
	virtual void Update() = 0;
	
	/// Creates a new UI View.
	/// @warning Must be called after the system has signaled that it is ready via `Coherent::UI::EventListener::SystemReady`
	/// @param info an instance of the ViewInfo struct that describes all the parameters of the view
	/// @param path the page to load when the view is created
	/// @param listener an instance of the ViewListenerBase interface to allow listening to all the events raised for this view
	virtual void CreateView(const ViewInfo& info, const CharType* path, ViewListenerBase* listener) = 0;
};

} // UI
} // Coherent

// document the file, so that doxygen will include the documentation of free functions

/// @file

/// Check if the SDK binary version matches the specified one
/// @param sdkVersion the version to check against
/// @return true if the binary SDK version matches, false otherwise
extern "C" COHERENTUI_API bool CheckUISDKVersionExact(int sdkVersion);

/// Check if the SDK binary version is compatible with the specified one
/// @param sdkVersion the version to check against
/// @return true if the binary SDK version is compatible, false otherwise
extern "C" COHERENTUI_API bool CheckUISDKVersionCompatible(int sdkVersion);

/// Initializes the UI system
/// @param sdkVersion the version of the headers; this number is compared to the binary's version and initialization fails if there is a difference.
/// @param licenseKey the license key for this version of the product
/// @param settings encapsulates different system-wide settings
/// @param listener	an instance of a EventListener interface to allow user to listen to UI system events
/// @param logSeverity the severity used to initialized logging - all log messages with levels below this one will be ignored
/// @param logHandler an instance of an ILogHandler interface to allow users to provide their own logging functionality; if nullptr a default implementation will be used that writes logs to a file
/// @param fileHandler an instance of a FileHandler interface to allow users to provide their own file access functionality; if nullptr a default implementation will be used (available only in Full versions of Coherent UI)
/// @return Returns true if the basic initialization succeeds. However creating views is not allowed until the system signals that it is ready (via the EventListener::SystemReady event)
extern "C" COHERENTUI_API Coherent::UI::UISystem* COHERENT_CDECL InitializeUISystem(
	int sdkVersion
	, const char* licenseKey
	, const Coherent::UI::SystemSettings& settings = Coherent::UI::SystemSettings()
	, Coherent::UI::EventListener* listener = nullptr
	, Coherent::Logging::Severity logSeverity = Coherent::Logging::Debug
	, Coherent::Logging::ILogHandler* logHandler = nullptr
	, Coherent::UI::FileHandler* fileHandler = nullptr
	, void* reserved = nullptr
																					);
