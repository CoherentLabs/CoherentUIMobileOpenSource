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

#include <Binding/EventHandler.h>
#include <iOS/BinderiOS.h>

namespace Coherent
{
namespace UI
{
typedef void  (COHERENT_STDCALL *ManagedCallbackAOT)(unsigned int viewId, int callbackId, const void* data, double requestId);

class ManagedEventHandlerAOT : public IEventHandler
{
public:
	ManagedEventHandlerAOT(ManagedCallbackAOT callback, int id)
		: m_Callback(callback)
		, m_Id(id)
	{
	}

	virtual void Invoke(ArgumentsBinder* binder)
	{
		auto viewBinder = static_cast<BinderiOS*>(binder);
		m_Callback(viewBinder->GetViewId(), m_Id, viewBinder->GetArgumentsData(), viewBinder->GetRequestId());
	}

	virtual const void* GetTarget()
	{
	}

	ManagedCallbackAOT m_Callback;
	int m_Id;
};
}
}
