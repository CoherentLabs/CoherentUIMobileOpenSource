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
#include "precompiled.h"

#include <iOS/BinderiOS.h>
#include <iOS/Binding/BlobFromJS.h>
#include <Binding/Blob/BlobStream.h>
#include <Binding/Blob/Blob.h>
#include <ViewListener.h>

namespace Coherent
{
namespace UI
{
BinderiOS::BinderiOS()
	: m_WebView(nullptr)
	, m_ViewId(0)
	, m_Listener(nullptr)
{
}

BinderiOS::~BinderiOS()
{
	HandleUnload();
}

void BinderiOS::EventEnd()
{
    ViewBinder::EventEnd();

	auto js = m_JSEvent.str();
	NSString* script = [NSString stringWithCString:js.c_str() encoding:NSASCIIStringEncoding];
	[m_WebView stringByEvaluatingJavaScriptFromString: script];
}

void BinderiOS::SetView(UIWebView* webView, unsigned viewId, ViewListenerBase* listener)
{
	m_WebView = webView;
	m_ViewId = viewId;
	m_Listener = listener;
}

void BinderiOS::TriggerNative(NSString* code)
{
	int length = [code length];
	m_CurrentJS.resize(length + 1);
	[code getCString:&m_CurrentJS[0] maxLength:length + 1 encoding: NSASCIIStringEncoding];
	auto result = Detail::URIDecodeInplace(&m_CurrentJS[0], length);

	if (result.second)
	{
		const int prefixLength = 14;
		const char eventType = m_CurrentJS[12];
		switch (eventType) {
			case 'r': // ready
				{
					std::string url = m_CurrentJS.substr(prefixLength, result.first - 2 - prefixLength);
					bool isMainFrame = m_CurrentJS[result.first - 1] == '1';
					BindingsReady(0, url.c_str(), isMainFrame);
				}
				break;
			case 'u': // unload
				{
					HandleUnload();
				}
				break;
			case 'q': // query for engine
				{
					ExecuteScript("window.engine = window.engine || {};");
				}
				break;

			default:
			{
				const bool isCall = eventType == 'c';
				auto eventEnd = m_CurrentJS.find(':', prefixLength); // skip coherent-js:X:
				std::string  name = m_CurrentJS.substr(prefixLength, eventEnd - prefixLength);
				m_RequestId = 0.0;
				if (isCall)
				{
					char* requestEnd = nullptr;
					m_RequestId = strtod(&m_CurrentJS[eventEnd+1], &requestEnd);
					eventEnd = requestEnd - &m_CurrentJS[0];

				}
				Coherent::Blob::BlobOStream output(&m_BlobData[0], m_BlobData.size(), [this](size_t more) -> void * {
					m_BlobData.resize(m_BlobData.size() + more);
					return &m_BlobData[0];
				});

				if (Detail::JSONtoBlob(m_CurrentJS.c_str() + eventEnd + 1, result.first - eventEnd - 1, output))
				{
					Coherent::Blob::BlobIStream input(m_BlobData.data(), m_BlobData.size());
					EventArguments arguments(input);
					if (isCall)
					{
						Call(name, arguments, m_RequestId);
					}
					else
					{
						Trigger(name, arguments);
					}
					m_BlobData.clear();
				}
			}
		}
	}
}

void BinderiOS::ExecuteScript(const char* code)
{
	NSString* javaScript = [NSString stringWithCString:code encoding:NSUTF8StringEncoding];
	[m_WebView stringByEvaluatingJavaScriptFromString:javaScript];
}

void BinderiOS::BindingsReady(int frameId, const char* url, bool isMain)
{
	if (isMain)
	{
		m_URLWithBindings.assign(url, url + std::strlen(url));

		m_Listener->OnReadyForBindings(frameId, m_URLWithBindings.c_str(), isMain);

		Ready(frameId, isMain);
	}
}

void BinderiOS::HandleUnload()
{
	if (!m_URLWithBindings.empty())
	{
		m_Listener->OnBindingsReleased(0, m_URLWithBindings.c_str(), true);
		ReleaseBindings(0, m_URLWithBindings.c_str(), true);
		m_URLWithBindings.clear();
	}
}

#if defined(COHERENT_DOT_NET)

	const void* BinderiOS::GetArgumentsData() const
	{
		// skip the blob type
		const int offset = sizeof(Coherent::Blob::AlignedType);
		return (m_BlobData.size() > offset)? m_BlobData.data() + offset : nullptr;
	}

#endif

}
}

#endif

