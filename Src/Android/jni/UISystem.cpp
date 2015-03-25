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
#include "precompiled.h"


#include <UISystem.h>

#include <Android/jni/ViewImpl.h>
#include <ViewListener.h>
#include <Android/jni/ResourceResponseImpl.h>
#include <Android/jni/ViewObserver.h>
#include <Android/jni/ManagedReferences.h>
#include <Android/jni/StringConversions.h>

#include "com_coherentlabs_ui_CoherentWebChromeClient.h"
#include "com_coherentlabs_ui_CoherentWebViewClient.h"

#include <Android/jni/CriticalSection.h>

#include <unistd.h>

#if defined(COHERENT_UNITY3D_BUILD)
#include <Android/jni/UnityLogHandlerDecorator.h>
#endif

using Coherent::CustomString;

#define GET_VIEW_BY_ID(view, id, ...) \
	COHERENT_ASSERT( Coherent::UI::g_System != nullptr, \
						"System is NULL, while trying to get a View", ""); \
	if (Coherent::UI::g_System == nullptr) return __VA_ARGS__; \
	Coherent::UI::ViewImpl* view = \
		Coherent::UI::g_System->FindViewByID(id & 0xFF); \
	COHERENT_ASSERT(view != nullptr, "Requested view is null", ""); \
	if (view == nullptr) return __VA_ARGS__; \


namespace Coherent { namespace UI {

class UISystemDefaultLogHandler : public Logging::ILogHandler
{
public:
	virtual ~UISystemDefaultLogHandler() {}

	virtual void WriteLog(Logging::Severity severity, const char* message, size_t length) override
	{
		switch(severity)
		{
			case Logging::Trace:
				LOGV("Coherent UI: %s", message);
				break;
			case Logging::Debug:
				LOGD("Coherent UI: %s", message);
				break;
			case Logging::Info:
				LOGI("Coherent UI: %s", message);
				break;
			case Logging::Warning:
				LOGW("Coherent UI: %s", message);
				break;
			case Logging::AssertFailure:
				LOGW("Coherent UI: AssertFailure: %s", message);
				break;
			case Logging::Error:
				LOGE("Coherent UI: %s", message);
				break;
			default:
				break;
		}
	}
	
	virtual void Assert(const char* message) override
	{
		assert(false);
	}
};

class LoggingScope
{
public:
	LoggingScope()
		: m_SuppliedLogHandler(nullptr)
	{}
	
	void Initialize(Logging::Severity severity, Logging::ILogHandler* handler)
	{
		m_SuppliedLogHandler = handler;
		bool ownLogHandler = (m_SuppliedLogHandler == nullptr);
		if (ownLogHandler)
		{
			handler = new UISystemDefaultLogHandler();
		}
#if defined(COHERENT_DOT_NET)
		else
		{
			Coherent::UI::ManagedReferences::AddReference(m_SuppliedLogHandler);
		}
#endif

	#if defined(COHERENT_UNITY3D_BUILD)
		handler = new UnityLogHandlerDecorator(handler, ownLogHandler);
		ownLogHandler = true;
	#endif
		
		Coherent::Logging::Initialize(severity, handler, ownLogHandler);
	}
	
	~LoggingScope()
	{
		Coherent::Logging::Uninitialize();
#if defined(COHERENT_DOT_NET)
		if (m_SuppliedLogHandler)
		{
			Coherent::UI::ManagedReferences::ReleaseReference(m_SuppliedLogHandler);
		}
#endif
	}
private:
	Logging::ILogHandler* m_SuppliedLogHandler;
};
	
class System : public UISystem, public ViewObserver
{
public:
	System();
	virtual ~System();
	
	bool Initialize(const char* license, const SystemSettings& settings, EventListener* listener, Logging::Severity severity, Logging::ILogHandler* logHandler, FileHandler* fileHandler);
	
	virtual bool Uninitialize() override;
	virtual void Update() override;
	virtual void CreateView(const ViewInfo& info, const CharType* path, ViewListenerBase* listener) override;
	
	virtual void OnViewDestroyed(unsigned int viewId) override;

	jbyteArray RequestRead(const CustomString& url);

	ViewImpl* FindViewByID(unsigned int id);
	EventListener* GetEventListener() const;
	
