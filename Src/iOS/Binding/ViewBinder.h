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
#include <Binding/JStoNativeBinder.h>

namespace Coherent
{
namespace UI
{
namespace Detail
{

class ViewBinder : public JStoNativeBinder
{
public:
	ViewBinder();
	~ViewBinder();

	virtual void EventBegin(const char* name, size_t arguments) COHERENT_OVERRIDE;
	virtual void EventEnd() COHERENT_OVERRIDE;

	virtual void OnTypeBegin() COHERENT_OVERRIDE;
	virtual void OnTypeEnd() COHERENT_OVERRIDE;

	virtual void TypeName(const char* name) COHERENT_OVERRIDE;
	virtual void PropertyName(const char* name) COHERENT_OVERRIDE;

	virtual void BindNull() COHERENT_OVERRIDE;
	virtual void Bind(bool value) COHERENT_OVERRIDE;
	virtual void Bind(signed int value) COHERENT_OVERRIDE;
	virtual void Bind(unsigned int value) COHERENT_OVERRIDE;
	virtual void Bind(signed char value) COHERENT_OVERRIDE;
	virtual void Bind(unsigned char value) COHERENT_OVERRIDE;

	virtual void Bind(float value) COHERENT_OVERRIDE;
	virtual void Bind(double value) COHERENT_OVERRIDE;
	virtual void Bind(const char* value) COHERENT_OVERRIDE;
	virtual void Bind(const wchar_t* value) COHERENT_OVERRIDE;

	virtual void ArrayBegin(size_t size) COHERENT_OVERRIDE;
	virtual void ArrayEnd() COHERENT_OVERRIDE;

	virtual void MapBegin(size_t size) COHERENT_OVERRIDE;
	virtual void MapEnd() COHERENT_OVERRIDE;

	void SetError(ScriptCallErrorType error, const char* message);

	#if defined(COHERENT_DOT_NET)
		virtual void RawEvent(const char* name, size_t arguments, size_t size, const unsigned char* data) COHERENT_OVERRIDE;
		virtual void RawResult(size_t size, const unsigned char* data) COHERENT_OVERRIDE;

		virtual void SetBindingsReadyCallback(Coherent::UI::DotNetBinder::BindingsCallback callback, unsigned int binderId) COHERENT_OVERRIDE;
		virtual void SetBindingsReleasedCallback(Coherent::UI::DotNetBinder::BindingsCallback callback, unsigned int binderId) COHERENT_OVERRIDE;

		const void* GetArgumentsData() const;
		double GetRequestId() const
		{
			return m_RequestId;
		}
	#endif
protected:
	std::stringstream m_JSEvent;

private:
	void BindString(const char* value);
	void BlobsToJSON(size_t count, const unsigned char* data, size_t size);
	void InsertSeparator();
	int m_Separator;
	
	std::deque<bool> m_InObject;
};

}
}
}
