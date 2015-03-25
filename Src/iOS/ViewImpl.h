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

#include <View.h>
#include <ViewInfo.h>
#include <iOS/BinderiOS.h>
#include <iOS/StringConversions.h>

#import <UIKit/UIWebView.h>

@interface CoherentWebViewDelegate : NSObject <UIWebViewDelegate>
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType;
- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webViewDidStartLoad:(UIWebView *)webView;
@end

@interface CoherUIWebView : UIWebView
@end

namespace Coherent { namespace UI {

class ViewObserver;
class ViewListenerBase;
	
class ViewImpl : public View
{
public:
	ViewImpl(unsigned int myId, const ViewInfo& info, ViewListenerBase* listener, ViewObserver* observer);
	virtual ~ViewImpl();
	
	bool Initialize(const CharType* path);
	
	virtual void Destroy() override;
	virtual void SetShow(bool show) override;
	virtual bool IsShowing() const override;
	virtual int GetWidth() const override;
	virtual int GetHeight() const override;
	virtual void Move(unsigned x, unsigned y) override;
	virtual void Resize(unsigned width, unsigned height) override;
	virtual bool IsTransparent() const override;
	virtual bool SupportsClickThrough() const override;
	virtual void Load(const CharType* path) override;
	virtual void Reload(bool ignoreCache) override;
	virtual bool HasLoadedEverything() const override;
	virtual const CharType* GetCurentViewPath() const override;
	virtual const CharType* GetLastRequestedPath() const override;
	virtual const CharType* GetLastLoadedPath() const override;
	virtual const CharType* GetLastFailedPath() const override;
	virtual void RegisterForEvent(const char* name, IEventHandler* handler) override;
	virtual void BindCall(const char* name, IEventHandler* handler) override;
	virtual const unsigned int& GetId() const override;
	virtual void ExecuteScript(const char* script) override;
	virtual void InterceptURLRequests(bool intercept) override;
	virtual void SetScriptError(ScriptCallErrorType error,
								const char* message) override;

	virtual void GoBack() override;
	virtual void GoForward() override;
	
	virtual void SetInputState(ViewInputState state) override;
	virtual ViewInputState GetInputState() const override;

    virtual void TriggerEvent(const char* name) override;
	
	// webview observation
	void LoadFailed(const char* reason);
	bool ShouldStartLoad(const CharType* url, UIWebViewNavigationType type);
	void DidFinishLoad();
	void DidStartLoad();
	
	void Update();

    BinderiOS* GetBinder();
	const ViewInfo& GetViewInfo() const;

	virtual bool GetAsBitmap(void* buffer, int size) const override;

private:
	void MoveResizeFrame();
	void CreateBitmapContext(int width, int height) const;
	void ClearBitmapContext();

	ViewImpl(const ViewImpl&);
	ViewImpl& operator=(const ViewImpl&);
	
	unsigned int m_Id;
	CoherUIWebView* m_WebView;
	
	ViewListenerBase* m_Listener;	
	ViewInfo m_Info;
	
	ViewObserver* m_Observer;
	CustomString m_CurrentlyRequestedPath;
	CustomString m_LastFailedPath;
	
	CoherentWebViewDelegate* m_ViewDelegate;
	bool m_InterceptRequests;
	bool m_IsShowing;
	bool m_HasNotifiedReady;
	ViewInputState m_InputState;

	double m_ScaleFactor;

	BinderiOS m_ViewBinder;
	mutable CGContextRef m_BitmapContext;
	typedef std::vector
		<GLubyte>
			BitmapBuffer;
	mutable BitmapBuffer m_BitmapBuffer;
};
}}
