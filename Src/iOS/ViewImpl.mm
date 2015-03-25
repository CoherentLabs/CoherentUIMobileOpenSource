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

#if !defined(COHERENT_UNITY3D_BUILD) || defined(FORCE_COMPILE_FILE)
#include <iOS/ViewImpl.h>
#include <ViewListener.h>
#include <iOS/ViewObserver.h>

#import <UIKit/UIViewController.h>
#import <UIKit/UIScreen.h>

#import <QuartzCore/CALayer.h>

#include <functional>

#if defined(COHERENT_UNITY3D_BUILD)

extern "C" UIViewController* CoherentGetUnityGLViewController();
#endif

@interface CoherentWebViewDelegate ()
{
	Coherent::UI::ViewImpl* m_Parent;
}

- (id)initWithParent:(Coherent::UI::ViewImpl*)parent;

@end

@implementation CoherentWebViewDelegate

- (id)initWithParent:(Coherent::UI::ViewImpl*)parent
{
	self = [super init];
	m_Parent = parent;
	return self;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
	std::string errorDesc = [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding];
	m_Parent->LoadFailed(errorDesc.c_str());
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
	NSString* urlString = [[request URL] absoluteString];

	if ([urlString hasPrefix:@"coherent-js:"])
	{
		m_Parent->GetBinder()->TriggerNative(urlString);
		return NO;
	}
	Coherent::CustomString url = Coherent::CreateFromNSString(urlString);
	return m_Parent->ShouldStartLoad(url.c_str(), navigationType) ? YES : NO;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
	m_Parent->DidFinishLoad();
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
	m_Parent->DidStartLoad();
}

@end

@interface CoherUIWebView () {
	Coherent::UI::ViewImpl* m_Parent;
}
-(CoherUIWebView*) initWithFrame:(CGRect)aRect andParent:(Coherent::UI::ViewImpl*)parent;
-(UIView*)hitTest:(CGPoint)point withEvent:(UIEvent*)event;
@end

@implementation CoherUIWebView

-(CoherUIWebView*) initWithFrame:(CGRect)aRect andParent:(Coherent::UI::ViewImpl*)parent
{
	if(self = [super initWithFrame:aRect]) {
		m_Parent = parent;
	} else {
		return nil;
	}
	return self;
}

-(id)hitTest:(CGPoint)point withEvent:(UIEvent*)event
{
	const auto inputState = m_Parent->GetInputState();
	
	switch(inputState) {
		case Coherent::UI::VIS_TakeAll:
		case Coherent::UI::VIS_TakeNone: // we take care to disable user interactions so this will work correctly
			return [super hitTest:point withEvent:event];
			break;
		case Coherent::UI::VIS_Transparent:
			NSString* js = [NSString stringWithFormat:@"engine.checkClickThrough(%f, %f)", point.x, point.y];
			NSString* clickthroughRes = [self stringByEvaluatingJavaScriptFromString:js];
			if(!clickthroughRes || ![clickthroughRes isEqualToString:@"Y"]) {
				return nil;
			} else {
				return [super hitTest:point withEvent:event];
			}
			break;
	}
}

@end

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

ViewImpl::ViewImpl(unsigned int myId,
				   const ViewInfo& info,
				   ViewListenerBase* listener,
				   ViewObserver* observer)
	: m_Id(myId)
	, m_Info(info)
	, m_IsShowing(info.ShowImmediately)
	, m_Listener(listener)
	, m_Observer(observer)
	, m_WebView(nullptr)
	, m_ViewDelegate(nullptr)
	, m_InterceptRequests(false)
	, m_HasNotifiedReady(false)
	, m_InputState(info.InitialInputState)
	, m_ScaleFactor(1)
	, m_BitmapContext(nullptr)
{}

ViewImpl::~ViewImpl()
{}
	
