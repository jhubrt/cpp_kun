// The MIT License (MIT)
//
// Copyright(c) 2016-2017 Jerome Hubert
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "interface\dynamic\kun\reflect\id.h"

namespace kun
{
    namespace reflect
    {
		struct AttributesPack;

		// Protypes are the way to define and identify your functors prototypes
		// they can be used for both method and function (static method) prototypes
#		define KREF_FUNCTION_PROTOTYPE(RESULT,...)	kref::FunctionPrototype<kref::StringCRC(#RESULT "(" #__VA_ARGS__ ")"), RESULT, __VA_ARGS__>

		template<unsigned ID, typename R, typename... Params> struct FunctionPrototype
		{
			typedef R(*TypedCall)(Params...);
			typedef R Result;
			static const kun::u32 id = ID;
		};

#		define KREF_METHOD_PROTOTYPE(RESULT,CLASS,...)	kref::MethodPrototype<kref::StringCRC(#RESULT "(" #CLASS "*" #__VA_ARGS__ ")"), RESULT, CLASS, __VA_ARGS__>

		template<unsigned ID, typename R, typename C, typename... Params> struct MethodPrototype
		{
			typedef R(*TypedCall)(C*, Params...);
			typedef R Result;
			typedef C Class;
			static const unsigned id = ID;
		};

        // Methods store and run methods / static methods from classes
        // Signature is generic : use Prototypes (generic parameters list signature) to specific your calls format

#		define KREF_FUNCTION_STORAGE(PROTOTYPE,F)	kref::Method::functionStorage<PROTOTYPE>(F), PROTOTYPE::id
#		define KREF_METHOD_STORAGE(PROTOTYPE,M)		kref::Method::methodStorage<PROTOTYPE,decltype(M),M>(M), PROTOTYPE::id

#		define KREF_METHOD_WRAPPER0(METHODNAME,TARGET)									static auto s_##METHODNAME (TARGET _this)													-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME();				 }
#		define KREF_METHOD_WRAPPER1(METHODNAME,TARGET,TYPE1)							static auto s_##METHODNAME (TARGET _this, TYPE1 _1)											-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1);				 }
#		define KREF_METHOD_WRAPPER2(METHODNAME,TARGET,TYPE1,TYPE2)						static auto s_##METHODNAME (TARGET _this, TYPE1 _1, TYPE2 _2)								-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1,_2);			 }
#		define KREF_METHOD_WRAPPER3(METHODNAME,TARGET,TYPE1,TYPE2,TYPE3)				static auto s_##METHODNAME (TARGET _this, TYPE1 _1, TYPE2 _2, TYPE3 _3)				     	-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1,_2,_3);		 }
#		define KREF_METHOD_WRAPPER4(METHODNAME,TARGET,TYPE1,TYPE2,TYPE3,TYPE4)			static auto s_##METHODNAME (TARGET _this, TYPE1 _1, TYPE2 _2, TYPE3 _3, TYPE4 _4)			-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1,_2,_3,_4);		 }
#		define KREF_METHOD_WRAPPER5(METHODNAME,TARGET,TYPE1,TYPE2,TYPE3,TYPE4,TYPE5)	static auto s_##METHODNAME (TARGET _this, TYPE1 _1, TYPE2 _2, TYPE3 _3, TYPE4 _4, TYPE5 _5) -> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1,_2,_3,_4,_5);   }

#		define KREF_POLYMORPH_METHOD_WRAPPER0(METHODNAME)								static auto s_##METHODNAME (kref::Polymorph* _this)											-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME();				 }
#		define KREF_POLYMORPH_METHOD_WRAPPER1(METHODNAME,TYPE1)							static auto s_##METHODNAME (kref::Polymorph* _this, TYPE1 _1)								-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1);				 }
#		define KREF_POLYMORPH_METHOD_WRAPPER2(METHODNAME,TYPE1,TYPE2)					static auto s_##METHODNAME (kref::Polymorph* _this, TYPE1 _1, TYPE2 _2)						-> auto { return safe_cast<ThisClass*>(_this)->METHODNAME(_1,_2);			 }


		struct Method
        {
			typedef void (*FuncPtr)(void);

            const NameCRC           name;            
			const FuncPtr			funcptr;
			const NameCRC			prototypeid;
			u32						flags;
			AttributesPack*	const	attributes;
			const char* const       debugname;

			struct Flags
			{
				enum Enum : u32
				{
					STATIC = 0x1,
					RESERVED = 0x1
				};
			};

            void orFlags (u32 _flags)
            {
                assert((_flags & Flags::RESERVED) == 0);
                flags |= _flags;
			}

			void andFlags(u32 _flags)
			{
				flags &= (_flags | Flags::RESERVED);
			}

			bool isStatic() const
			{
				return (flags & Flags::STATIC) != 0;
			}

			// Function (static methods) storage definitions --------------------
			// Look at KREF_FUNCTION_STORAGE. 
			// This method just ensures the Prototype matches function signature
			template <typename Prototype> static constexpr FuncPtr functionStorage(typename Prototype::TypedCall _func)
			{
				return reinterpret_cast<FuncPtr>(_func);
			}

			// Methods storage definitions --------------------------------------
			
			// This hidden stub transforms a method call from a specific class into a simple function call
			template <class T, typename Result, typename... Params> struct PrivateStub
			{
				template <Result(T::*M)(Params...)> static Result stub(T* _obj, Params... _args)
				{
					return (_obj->*M)(_args...);
				}
			};

			// Look at KREF_METHOD_STORAGE (this macro avoid needed redundancy while writing registrations)
			template <typename Prototype, typename MethodType, MethodType Method, typename T, typename... Params> static constexpr FuncPtr methodStorage(typename Prototype::Result(T::* _methodPtr)(Params...))
			{
				return reinterpret_cast<FuncPtr>(PrivateStub<T, typename Prototype::Result, Params...>::template stub<Method>);
			}

			// Invoke the function or method 
			// Checks the Protype used for the call corresponds to the registered Prototype
			template <class Prototype, typename...Types> typename Prototype::Result invoke(Types...args) const
			{
				assert(prototypeid == Prototype::id);
				return reinterpret_cast<typename Prototype::TypedCall>(funcptr)(args...);
			}

            Method& operator=(const Method&) { return *this; } // anti-warning

			bool operator == (NameCRC _name) const { return _name == name; }
        };
    } // reflect::
} // kun::