	void PostCall(const std::function<void()>& call);
	void PostCallForView(const std::function<void()>& call, int viewId);
		
private:
	std::vector<ViewImpl*> m_ActiveViews;
	unsigned int m_NextViewId;
	
	EventListener* m_Listener;
	SystemSettings m_Settings;
	
	LoggingScope m_Logging;
	
	FileHandler* m_FileHandler;
	
	bool m_HasNotifiedReady;

	static const int VIEW_ID_NONE = -1;
	struct JNICall
	{
		JNICall(const std::function<void()>& function, int viewId)
			: Function(function)
			, ViewID(viewId)
		{
		}

		JNICall(JNICall&& other)
			: Function(std::move(other.Function))
			, ViewID(std::move(other.ViewID))
		{
		}

		JNICall& operator= (JNICall&& other)
		{
			std::swap(Function, other.Function);
			std::swap(ViewID, other.ViewID);
		}

		std::function<void()> Function;
		int ViewID;

	private:
		JNICall(const JNICall&);
		JNICall& operator= (const JNICall&);
	};
	std::queue<JNICall> m_JNICalls;
	PlatformCriticalSection m_CriticalSection;
};

System* g_System = nullptr;
	
System::System()
	: m_NextViewId(0)
	, m_Listener(nullptr)
	, m_HasNotifiedReady(false)
	, m_FileHandler(nullptr)
{}

System::~System()
{}
	
bool System::Initialize(const char* license, const SystemSettings& settings, EventListener* listener, Logging::Severity severity, Logging::ILogHandler* logHandler, FileHandler* fileHandler)
{
	m_Logging.Initialize(severity, logHandler);

	m_Settings = settings;
	m_Listener = listener;
	
	m_FileHandler = fileHandler;
	
	COHERENT_INFO("Initializing Coherent UI ver. ", COHERENT_UI_MAJOR_VERSION, ".", COHERENT_UI_MINOR_VERSION, ".", COHERENT_UI_PATCH_VERSION, ".", COHERENT_UI_BUILD_VERSION);
	COHERENT_DEBUG("Is using default log handler {", logHandler == nullptr ? "yes" : "no", "}");
	COHERENT_DEBUG("Is using default file handler {", fileHandler == nullptr? "yes" : "no", "}");

	return true;
}

bool System::Uninitialize()
{
	COHERENT_INFO("Uninitializing Coherent UI");
	
	// Destory all remaining views (CAUTION - we delete the views from the collection as we iterate)
	auto viewsCopy = m_ActiveViews;
	std::for_each(viewsCopy.begin(), viewsCopy.end(), [](ViewImpl* view) { view->Destroy(); });
	
	COHERENT_INFO("Coherent UI Uninitialized");
	
	g_System = nullptr;
	delete this;
	return true;
}
	
void System::Update()
{
	if (!m_HasNotifiedReady)
	{
		if (m_Listener)
		{
			m_Listener->SystemReady();
		}
		m_HasNotifiedReady = true;
	}

	#if defined(COHERENT_UNITY3D_BUILD)
		UnityLogHandlerDecorator::FetchLogMessages();
	#endif

	{
		CriticalSectionLock lock(m_CriticalSection);
		for (size_t i = 0, currentCallsCount = m_JNICalls.size();
			i < currentCallsCount; ++i)
		{
			JNICall jniCall = std::move(m_JNICalls.front());
			m_JNICalls.pop();

			if (jniCall.ViewID != VIEW_ID_NONE)
			{
				GET_VIEW_BY_ID(view, jniCall.ViewID)

				if (view == nullptr)
				{
					continue;
				}

				if (view->IsReady())
				{
					jniCall.Function();
				}
				else
				{
					// Re-queue the call until the view is ready
					m_JNICalls.push(std::move(jniCall));
				}
			}
			else
			{
				jniCall.Function();
			}
		}
	}
	std::for_each(m_ActiveViews.begin(), m_ActiveViews.end(), [](ViewImpl* view) { view->Update(); } );
}
	
void System::CreateView(const ViewInfo& info, const CharType* path, ViewListenerBase* listener)
{
	COHERENT_DEBUG("Starting view creation with options:");
	COHERENT_DEBUG("Parent {", (size_t)info.Parent, "}");
	COHERENT_DEBUG("X {", info.X, "}");
	COHERENT_DEBUG("Y {", info.Y, "}");
	COHERENT_DEBUG("Width {", info.Width, "}");
	COHERENT_DEBUG("Height {", info.Height, "}");
	COHERENT_DEBUG("SupportClickThrough {", info.SupportClickThrough, "}");
	COHERENT_DEBUG("IsTransparent {", info.IsTransparent, "}");
	COHERENT_DEBUG("Path {", path ? path : reinterpret_cast<const CharType*>(L""), "}");
	
	ViewImpl* newView = new ViewImpl(m_NextViewId++, info, listener, this);
	
	bool usesDefaultFileHandler = (m_FileHandler == nullptr);
	if(!newView->Initialize(path, usesDefaultFileHandler)) {
		delete newView;
		
		ViewError error = { VE_FailedCreation, "Unable to create view" };
		listener->OnError(error);
		return;
	}

	m_ActiveViews.push_back(newView);
	
	COHERENT_INFO("Created view ", m_NextViewId-1);
}
	
void System::OnViewDestroyed(unsigned int viewId)
{
	auto it = std::find_if(m_ActiveViews.begin(), m_ActiveViews.end(), [viewId](ViewImpl* view) { return viewId == view->GetId(); });
	
	if (it == m_ActiveViews.end())
	{
		COHERENT_WARNING("Destroying unregistered view!");
		return;
	}

	auto theView = *it;
	
	COHERENT_INFO("Destroyed view ", theView->GetId());
	
	m_ActiveViews.erase(it);
	
	delete theView;
}

ViewImpl* System::FindViewByID(unsigned int viewId)
{
	auto it = std::find_if(m_ActiveViews.begin(), m_ActiveViews.end(), [viewId](ViewImpl* view) { return viewId == view->GetId(); });
	
	if (it == m_ActiveViews.end())
	{
		return nullptr;
	}

	return *it;
}

EventListener* System::GetEventListener() const
{
	return m_Listener;
}

jbyteArray System::RequestRead(const CustomString& url)
{
	// This is called from the I/O thread that invoked java.io.InputStream.read
	// and is in the same call stack.

	ResourceResponseImpl* response(new ResourceResponseImpl);
	
	COHERENT_DEBUG("Requesting read of resource: ", url);
	
	m_FileHandler->ReadFile(url.c_str(), response);

	if (!response->IsResponseReady())
	{
		ResourceResponseSynchronizationObject* syncObject = response->GetSyncObject();
		COHERENT_TRACE("Waiting for response for resource read for \"", url, "\".");
		COHERENT_ASSERT(syncObject != nullptr, "Resource response sync object is null", "Resource response sync object cannot be null!");
		pthread_mutex_lock(&syncObject->Mutex);
		pthread_cond_wait(&syncObject->ConditionVariable, &syncObject->Mutex);
		pthread_mutex_unlock(&syncObject->Mutex);
	}

	jbyteArray result = nullptr;

	if (response->HasFailed())
	{
		COHERENT_ERROR("Resource read for \"", url, "\" failed.");
	}
	else
	{
		COHERENT_TRACE("Resource read for \"", url, "\" succeeded.");
	}

	if (!response->HasFailed())
	{
		GET_JNIENV(env);
		result = env->NewByteArray(response->GetDataLength());
		if (result != nullptr)
		{
			env->SetByteArrayRegion(result, 0, response->GetDataLength(), (const jbyte*)response->GetData());
		}
		else
		{
			COHERENT_ERROR("Unable to allocate managed array of size ", response->GetDataLength(), " for \"", url, "\"");
		}
	}
	
	delete response;

	return result;
}

void System::PostCall(const std::function<void()>& call)
{
	PostCallForView(call, VIEW_ID_NONE);
}

void System::PostCallForView(const std::function<void()>& call, int viewId)
{
	CriticalSectionLock lock(m_CriticalSection);
	m_JNICalls.push(JNICall(call, viewId));
}

}}

