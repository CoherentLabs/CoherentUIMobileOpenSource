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

namespace Coherent
{
namespace UI
{

template <typename Key, typename Value, typename Comparator, typename Allocator>
void CoherentBindInternal(Binder* binder, const std::map<Key, Value, Comparator, Allocator>& value)
{
	binder->MapBegin(value.size());
	typedef typename std::map<Key, Value, Comparator, Allocator>::const_iterator Iterator;
	
	Iterator end = value.end();
	for (Iterator i = value.begin(); i != end; ++i)
	{
		CoherentBindInternal(binder, i->first);
		CoherentBindInternal(binder, i->second);
	}
	
	binder->MapEnd();
}

template <typename Key, typename Value, typename Comparator, typename Allocator>
void CoherentReadInternal(Binder* binder, std::map<Key, Value, Comparator, Allocator>& value)
{
	size_t size = binder->ReadMapBegin();
	
	typedef typename std::map<Key, Value, Comparator, Allocator>::value_type ValueType;

	for (size_t i = 0; i < size; ++i)
	{
		Key key;
		Value mapValue;
		binder->ReadKeyValuePair();
		CoherentReadInternal(binder, key);
		CoherentReadInternal(binder, mapValue);

		value.insert(ValueType(key, mapValue));
	}
	
	binder->ReadMapEnd();
}

}
}
