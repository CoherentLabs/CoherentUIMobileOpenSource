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

#include <ScriptEventWriter.h>
#include <iOS/BinderiOS.h>
#include <iOS/ViewImpl.h>

namespace Coherent
{
namespace UI
{
namespace Detail
{
class ScriptEventWriterImpl : public ScriptEventWriter
{
public:
	ScriptEventWriterImpl()
	{}

	~ScriptEventWriterImpl()
	{}

	virtual void BeginEvent(const char* eventName, size_t arguments) COHERENT_OVERRIDE
	{
		m_Binder->BinderiOS::EventBegin(eventName, arguments);
	}


	virtual void EndEvent() COHERENT_OVERRIDE
	{
		m_Binder->BinderiOS::EventEnd();
	}

	virtual Binder* GetBinder() COHERENT_OVERRIDE
	{
		return m_Binder;
	}
	
	void SetView(Coherent::UI::View* view)
	{
		m_Binder = static_cast<Coherent::UI::ViewImpl*>(view)->GetBinder();
	}

private:
	BinderiOS* m_Binder;
};

}
}
}

Coherent::UI::ScriptEventWriter* GetScriptEventWriter(Coherent::UI::View* view)
{
	static Coherent::UI::Detail::ScriptEventWriterImpl impl;
	impl.SetView(view);
	return &impl;
}