bool ViewImpl::Initialize(const CharType* path)
{
#if defined(COHERENT_UNITY3D_BUILD)
	if (!m_Info.IsSurfaceView)
	{
		m_Info.Parent = (__bridge void*)CoherentGetUnityGLViewController().view;
	}
#endif

	// calculate points for retina
	m_ScaleFactor = (m_Info.IsSurfaceView)? 1.0 : [[UIScreen mainScreen] scale];
	const unsigned xPoints = m_Info.X / m_ScaleFactor;
	const unsigned yPoints = m_Info.Y / m_ScaleFactor;
	const unsigned width = m_Info.Width / m_ScaleFactor;
	const unsigned height = m_Info.Height / m_ScaleFactor;

	m_WebView = [[CoherUIWebView alloc] initWithFrame:CGRectMake(xPoints, yPoints, width, height) andParent:this];

#if defined(COHERENT_FORCE_WEBGL_IOS)
	if(m_Info.EnableWebGLSupport)
	{
		//Note: This is a private API used by iAd framework since iOS 4.2
		id webDocumentView = [m_WebView performSelector:@selector(_browserView)];
		id backingWebView = [webDocumentView performSelector:@selector(webView)];
		[backingWebView performSelector:@selector(_setWebGLEnabled:) withObject:[NSNumber numberWithBool:YES]];
	}
#endif

	m_WebView.contentScaleFactor = m_ScaleFactor;
	m_WebView.scalesPageToFit = m_Info.ScaleToFit && !m_Info.IsSurfaceView;
	
	m_ViewDelegate = [[CoherentWebViewDelegate alloc] initWithParent:this];
	m_WebView.delegate = m_ViewDelegate;
	
	if(!m_WebView) {
		return false;
	}

	if (m_Info.Parent)
	{
		[(__bridge UIView*)m_Info.Parent addSubview:m_WebView];
	}
	m_ViewBinder.SetView(m_WebView, m_Id, m_Listener);
	
	m_WebView.opaque = m_Info.IsTransparent ? NO : YES;
	m_WebView.backgroundColor = [UIColor clearColor];
   
	Load(path);
	
	m_WebView.hidden = m_IsShowing ? NO : YES;
	m_WebView.userInteractionEnabled = ((m_InputState == VIS_TakeNone) ? NO : YES);
	
	m_WebView.scrollView.bounces = NO;
	
	if (m_Info.ResizeOnRotation)
	{
		m_WebView.autoresizingMask = UIViewAutoresizingFlexibleWidth |
			UIViewAutoresizingFlexibleHeight;
		m_WebView.contentMode = (UIViewContentMode) (UIViewContentModeTop |
													 UIViewContentModeLeft);
	}
	return true;
}

void ViewImpl::Destroy()
{
	[m_WebView removeFromSuperview];

	ClearBitmapContext();

	if (m_Observer)
	{
		m_Observer->OnViewDestroyed(m_Id);
	}
}

void ViewImpl::SetShow(bool show)
{
	m_IsShowing = show;
	m_WebView.hidden = m_IsShowing ? NO : YES;
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
	
	MoveResizeFrame();
}
	
void ViewImpl::Resize(unsigned width, unsigned height)
{
	ClearBitmapContext();
	m_Info.Width = width;
	m_Info.Height = height;

	MoveResizeFrame();
}
	
void ViewImpl::MoveResizeFrame()
{
	// calculate points for retina
	const unsigned xPoints = m_Info.X / m_ScaleFactor;
	const unsigned yPoints = m_Info.Y / m_ScaleFactor;
	const unsigned widthPoints = m_Info.Width / m_ScaleFactor;
	const unsigned heightPoints = m_Info.Height / m_ScaleFactor;
	
	m_WebView.frame = CGRectMake(xPoints, yPoints, widthPoints, heightPoints);
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
	NSString* pathString = CreateNSStringFrom(path);
	NSURL* url = [NSURL URLWithString:pathString];
	NSURLRequest* request = [NSURLRequest requestWithURL:url];
	[m_WebView loadRequest:request];
	
	m_CurrentlyRequestedPath = path;
}
	
void ViewImpl::Reload(bool ignoreCache)
{
	[m_WebView reload];
}
	
bool ViewImpl::HasLoadedEverything() const
{
	return m_WebView.loading;
}
	
const CharType* ViewImpl::GetCurentViewPath() const
{
	return CreateFromNSString([m_WebView.request.URL absoluteString]);;
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
	if (m_ViewBinder.AddEventHandler(name, BinderiOS::EventHandlerPtr(handler)))
	{
		View::TriggerEvent("_Register", name);
	}
}

void ViewImpl::BindCall(const char* name, IEventHandler* handler)
{
	m_ViewBinder.AddCallHandler(name, BinderiOS::EventHandlerPtr(handler));
}

const unsigned int& ViewImpl::GetId() const
{
	return m_Id;
}
	