//------------------------------------------------------------------------------------
// Java -> C++ notifications

#ifdef __cplusplus
extern "C" {
#endif

static std::string GetStringFromJString(JNIEnv* env, jstring str)
{
	if(!str)
	{
		return std::string();
	}
	const char* utfStr = (const char*)env->GetStringUTFChars(str, nullptr);
	const jsize utfLen = env->GetStringUTFLength(str);
	std::string result(utfStr, utfLen);
	env->ReleaseStringUTFChars(str, utfStr);
	return result;
}

static CustomString GetCustomStringFromJString(JNIEnv* env, jstring str)
{
	CustomString result;
	if(!str)
	{
		return result;
	}
#ifdef COHERENT_UNITY3D_BUILD
	result = GetStringFromJString(env, str);
#else
	const char* utfStr = (const char*)env->GetStringUTFChars(str, nullptr);
	const jsize utfLen = env->GetStringUTFLength(str);
	utf8::utf8to32(utfStr, utfStr + utfLen, std::back_inserter(result));
	env->ReleaseStringUTFChars(str, utfStr);
#endif
	return result;
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebChromeClient_notifyOnJSAlert
  (JNIEnv * env, jobject, jint id, jstring url, jstring message)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	CustomString msg = GetCustomStringFromJString(env, message);

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::JavaScriptMessageReceived, view,
			msg, CustomString(), frameUrl,
			Coherent::UI::ViewListenerBase::JSMT_Alert),
		id);
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebChromeClient_notifyOnJSConfirm
  (JNIEnv * env, jobject, jint id, jstring url, jstring message)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	CustomString msg = GetCustomStringFromJString(env, message);

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::JavaScriptMessageReceived, view,
			msg, CustomString(), frameUrl,
			Coherent::UI::ViewListenerBase::JSMT_Confirm),
		id);
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebChromeClient_notifyOnJSPrompt
  (JNIEnv * env, jobject, jint id, jstring url, jstring message, jstring defaultValue)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	CustomString msg = GetCustomStringFromJString(env, message);
	CustomString defaultPromptValue = GetCustomStringFromJString(env, defaultValue);

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::JavaScriptMessageReceived, view,
			msg, defaultPromptValue, frameUrl,
			Coherent::UI::ViewListenerBase::JSMT_Prompt),
		id);
}


JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebViewClient_notifyOnPageStarted
  (JNIEnv * env, jobject, jint id, jstring url)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	view->DidStartLoad(frameUrl.c_str());
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebViewClient_notifyOnPageError
  (JNIEnv * env, jobject, jint id, jstring url, jint errorCode, jstring description)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	std::string reason = GetStringFromJString(env, description);

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::LoadFailed, view, frameUrl, reason),
		id);
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebViewClient_notifyOnPageFinished
  (JNIEnv * env, jobject, jint id, jstring url)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	
	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::DidFinishLoad, view, frameUrl), id);
}

JNIEXPORT jboolean JNICALL Java_com_coherentlabs_ui_CoherentWebViewClient_shouldStartLoad
  (JNIEnv * env, jobject, jint id, jstring url)
{
	GET_VIEW_BY_ID(view, id, false);

	CustomString frameUrl = GetCustomStringFromJString(env, url);

	bool shouldStartLoad;

	Coherent::UI::ViewSynchronizationObject* syncObject = view->GetSyncObject();
	if(!syncObject)
	{
		COHERENT_ASSERT(syncObject != nullptr, "View synchronization object is null", "View synchronization object should not be null!");
		return false;
	}
	else
	{
		if (getpid() != gettid())
		{
			Coherent::UI::g_System->PostCallForView(
				std::bind(&Coherent::UI::ViewImpl::ShouldStartLoad, view,
					frameUrl, std::ref(shouldStartLoad)),
				id);

			pthread_mutex_lock(&syncObject->Mutex);
			pthread_cond_wait(&syncObject->ConditionVariable,
				&syncObject->Mutex);
			pthread_mutex_unlock(&syncObject->Mutex);
		}
		else
		{
			// Invoke the call immediately when on the main thread
			view->ShouldStartLoad(frameUrl, shouldStartLoad);
		}
	}

	return shouldStartLoad;
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentWebViewClient_triggerNativeCode
  (JNIEnv *env, jobject, jint id, jstring url)
{
	GET_VIEW_BY_ID(view, id);

	CustomString frameUrl = GetCustomStringFromJString(env, url);
	Coherent::UI::BinderAndroid* viewBinder = view->GetBinder();
	COHERENT_ASSERT(viewBinder != nullptr, "Android view binder is null", "Android view binder must not be null!");

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::BinderAndroid::TriggerNative, viewBinder,
			frameUrl),
		id);
}

