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
template <bool v>
struct BoolConstant
{
	enum { Value = v };
};

typedef BoolConstant<true> TrueType;
typedef BoolConstant<false> FalseType;

template <typename T>
struct IsVoid : FalseType
{
};

template <>
struct IsVoid<void> : TrueType
{
};

template <typename T>
struct RemoveConstRef
{
	typedef T Type;
};

template <typename T>
struct RemoveConstRef<const T>
{
	typedef T Type;
};

template <typename T>
struct RemoveConstRef<T &>
{
	typedef T Type;
};

template <typename T>
struct RemoveConstRef<const T &>
{
	typedef T Type;
};

template <int A>
struct FunctorArity
{
	enum { Value = A };
};

template <typename Functor>
struct FunctorTraits;

template <typename ResultType>
struct FunctorTraits<ResultType (*)()>
{
	typedef FunctorArity<0> Arity;
	typedef ResultType Result;

	typedef FalseType IsMember;
};

template <typename ResultType, typename ClassType>
struct FunctorTraits<ResultType (ClassType::*)()>
{
	typedef FunctorArity<0> Arity;
	typedef ResultType Result;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <typename ResultType, typename ClassType>
struct FunctorTraits<ResultType (ClassType::*)() const> :
	public FunctorTraits<ResultType (ClassType::*)()>
{
};

template <typename ResultType>
struct FunctorTraits<ResultType ()>
{
	typedef FunctorArity<0> Arity;
	typedef ResultType Result;

	typedef FalseType IsMember;
};

template <typename ResultType, typename Arg0Type>
struct FunctorTraits<ResultType (*)(Arg0Type)>
{
	typedef FunctorArity<1> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;

	typedef FalseType IsMember;
};

template <typename ResultType, typename ClassType, typename Arg0Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type)>
{
	typedef FunctorArity<1> Arity;
	typedef ResultType Result;
	
	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <typename ResultType, typename ClassType, typename Arg0Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type) const>
	: public FunctorTraits<ResultType (ClassType::*)(Arg0Type)>
{
};

template <typename ResultType, typename Arg0Type>
struct FunctorTraits<ResultType (Arg0Type)>
{
	typedef FunctorArity<1> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;

	typedef FalseType IsMember;
};

template <typename ResultType, typename Arg0Type, typename Arg1Type>
struct FunctorTraits<ResultType (*)(Arg0Type, Arg1Type)>
{
	typedef FunctorArity<2> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;

	typedef FalseType IsMember;
};

template <typename ResultType, typename ClassType, typename Arg0Type, typename Arg1Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type)>
{
	typedef FunctorArity<2> Arity;
	typedef ResultType Result;
	
	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <
	typename ResultType,
	typename ClassType,
	typename Arg0Type,
	typename Arg1Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type) const>
	: public FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type)>
{
};

template <typename ResultType, typename Arg0Type, typename Arg1Type>
struct FunctorTraits<ResultType (Arg0Type, Arg1Type)>
{
	typedef FunctorArity<2> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;

	typedef FalseType IsMember;
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type>
struct FunctorTraits<ResultType (*)(Arg0Type, Arg1Type, Arg2Type)>
{
	typedef FunctorArity<3> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;

	typedef FalseType IsMember;
};

template <typename ResultType, typename ClassType, typename Arg0Type, typename Arg1Type, typename Arg2Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type)>
{
	typedef FunctorArity<3> Arity;
	typedef ResultType Result;
	
	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <
	typename ResultType,
	typename ClassType,
	typename Arg0Type,
	typename Arg1Type,
	typename Arg2Type>
struct FunctorTraits<
	ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type) const>
	: public FunctorTraits<
		ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type)>
{
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type>
struct FunctorTraits<ResultType (Arg0Type, Arg1Type, Arg2Type)>
{
	typedef FunctorArity<3> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;

	typedef FalseType IsMember;
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
struct FunctorTraits<ResultType (*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type)>
{
	typedef FunctorArity<4> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;

	typedef FalseType IsMember;
};

template <typename ResultType, typename ClassType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type)>
{
	typedef FunctorArity<4> Arity;
	typedef ResultType Result;
	
	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <
	typename ResultType,
	typename ClassType,
	typename Arg0Type,
	typename Arg1Type,
	typename Arg2Type,
	typename Arg3Type>
struct FunctorTraits<
	ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type) const>
	: public FunctorTraits<
		ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type)>
{
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
struct FunctorTraits<ResultType (Arg0Type, Arg1Type, Arg2Type, Arg3Type)>
{
	typedef FunctorArity<4> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;

	typedef FalseType IsMember;
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
struct FunctorTraits<ResultType (*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type)>
{
	typedef FunctorArity<5> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;
	typedef typename RemoveConstRef<Arg4Type>::Type Argument4;

	typedef FalseType IsMember;
};


template <typename ResultType, typename ClassType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
struct FunctorTraits<ResultType (ClassType::*)(Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type)>
{
	typedef FunctorArity<5> Arity;
	typedef ResultType Result;
	
	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;
	typedef typename RemoveConstRef<Arg4Type>::Type Argument4;

	typedef TrueType IsMember;
	typedef ClassType Class;
};

template <
	typename ResultType,
	typename ClassType,
	typename Arg0Type,
	typename Arg1Type,
	typename Arg2Type,
	typename Arg3Type,
	typename Arg4Type>
struct FunctorTraits<ResultType (ClassType::*)(
		Arg0Type,
		Arg1Type,
		Arg2Type,
		Arg3Type,
		Arg4Type) const>
	: public FunctorTraits<ResultType (ClassType::*)(
		Arg0Type,
		Arg1Type,
		Arg2Type,
		Arg3Type,
		Arg4Type)>
{
};

template <typename ResultType, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
struct FunctorTraits<ResultType (Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type)>
{
	typedef FunctorArity<5> Arity;
	typedef ResultType Result;

	typedef typename RemoveConstRef<Arg0Type>::Type Argument0;
	typedef typename RemoveConstRef<Arg1Type>::Type Argument1;
	typedef typename RemoveConstRef<Arg2Type>::Type Argument2;
	typedef typename RemoveConstRef<Arg3Type>::Type Argument3;
	typedef typename RemoveConstRef<Arg4Type>::Type Argument4;

	typedef FalseType IsMember;
};

}
}

