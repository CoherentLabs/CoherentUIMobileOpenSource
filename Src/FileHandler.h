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

namespace Coherent { namespace UI {

/// Abstract interface for responding to read resource requests
///
class ResourceResponse
{
public:
	virtual ~ResourceResponse() {}
	
	/// Request buffer for resource data.
	/// @warning GetBuffer **must** be called only once.
	/// @note GetBuffer may be called in any thread.
	/// @param size the size of the resource
	/// @return pointer to the beginning of the buffer. It may return nullptr when allocating the buffer has failed
	virtual void* GetBuffer(unsigned long size) = 0;
	
	/// Report that the resource is read successfully.
	/// @warning This should be executed in the thread that all Coherent UI methods are executed.
	/// @warning After this method is called, any calls to any method of the instance are undefined behavior.
	virtual void SignalSuccess() = 0;
	
	/// Report that resource reading has failed.
	/// @warning This should be executed in the thread that all Coherent UI methods are executed.
	/// @warning After this method is called, any calls to any method of the instance are undefined behavior.
	virtual void SignalFailure() = 0;
};

/// Abstract interface that allows clients to provide their own file-handling functionality
///
class FileHandler
{
public:
	virtual ~FileHandler() {}
	/// Requests to read a resource
	/// @param url the coui url of the resource.
	/// @note The url has been decoded - any \%xx have been replaced by their original characters.
	/// @param response the response to be filled with the resource data or notified for failure
	virtual void ReadFile(const CharType* url, ResourceResponse* response) = 0;
};
	
}}
