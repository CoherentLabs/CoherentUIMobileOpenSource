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

#include <functional>
#include <Binding/FunctorTraits.h>

namespace Coherent
{
namespace UI
{

template <typename R>
struct FunctorTraits<std::function<R ()>>
    : public FunctorTraits<R ()>
{};

template <typename R, typename A0>
struct FunctorTraits<std::function<R (A0)>>
    : public FunctorTraits<R (A0)>
{};

template <typename R, typename A0, typename A1>
struct FunctorTraits<std::function<R (A0, A1)>>
    : public FunctorTraits<R (A0, A1)>
{};

template <typename R, typename A0, typename A1, typename A2>
struct FunctorTraits<std::function<R (A0, A1, A2)>>
    : public FunctorTraits<R (A0, A1, A2)>
{};

template <typename R, typename A0, typename A1, typename A2, typename A3>
struct FunctorTraits<std::function<R (A0, A1, A2, A3)>>
    : public FunctorTraits<R (A0, A1, A2, A3)>
{};

template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
struct FunctorTraits<std::function<R (A0, A1, A2, A3, A4)>>
    : public FunctorTraits<R (A0, A1, A2, A3, A4)>
{};

}
}