void ViewImpl::ExecuteScript(const char* script)
{
	m_ViewBinder.ExecuteScript(script);
}

void ViewImpl::InterceptURLRequests(bool intercept)
{
	m_InterceptRequests = intercept;
}

void ViewImpl::SetScriptError(ScriptCallErrorType error, const char* message)
{
	m_ViewBinder.SetError(error, message);
}
	
void ViewImpl::LoadFailed(const char* reason)
{
	m_LastFailedPath = m_CurrentlyRequestedPath;
	m_Listener->OnFailLoad(0, m_CurrentlyRequestedPath.c_str(), true, reason);
}
	
bool ViewImpl::ShouldStartLoad(const CharType* url, UIWebViewNavigationType type)
{
	COHERENT_TRACE("Requested permission to load: ", url);
	if(m_InterceptRequests) {
		URLResponseImpl response;
		m_Listener->OnURLRequest(url, &response);
		
		if(!response.NewPath.empty()) {
			Load(response.NewPath.c_str());
			return true;
		} else if (response.ShouldContinue) {
			return true;
		} else {
			return false;
		}
	}
	return true;
}
	
void ViewImpl::DidFinishLoad() {
	Coherent::CustomString url = CreateFromNSString([m_WebView.request.URL absoluteString]);
	m_Listener->OnFinishLoad(0, url.c_str(), true, 0, nullptr, 0);
}
	
void ViewImpl::DidStartLoad()
{}
	
void ViewImpl::Update()
{
	if(!m_HasNotifiedReady) {
		m_Listener->OnViewCreated(this);
		m_HasNotifiedReady = true;
	}
}
	
void ViewImpl::GoBack()
{
	[m_WebView goBack];
}

void ViewImpl::GoForward()
{
	[m_WebView goForward];
}
	
void ViewImpl::SetInputState(ViewInputState state)
{
	m_InputState = state;
	m_WebView.userInteractionEnabled = ((m_InputState == VIS_TakeNone) ? NO : YES);
}

ViewInputState ViewImpl::GetInputState() const
{
	return m_InputState;
}

void ViewImpl::TriggerEvent(const char* name)
{
	m_ViewBinder.EventBegin(name, 0);
	m_ViewBinder.EventEnd();
}

BinderiOS* ViewImpl::GetBinder()
{
	return &m_ViewBinder;
}
	
const ViewInfo& ViewImpl::GetViewInfo() const
{
	return m_Info;
}

void ViewImpl::CreateBitmapContext(int width, int height) const
{
	if (m_BitmapContext == nullptr)
	{
		m_BitmapBuffer.resize(4 * width * height);
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		m_BitmapContext = CGBitmapContextCreate(
			&m_BitmapBuffer[0],
			width,
			height,
			8,
			4 * width,
			colorSpace,
			kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);

		CGColorSpaceRelease(colorSpace);

		if (!m_BitmapContext)
		{
			NSLog(@"Could not create context for bitmap");
		}
	}
}

void ViewImpl::ClearBitmapContext()
{
	if (m_BitmapContext)
	{
		CGContextRelease(m_BitmapContext);
		m_BitmapContext = nullptr;
	}
	BitmapBuffer temp;
	m_BitmapBuffer.swap(temp);
}

bool ViewImpl::GetAsBitmap(void* buffer, int size) const
{
	int width = ViewImpl::GetWidth();
	int height = ViewImpl::GetHeight();

	if (!m_BitmapContext)
	{
		CreateBitmapContext(width, height);
	}

	assert(m_BitmapContext);

	CGRect rect = CGRectMake(0, 0, width, height);
	CGContextClearRect(m_BitmapContext, rect);

	[m_WebView.layer renderInContext:m_BitmapContext];

	const auto limit = std::min(width * height, size / 4) * 4;
						// make sure it is multiple by 4

	const auto stride = width * 4;
	const auto actualSize = (limit / stride) * stride;
	auto destination = static_cast<unsigned char*>(buffer)
							+ actualSize - stride;
	const auto end = &m_BitmapBuffer[0] + actualSize;

	for (auto source = &m_BitmapBuffer[0]; source != end; source += stride)
	{
		std::copy(source, source + stride, destination);
		destination -= stride;
	}

	m_Listener->OnBitmapReady(buffer, actualSize);
	return true;
}
}
}

#endif
