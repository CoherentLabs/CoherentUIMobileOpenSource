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

#include <FileHandler.h>

#include <functional>
#include <vector>
#include <pthread.h>

namespace Coherent { namespace UI {

struct ResourceResponseSynchronizationObject
{
	ResourceResponseSynchronizationObject()
		: ConditionVariable(PTHREAD_COND_INITIALIZER)
		, Mutex(PTHREAD_MUTEX_INITIALIZER)
	{}

	pthread_cond_t ConditionVariable;
	pthread_mutex_t Mutex;
};

class ResourceResponseImpl : public ResourceResponse
{
public:	
	ResourceResponseImpl()
		: m_Failed(false)
		, m_ResponseReady(false)
	{}
	
	virtual ~ResourceResponseImpl();
	
	virtual void* GetBuffer(unsigned long size) override;
	
	virtual void SignalSuccess() override;
	
	virtual void SignalFailure() override;
	
	char* GetData() const;
	size_t GetDataLength() const;
	bool HasFailed() const;
	bool IsResponseReady() const;

	ResourceResponseSynchronizationObject* GetSyncObject();

private:
	void OnResponseReady();
	
private:
	typedef std::vector<char> CoherentByteVector;
	CoherentByteVector m_Data;

	ResourceResponseSynchronizationObject m_SyncObject;
	bool m_Failed;
	bool m_ResponseReady;
};

}}