JNIEXPORT jbyteArray JNICALL Java_com_coherentlabs_ui_CoherentInputStream_readAssetInMemory
  (JNIEnv * env, jobject, jstring url)
{
	CustomString frameUrl = GetCustomStringFromJString(env, url);

	COHERENT_ASSERT(Coherent::UI::g_System != nullptr, "Coherent UI System is null", "System must not be null!");
	return Coherent::UI::g_System->RequestRead(frameUrl);
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentJavaScriptInterface_addCoherentTouchEvent
  (JNIEnv * env, jobject, jint id, jint fingerId, jint phase, jint x, jint y)
{
	GET_VIEW_BY_ID(view, id);

	Coherent::UI::g_System->PostCallForView(
		std::bind(&Coherent::UI::ViewImpl::AddNewTouchEvent, view, fingerId,
			phase, x, y),
		id);
}

JNIEXPORT void JNICALL Java_com_coherentlabs_ui_CoherentAndroidViewBridge_notifySoftKeyboardVisible
  (JNIEnv * env, jobject, jboolean visible)
{
	COHERENT_ASSERT(Coherent::UI::g_System != nullptr, "Coherent UI System is null", "System must not be null!");
	Coherent::UI::EventListener* eventListener = Coherent::UI::g_System->GetEventListener();
	if (eventListener)
	{
		Coherent::UI::g_System->PostCall(std::bind(
			&Coherent::UI::EventListener::OnSoftwareKeyboardVisibilityChanged,
			eventListener, visible));
	}
	else
	{
		COHERENT_DEBUG("System event listener shouldn't be null!");
	}
}

/*
 * Class:     com_coherentlabs_ui_CoherentAndroidViewBridge
 * Method:    notifyOnBitmapReady
 * Signature: (ILjava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL
	Java_com_coherentlabs_ui_CoherentAndroidViewBridge_notifyOnBitmapReady(
		JNIEnv* env,
		jobject,
		jint id,
		jobject buffer,
		jint size)
{
	GET_VIEW_BY_ID(view, id);

	auto data = env->GetDirectBufferAddress(buffer);
	Coherent::UI::g_System->PostCallForView(std::bind(
		&Coherent::UI::ViewImpl::NotifyBitmapReady, view, data, size), id);
}

#ifdef __cplusplus
}
#endif


//------------------------------------------------------------------------------------

bool CheckUISDKVersionExact(int sdkVersion)
{
	return sdkVersion == COHERENT_UI_SDK_VER;
}

bool CheckUISDKVersionCompatible(int sdkVersion)
{
	// After masking with this, the version has its build number ignored
	const int COUI_SDK_VER_COMPATIBILITY_MASK = ~((1 << COHERENT_BITS_PER_VERSION) - 1);

	return (sdkVersion & COUI_SDK_VER_COMPATIBILITY_MASK) == (COHERENT_UI_SDK_VER & COUI_SDK_VER_COMPATIBILITY_MASK);
}

Coherent::UI::UISystem* InitializeUISystem(  int sdkVersion
										   , const char* licenseKey
										   , const Coherent::UI::SystemSettings& settings
										   , Coherent::UI::EventListener* listener
										   , Coherent::Logging::Severity logSeverity
										   , Coherent::Logging::ILogHandler* logHandler
										   , Coherent::UI::FileHandler* fileHandler
										   , void*
										   )
{
	if (!CheckUISDKVersionCompatible(sdkVersion))
	{
		LOGE("SDK Binary version (%d) is not compatible with the initialization one (%d)!", COHERENT_UI_SDK_VER, sdkVersion);
		return nullptr;
	}
	else if (!CheckUISDKVersionExact(sdkVersion))
	{
		LOGW("SDK Binary version (%d) is compatible with the initialization one (%d) but does not match!", COHERENT_UI_SDK_VER, sdkVersion);
	}

	auto system = new Coherent::UI::System;
	if(!system->Initialize(licenseKey, settings, listener, logSeverity, logHandler, fileHandler))
	{
		system->Uninitialize();
		return nullptr;
	}
	Coherent::UI::g_System = system;
	return system;
}
