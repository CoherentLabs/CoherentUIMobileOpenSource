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

#include <Android/jni/ViewImpl.h>
#include <ViewListener.h>
#include <Android/jni/ViewObserver.h>

static const int COHERENT_VIEW_ID_SALT = 0xFF00;

namespace Coherent { namespace UI {

class URLResponseImpl : public URLResponse
{
public:
	URLResponseImpl() : ShouldContinue(true)
	{}

	virtual void AllowRequest() override
	{
		ShouldContinue = true;
	}
	
	virtual void DenyRequest() override
	{
		ShouldContinue = false;
	}
	
	virtual void RedirectRequest(const CharType* newURL) override
	{
		ShouldContinue = true;
		NewPath = newURL;
	}

	bool ShouldContinue;
	CustomString NewPath;
};

ViewImpl::ViewImpl(unsigned int myId, const ViewInfo& info, ViewListenerBase* listener, ViewObserver* observer)
	: m_Id(myId)
	, m_Info(info)
	, m_IsShowing(info.ShowImmediately)
	, m_Listener(listener)
	, m_Observer(observer)
	, m_InterceptRequests(false)
	, m_HasNotifiedReady(false)
	, m_InputState(info.InitialInputState)
	, m_IsLoading(false)
	, m_BitmapBuffer(nullptr)
	, m_ByteBuffer(nullptr)
	, m_ClientBitmap(nullptr)
	, m_ClientBitmapSize(0)
{}

ViewImpl::~ViewImpl()
{}
	
bool ViewImpl::Initialize(const CharType* path, bool useDefaultFileHandler)
{
	GET_JNIENV(env);

	jobject obj_Activity;
#if defined(COHERENT_UNITY3D_BUILD) || defined(COHERENT_UI_UNITY3D_MOBILE_BUILD)
	if (m_Info.Parent != nullptr)
	{
		COHERENT_DEBUG("ViewInfo::Parent must be null for Unity3D! It will be set to null and ignored.");
		m_Info.Parent = nullptr;
	}	

	jclass cls_UnityPlayer = env->FindClass("com/unity3d/player/UnityPlayer");
	jfieldID fid_UnityPlayer_currentActivity = env->GetStaticFieldID(cls_UnityPlayer, "currentActivity", "Landroid/app/Activity;");
	obj_Activity = env->GetStaticObjectField(cls_UnityPlayer, fid_UnityPlayer_currentActivity);
#else
	COHERENT_ASSERT(m_Info.Parent != nullptr, "ViewInfo::Parent must be set to the Java object representing the activity!", "ViewInfo::Parent must NOT be null!");
	obj_Activity = reinterpret_cast<jobject>(m_Info.Parent); 
#endif

	jmethodID mid_CoherentView = env->GetMethodID(cls_CoherentView, "<init>", "(Landroid/app/Activity;)V");
	jobject obj_CoherentView = env->NewObject(cls_CoherentView, mid_CoherentView, obj_Activity);
	m_CoherentView = env->NewGlobalRef(obj_CoherentView);

	jmethodID mid_CoherentView_Create = env->GetMethodID(cls_CoherentView,
			"createView", "(IIIZZZZZZZLjava/lang/String;)V");
	
	jstring str_Path = Coherent::CreateJStringFrom(env, path);

	env->CallVoidMethod(m_CoherentView,
						mid_CoherentView_Create,
						m_Info.Width,
						m_Info.Height,
						COHERENT_VIEW_ID_SALT | m_Id,
						m_Info.ShowImmediately,
						m_Info.IsTransparent,
						useDefaultFileHandler,
						m_Info.IsSurfaceView,
						m_Info.ForceSoftwareRendering,
						m_Info.IgnoreDisplayDensity,
						m_Info.EnableViewportMetaTag,
						str_Path);

	m_ViewBinder.SetView(this, m_Id, m_Listener);

	if (m_Info.X != 0 || m_Info.Y != 0)
	{
		Move(m_Info.X, m_Info.Y);
	}

	return true;
}

void ViewImpl::Destroy()
{
	GET_JNIENV(env);

	jmethodID mid_CoherentView_DestroyView = env->GetMethodID(cls_CoherentView, "destroyView", "()V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_DestroyView);

	env->DeleteGlobalRef(m_CoherentView);
	m_CoherentView = nullptr;

	if (m_ByteBuffer)
	{
		env->DeleteGlobalRef(m_ByteBuffer);
	}
	m_BitmapBuffer.reset();
	
	if (m_Observer)
	{
		m_Observer->OnViewDestroyed(m_Id);
	}
}
	
void ViewImpl::SetShow(bool show)
{
	m_IsShowing = show && !m_Info.IsSurfaceView;

	GET_JNIENV(env);

	jmethodID mid_CoherentView_SetShow = env->GetMethodID(cls_CoherentView, "setShow", "(Z)V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_SetShow, m_IsShowing);
}

bool ViewImpl::IsShowing() const
{
	return m_IsShowing;
}
	
int ViewImpl::GetWidth() const
{
	return m_Info.Width;
}
	
int ViewImpl::GetHeight() const
{
	return m_Info.Height;
}

void ViewImpl::Move(unsigned x, unsigned y)
{
	m_Info.X = x;
	m_Info.Y = y;

	GET_JNIENV(env);

	jmethodID mid_CoherentView_MoveView = env->GetMethodID(cls_CoherentView, "moveView", "(II)V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_MoveView, x, y);
}
	
void ViewImpl::Resize(unsigned width, unsigned height)
{
	m_BitmapBuffer.reset();
	m_Info.Width = width;
	m_Info.Height = height;

	GET_JNIENV(env);

	jmethodID mid_CoherentView_ResizeView = env->GetMethodID(cls_CoherentView, "resizeView", "(II)V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_ResizeView, width, height);
}
	
bool ViewImpl::IsTransparent() const
{
	return m_Info.IsTransparent;
}
	
bool ViewImpl::SupportsClickThrough() const
{
	return m_Info.SupportClickThrough;;
}
	
void ViewImpl::Load(const CharType* path)
{
	GET_JNIENV(env);

	jstring str_Path = Coherent::CreateJStringFrom(env, path);

	jmethodID mid_CoherentView_Load = env->GetMethodID(cls_CoherentView, "load", "(Ljava/lang/String;)V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_Load, str_Path);

	m_CurrentlyRequestedPath = path;
}
	
void ViewImpl::Reload(bool ignoreCache)
{
	GET_JNIENV(env);

	jmethodID mid_CoherentView_Reload = env->GetMethodID(cls_CoherentView, "reload", "()V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_Reload);
}
	
bool ViewImpl::HasLoadedEverything() const
{
	return !m_IsLoading;
}
	
const CharType* ViewImpl::GetCurentViewPath() const
{
	// Since we're not on the UI thread we can't query the WebView for the current path
	// and our best bet is to return the last requested path
	return m_CurrentlyRequestedPath.c_str();
}
	
const CharType* ViewImpl::GetLastRequestedPath() const
{
	return m_CurrentlyRequestedPath.c_str();
}
	
const CharType* ViewImpl::GetLastLoadedPath() const
{
	return GetCurentViewPath();
}
	
const CharType* ViewImpl::GetLastFailedPath() const
{
	return m_LastFailedPath.c_str();
}
	
void ViewImpl::RegisterForEvent(const char* name, IEventHandler* handler)
{
	if (m_ViewBinder.AddEventHandler(name, BinderAndroid::EventHandlerPtr(handler)))
	{
		View::TriggerEvent("_Register", name);
	}
}

void ViewImpl::BindCall(const char* name, IEventHandler* handler)
{
	m_ViewBinder.AddCallHandler(name, BinderAndroid::EventHandlerPtr(handler));
}

const unsigned int& ViewImpl::GetId() const
{
	return m_Id;
}
	
void ViewImpl::ExecuteScript(const char* script)
{
	GET_JNIENV(env);

	jmethodID mid_CoherentView_ExecuteScript = env->GetMethodID(cls_CoherentView, "executeScript", "(Ljava/lang/String;)V");
	jstring str_Script = env->NewStringUTF(script);
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_ExecuteScript, str_Script);
}

void ViewImpl::SetScriptError(ScriptCallErrorType error, const char* message)
{
	m_ViewBinder.SetError(error, message);
}

void ViewImpl::InterceptURLRequests(bool intercept)
{
	m_InterceptRequests = intercept;
}
	
void ViewImpl::LoadFailed(const CustomString& failedUrl, const std::string& reason)
{
	m_LastFailedPath = failedUrl;
	m_Listener->OnFailLoad(0, failedUrl.c_str(), true, reason.c_str());
	m_IsLoading = false;
}

void ViewImpl::ShouldStartLoad(const CustomString& url, bool& shouldStartLoad)
{
	COHERENT_TRACE("Requested permission to load: ", url);
	if (m_InterceptRequests)
	{
		URLResponseImpl response;
		m_Listener->OnURLRequest(url.c_str(), &response);
		
		if(!response.NewPath.empty())
		{
			Load(response.NewPath.c_str());
			shouldStartLoad = false;
		}
		else if (response.ShouldContinue)
		{
			shouldStartLoad = true;
		}
		else
		{
			shouldStartLoad = false;
		}
	}
	else
	{
		shouldStartLoad = true;
	}

	pthread_mutex_lock(&m_SyncObject.Mutex);
	pthread_cond_signal(&m_SyncObject.ConditionVariable);
	pthread_mutex_unlock(&m_SyncObject.Mutex);
}

void ViewImpl::DidFinishLoad(const CustomString& url)
{
	m_Listener->OnFinishLoad(0, url.c_str(), true, 0, nullptr, 0);
	m_IsLoading = false;
}
	
void ViewImpl::DidStartLoad(const CharType*)
{
	m_IsLoading = true;
}

void ViewImpl::JavaScriptMessageReceived(const CustomString& message, const CustomString& defaultPrompt, const CustomString& frameUrl, int messageType)
{
	m_Listener->OnJavaScriptMessage(message.c_str(), defaultPrompt.c_str(), frameUrl.c_str(), messageType);
}
	
void ViewImpl::Update()
{
	if(!m_HasNotifiedReady)
	{
		if (IsJavaViewNonNull())
		{
			m_Listener->OnViewCreated(this);
			m_HasNotifiedReady = true;
		}
	}
}

bool ViewImpl::IsJavaViewNonNull() const
{
	if (m_CoherentView)
	{
		GET_JNIENV(env);

		jmethodID mid_CoherentView_IsViewNonNull = env->GetMethodID(cls_CoherentView, "isViewNonNull", "()Z");
		jboolean result = env->CallBooleanMethod(m_CoherentView, mid_CoherentView_IsViewNonNull);
		return result;
	}

	return false;
}
	
void ViewImpl::GoBack()
{
	GET_JNIENV(env);

	jmethodID mid_CoherentView_GoBack = env->GetMethodID(cls_CoherentView, "goBack", "()V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_GoBack);
}

void ViewImpl::GoForward()
{
	GET_JNIENV(env);

	jmethodID mid_CoherentView_GoForward = env->GetMethodID(cls_CoherentView, "goForward", "()V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentView_GoForward);
}
	
void ViewImpl::SetInputState(ViewInputState state)
{
	m_InputState = state;
	
	std::stringstream js;
	js << "if (window.__couiAndroid !== undefined) window.__couiAndroid.inputState = " << (int)state << ";";
	ExecuteScript(js.str().c_str());
}

ViewInputState ViewImpl::GetInputState() const
{
	return m_InputState;
}

void ViewImpl::DispatchKeyEvent(int state, int character)
{
	GET_JNIENV(env);

	jmethodID mid_CoherentViewDispatch = env->GetMethodID(cls_CoherentView, "dispatchKeyEvent", "(II)V");
	env->CallVoidMethod(m_CoherentView, mid_CoherentViewDispatch, state, character);
}

void ViewImpl::AddNewTouchEvent(int fingerId, int phase, int x, int y)
{
	GET_JNIENV(env);

	m_Listener->OnNewTouchEvent(fingerId, phase, x, y);
}
	
void ViewImpl::TriggerEvent(const char* name)
{
	m_ViewBinder.EventBegin(name, 0);
	m_ViewBinder.EventEnd();
}

BinderAndroid* ViewImpl::GetBinder()
{
	return &m_ViewBinder;
}

bool ViewImpl::GetAsBitmap(void* data, int size) const
{
	if (m_ClientBitmap == nullptr)
	{
		m_ClientBitmap = static_cast<unsigned*>(data);
		m_ClientBitmapSize = size;
		GetAsBitmapImpl();
		return true;
	}
	return false;
}

void ViewImpl::GetAsBitmapImpl() const
{
	const auto width = ViewImpl::GetWidth();
	const auto height = ViewImpl::GetHeight();
	const auto size = 4 * width * height;

	GET_JNIENV(env);

	COHERENT_DEBUG("Drawing view ", m_Id, " to bitmap ", width, "x", height);
	if (!m_BitmapBuffer)
	{
		m_BitmapBuffer.reset(new unsigned char[size]);
		auto buffer = env->NewDirectByteBuffer(m_BitmapBuffer.get(), size);
		m_ByteBuffer = env->NewGlobalRef(buffer);
	}

	auto mid_GetAsBitmap = env->GetMethodID(
		cls_CoherentView, "getAsBitmap", "(Ljava/nio/ByteBuffer;II)V");

	env->CallVoidMethod(m_CoherentView,
						mid_GetAsBitmap,
						m_ByteBuffer,
						ViewImpl::GetWidth(),
						ViewImpl::GetHeight());
}

void ViewImpl::NotifyBitmapReady(const void* data, int size)
{
	COHERENT_WARNING("NotifyBitmapReady view ", m_Id, " to buffer ", size, "x",
		m_ClientBitmapSize);
	const auto limit = std::min(size, m_ClientBitmapSize) / 4;
	auto buffer = static_cast<const unsigned*>(data);
	// this copy can be spared with the price of
	// creating a new m_ByteBuffer for each GetAsBitmap
	std::copy(buffer, buffer + limit, m_ClientBitmap);
	m_Listener->OnBitmapReady(m_ClientBitmap, limit * 4);
	m_ClientBitmap = nullptr;
}

ViewSynchronizationObject* ViewImpl::GetSyncObject()
{
	return &m_SyncObject;
}

bool ViewImpl::IsReady() const
{
	return m_HasNotifiedReady;
}

}}

