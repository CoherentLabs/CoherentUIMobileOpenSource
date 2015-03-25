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

#include <Android/jni/BinderAndroid.h>
#include <Android/jni/Binding/BlobFromJS.h>
#include <Binding/Blob/BlobStream.h>
#include <Binding/Blob/Blob.h>
#include <ViewListener.h>
#include <Android/jni/ViewImpl.h>

namespace Coherent
{
namespace UI
{
BinderAndroid::BinderAndroid()
	: m_ViewImpl(nullptr)
	, m_ViewId(0)
	, m_Listener(nullptr)
{
}

BinderAndroid::~BinderAndroid()
{
	HandleUnload();
}

void BinderAndroid::EventEnd()
{
	ViewBinder::EventEnd();

	auto js = m_JSEvent.str();
	m_ViewImpl->ExecuteScript(js.c_str());
}

void BinderAndroid::SetView(ViewImpl* viewImpl, unsigned viewId, ViewListenerBase* listener)
{
	m_ViewImpl = viewImpl;
	m_ViewId = viewId;
	m_Listener = listener;
}

void BinderAndroid::TriggerNative(const CustomString& coherentUrlString)
{
	const CharType* coherentUrl = coherentUrlString.c_str(); 
	size_t length = coherentUrlString.length();
	
	
	m_CurrentJS.resize(length + 1);
	m_CurrentJS.assign(coherentUrl, coherentUrl + length); // Convert to ASCII
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
					// Here we need to execute "window.engine = window.engine || {};"
					// in the same call stack as the calling function;
					// Since this is a problem with the current Android architecture,
					// we actually invoke a method in CoherentJavascriptInterface.java
					// and never get in here.
				}
				break;

			default:
			{
				const bool isCall = eventType == 'c';
				auto eventEnd = m_CurrentJS.find(':', prefixLength); // skip coherent-js:X:
				std::string name = m_CurrentJS.substr(prefixLength, eventEnd - prefixLength);
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

void BinderAndroid::BindingsReady(int frameId, const char* url, bool isMain)
{
	if (isMain)
	{
		m_URLWithBindings.assign(url, url + std::strlen(url));

		m_Listener->OnReadyForBindings(frameId, m_URLWithBindings.c_str(), isMain);

		Ready(frameId, isMain);
	}
}

void BinderAndroid::HandleUnload()
{
	if (!m_URLWithBindings.empty())
	{
		m_Listener->OnBindingsReleased(0, m_URLWithBindings.c_str(), true);
		ReleaseBindings(0, m_URLWithBindings.c_str(), true);
		m_URLWithBindings.clear();
	}
}

#if defined(COHERENT_DOT_NET)

	const void* BinderAndroid::GetArgumentsData() const
	{
		// skip the blob type
		const int offset = sizeof(Coherent::Blob::AlignedType);
		return (m_BlobData.size() > offset)? m_BlobData.data() + offset : nullptr;
	}

#endif

}
}


