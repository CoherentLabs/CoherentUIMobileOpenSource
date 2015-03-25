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

#include <Declarations.h>

namespace Coherent
{
namespace UI
{
class Binder;
class View;

/// Manages packing events to be sent to JS
///
class ScriptEventWriter
{
public:
	/// Starts new event for binding
	/// @param eventName name of the event to be triggered
	/// @param arguments number of arguments for the event
	virtual void BeginEvent(const char* eventName, size_t arguments) = 0;

	virtual Binder* GetBinder() = 0;

	/// Ends the event and it is now ready to be sent to JS
	/// 
	virtual void EndEvent() = 0;
};
																				
}
}

// document the file, so that doxygen will include the documentation of free functions

/// @file

/// Creates a new event writer
/// @param view the view to bind this writer to
/// @return pointer to a ScriptEventWriter
extern "C" COHERENTUI_API Coherent::UI::ScriptEventWriter* GetScriptEventWriter(Coherent::UI::View* view);
