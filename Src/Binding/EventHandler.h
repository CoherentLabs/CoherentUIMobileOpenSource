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

#include <Binding/FunctorTraits.h>

namespace Coherent
{
/// @namespace Coherent
/// Contains almost all Coherent namespaces, classes and functions

namespace UI
{

class Binder;
class EventArguments;

class ArgumentsBinder
{
public:
	virtual ~ArgumentsBinder()
	{
	}

	template <typename T>
	void Result(const T& value)
	{
		Binder* binder = ResultBegin();
		if (binder)
		{
			CoherentBindInternal(binder, value);
			ResultEnd();
		}
	}

	template <typename T>
	void GetArgument(int argument, T& value)
	{
		CoherentReadInternal(GetArgument(argument), value);
	}

	virtual Binder* ResultBegin() = 0;
	virtual void ResultEnd() = 0;
	virtual void ResultVoid() = 0;
	virtual Binder* GetArgument(int argument) = 0;

	virtual bool HasError() const = 0;

	virtual const EventArguments* GetArguments() = 0;
};

template <typename BoolType>
class ThisHolder
{
protected:
	const void* GetTarget()
	{
		return nullptr;
	}
};

template <>
class ThisHolder<TrueType>
{
public:
	void SetObject(void* object)
	{
		m_Object = object;
	}

protected:
	ThisHolder()
		: m_Object(nullptr)
	{
	}

	const void* GetTarget()
	{
		return m_Object;
	}

	void* m_Object;
};

class IEventHandler
{
public:
	virtual ~IEventHandler() {}
	virtual void Invoke(ArgumentsBinder* binder) = 0;
	virtual const void* GetTarget() = 0;
};

template <typename Functor>
class EventHandler : public IEventHandler, public ThisHolder<typename FunctorTraits<Functor>::IsMember>
{
public:
	typedef FunctorTraits<Functor> Traits;

	EventHandler(Functor functor)
		: m_Functor(functor)
	{
	}

	virtual void Invoke(ArgumentsBinder* binder)
	{
		InvokeStub(binder, typename Traits::Arity(), typename Traits::IsMember(), IsVoid<typename Traits::Result>());
	}

	virtual const void* GetTarget()
	{
		return ThisHolder<typename Traits::IsMember>::GetTarget();
	}

private:

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<0> arity, FalseType isMember, TrueType isVoid)
	{
		m_Functor();
		binder->ResultVoid();
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<0> arity, FalseType isMember, FalseType isVoid)
	{
		binder->Result(m_Functor());
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<0> arity, TrueType isMember, TrueType isVoid)
	{
		typedef typename Traits::Class* ClassPtr;
		(static_cast<ClassPtr>(this->m_Object)->*m_Functor)();
		binder->ResultVoid();
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<0> arity, TrueType isMember, FalseType isVoid)
	{
		typedef typename Traits::Class* ClassPtr;
		binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)());
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<1> arity, FalseType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		if (!binder->HasError())
		{
			m_Functor(argument0);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<1> arity, FalseType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		if (!binder->HasError())
		{
			binder->Result(m_Functor(argument0));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<1> arity, TrueType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		typedef typename Traits::Class* ClassPtr;
		if (!binder->HasError())
		{
			(static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<1> arity, TrueType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		typedef typename Traits::Class* ClassPtr;
		if (!binder->HasError())
		{
			binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<2> arity, FalseType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);

		if (!binder->HasError())
		{
			m_Functor(argument0, argument1);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<2> arity, FalseType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);

		if (!binder->HasError())
		{
			binder->Result(m_Functor(argument0, argument1));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<2> arity, TrueType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);

		typedef typename Traits::Class* ClassPtr;
		if (!binder->HasError())
		{
			(static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<2> arity, TrueType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);

		typedef typename Traits::Class* ClassPtr;
		if (!binder->HasError())
		{
			binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1));
		}
	}
		
		
	void InvokeStub(ArgumentsBinder* binder, FunctorArity<3> arity, FalseType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);

		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		if (!binder->HasError())
		{
			m_Functor(argument0, argument1, argument2);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<3> arity, FalseType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		if (!binder->HasError())
		{
			binder->Result(m_Functor(argument0, argument1, argument2));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<3> arity, TrueType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			(static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<3> arity, TrueType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<4> arity, FalseType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);
		
		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		if (!binder->HasError())
		{
			binder->Result(m_Functor(argument0, argument1, argument2, argument3));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<4> arity, FalseType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);
		
		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		if (!binder->HasError())
		{
			m_Functor(argument0, argument1, argument2, argument3);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<4> arity, TrueType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2, argument3));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<4> arity, TrueType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			(static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2, argument3);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<5> arity, FalseType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typename Traits::Argument4 argument4;
		binder->GetArgument(4, argument4);

		if (!binder->HasError())
		{
			binder->Result(m_Functor(argument0, argument1, argument2, argument3, argument4));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<5> arity, FalseType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typename Traits::Argument4 argument4;
		binder->GetArgument(4, argument4);

		if (!binder->HasError())
		{
			m_Functor(argument0, argument1, argument2, argument3, argument4);
			binder->ResultVoid();
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<5> arity, TrueType isMember, FalseType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typename Traits::Argument4 argument4;
		binder->GetArgument(4, argument4);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			binder->Result((static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2, argument3, argument4));
		}
	}

	void InvokeStub(ArgumentsBinder* binder, FunctorArity<5> arity, TrueType isMember, TrueType isVoid)
	{
		typename Traits::Argument0 argument0;
		binder->GetArgument(0, argument0);
			
		typename Traits::Argument1 argument1;
		binder->GetArgument(1, argument1);
			
		typename Traits::Argument2 argument2;
		binder->GetArgument(2, argument2);

		typename Traits::Argument3 argument3;
		binder->GetArgument(3, argument3);

		typename Traits::Argument4 argument4;
		binder->GetArgument(4, argument4);

		typedef typename Traits::Class* ClassPtr;

		if (!binder->HasError())
		{
			(static_cast<ClassPtr>(this->m_Object)->*m_Functor)(argument0, argument1, argument2, argument3, argument4);
			binder->ResultVoid();
		}
	}

	Functor m_Functor;
};

/// @file
/// @namespace Coherent::UI

/// @name Event handlers
/// @{
/// Creating event handlers from C++ functors

/// Create event handler from generic C++ functor
/// @param functor generic C++ functor, i.e. pointer to static function or object with operator()
/// @return IEventHandler that wraps the functor
template <typename Functor>
IEventHandler* MakeHandler(Functor functor)
{
	return new EventHandler<Functor>(functor);
}

/// Create event handler from C++ method
/// @param object pointer to the instance with which the method will be invoked
/// @param method the C++ method to be invoked
/// @return IEventHandler that wraps the method
/// @warning the object pointer is stored until `Coherent::UI::ViewListener::OnBindingsReleased` is called.
/// Therefore the pointer **must** remain valid until then.
template <typename Method>
IEventHandler* MakeHandler(void* object, Method method)
{
	EventHandler<Method>* handler(new EventHandler<Method>(method));
	handler->SetObject(object);
	return handler;
}
/// @}

typedef void* BoundEventHandle;

}
}
