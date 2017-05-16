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


#include "stdafx.h"

#include <conio.h>

#include <initializer_list>
#include <vector>
#include <typeinfo>
#include <type_traits>

#include "interface\dynamic\kun\reflect\id.h"
#include "interface\dynamic\kun\reflect\metaclass.h"
#include "interface\dynamic\kun\reflect\registry.h"
#include "interface\dynamic\kun\reflect\unittestbase.h"
#include "interface\dynamic\kun\reflect\jsonwriterstream.h"
#include "interface\dynamic\kun\reflect\binstream.h"
#include "interface\dynamic\kun\reflect\file.h"
#include "interface\dynamic\kun\reflect\rttivector.h"

#include "implement\kun\reflect\jsonreader.h"



class B : public kref::Polymorph
{
	friend class kref::MetaClass<B>;

	int  pipi;

public:
	B() : Polymorph(this) {}
};

class D : public kref::Polymorph
{
	friend class kref::MetaClass<D>;

	int  D_titi;
	int  D_toto;

public:
	D() : Polymorph(this) {}
};

class A : public kref::Polymorph
{
	friend class kref::MetaClass<A>;

	B	 m_subobject;
public:

	std::vector<B>			m_subobjectsVector;
	std::vector<kun::s16>	m_shortVector;

	kref::rttivector m_variants;

	char		m_dummy;
	int			popo;
	std::string text;

	A() 
		: Polymorph(this)
		, m_dummy(123) 		
	{}

	int test(int _num)
	{
		printf("popo = %d, _num = %d\n", m_dummy, _num);
		return _num + 2;
	}

	int consttest(int _num) const
	{
		printf("popo = %d, _num = %d\n", m_dummy, _num);
		return 4321;
	}

	static unsigned statictest(int _test)
	{
		printf("static test = %d\n", _test);
		return _test + 1;
	}
};

/*struct O
{
virtual ~O() {}
char m_um[8];
};*/

class C : /*public O,*/ public B
{
	friend class kref::MetaClass<C>;
};


struct SpecAttrib : public kref::Attribute
{
	int m_dummy;

	SpecAttrib(int _d)
		: m_dummy(_d)
	{
		printf("construct at %p\n", this);
	}

	~SpecAttrib()
	{
		printf("destruct at %p\n", this);
	}
};



/*
template<class Base, class Derived> constexpr ptrdiff_t Base2DerivedOffset()
{
constexpr A* p = (A*)(void*)8;
return 0;
//static const ptrdiff_t value = ;
};
*/

// A
KREF_CLASS(A);

KREF_CLASS_SUPERS_BEGIN(A)
KREF_CLASS_SUPERS_END

// { krefl::StringCRC("m_dummy")		, krefl::TypeTraits<decltype(A::m_dummy)>::m_name	 , &KREFL_GETFIELD(A, m_dummy)		, sizeof(A::m_dummy)	, krefl::TypeTraits<decltype(A::m_dummy)>::Serialize, krefl::TypeTraits<decltype(A::m_dummy)>::Deserialize	  , "m_dummy"		  },
// { krefl::StringCRC("popo")			, krefl::TypeTraits<decltype(A::popo	)>::m_name	 , &KREFL_GETFIELD(A, popo)			, sizeof(A::popo)		, krefl::TypeTraits<decltype(A::popo) >::Serialize  , krefl::TypeTraits<decltype(A::popo)>::Deserialize		  , "popo"			  },
// { krefl::StringCRC("m_subobject")	, krefl::TypeTraits<decltype(A::m_subobject)>::m_name, &KREFL_GETFIELD(A, m_subobject)  , sizeof(A::m_subobject), krefl::TypeTraits<decltype(A::popo) >::Serialize  , krefl::TypeTraits<decltype(A::m_subobject)>::Deserialize, "m_subobject"	  }

KREF_CLASS_FIELDS_BEGIN(A)
KREF_CLASS_FIELD(A, m_dummy),
KREF_CLASS_FIELD(A, popo),
KREF_CLASS_FIELD(A, m_subobject),
KREF_CLASS_FIELD(A, m_subobjectsVector),
KREF_CLASS_FIELD(A, m_shortVector),
KREF_CLASS_FIELD(A, m_variants),
KREF_CLASS_FIELD(A, text),
KREF_CLASS_FIELDS_END

using MyMethodPrototype = KREF_METHOD_PROTOTYPE(int, A, int);			//kref::MethodPrototype<kref::StringCRC("int A::"), int, A, int>
using MyFunctionPrototype = KREF_FUNCTION_PROTOTYPE(unsigned, int);		// kref::FunctionPrototype<kref::StringCRC("int A::"), unsigned, int>;
																		//using MyConstMethodPrototype = kref::MethodPrototype<kref::StringCRC("int A::"), int, const A, int>;

																		// { "test", kref::StringCRC("test"), KREF_METHOD_STORAGE(MyMethodPrototype, &A::test) },
																		// { "statictest", kref::StringCRC("statictest"), KREF_FUNCTION_STORAGE(MyFunctionPrototype, A::statictest) },
																		// { "consttest", kref::StringCRC("consttest"), KREF_METHOD_STORAGE(MyConstMethodPrototype, &A::consttest) }

KREF_CLASS_METHODS_BEGIN(A)
KREF_CLASS_METHOD(A, test, MyMethodPrototype),
KREF_CLASS_FUNCTION(A, statictest, MyFunctionPrototype),
KREF_CLASS_METHODS_END


template<>void kref::MetaClass<A>::Register()
{
	kref::Registry::Register<A>();

	KREF_FIELD_ATTRIBUTES(m_dummy, SpecAttrib(1234), SpecAttrib(2233));
	KREF_METHOD_ATTRIBUTES(test, SpecAttrib(1), SpecAttrib(2));
	KREF_CLASS_ATTRIBUTES(SpecAttrib(1));
}


// B
KREF_CLASS(B);

KREF_CLASS_SUPERS_BEGIN(B)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(B)
KREF_CLASS_FIELD(B, pipi),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(B)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<B>::Register()
{
	kref::Registry::Register<B>();
}

// D
KREF_CLASS(D);

KREF_CLASS_SUPERS_BEGIN(D)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(D)
KREF_CLASS_FIELD(D, D_titi),
KREF_CLASS_FIELD(D, D_toto),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(D)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<D>::Register()
{
	kref::Registry::Register<D>();
}


// C
KREF_CLASS(C);

KREF_CLASS_SUPERS_BEGIN(C)
KREF_CLASS_SUPER(C, B),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(C)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(C)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<C>::Register()
{
	kref::Registry::Register<C>();
}

//kref::AttributesStorage<SpecAttrib, SpecAttrib> attrib(SpecAttrib(1), SpecAttrib(2));
//auto attrib = kref::MetaClass<A>::m_info.m_methods.find(kref::StringCRC("test"))->attributes;
//printf("%s\n", typeid(*attrib).name());
//printf("%lld\n", attrib->size());
//printf("%d\n", dynamic_cast<const SpecAttrib*>((*attrib)[0])->m_dummy);
//printf("%d\n", dynamic_cast<const SpecAttrib*>((*attrib)[1])->m_dummy);*/


//---------------------------------------------------------------------------
// Unit test class
//---------------------------------------------------------------------------
namespace kun
{
	namespace test
	{
		namespace reflect
		{
			struct UnitTest
			{
				static void Register()
				{
					kref::MetaClass<A>::Register();
					kref::MetaClass<B>::Register();
					kref::MetaClass<C>::Register();
					kref::MetaClass<D>::Register();

					kun::SinglePtr<kref::Registry>::Get()->Dump();
				}

				static void TestStaticMetaClasses()
				{
					// Generic access
					constexpr kref::NameCRC className = kref::StringCRC("A");
					const auto metaClass = kun::SinglePtr<kref::Registry>::Get()->FindMetaClass(className);
					
					KUTEST_CHECK(metaClass != nullptr);
					KUTEST_CHECK(strcmp(metaClass->m_debugname,"A") == 0);

					// Specialized access
					KUTEST_CHECK(strcmp(kref::MetaClass<B>::m_info.m_debugname, "B") == 0);
				}

				static void TestStaticFields()
				{
					constexpr kref::NameCRC fieldName = kref::StringCRC("m_dummy");
					const auto& fields = kref::MetaClass<A>::m_info.m_fields;

					auto itfield = std::find(fields.begin(), fields.end(), fieldName);

					KUTEST_CHECK(itfield != fields.end());
					KUTEST_CHECK(strcmp(itfield->debugname, "m_dummy") == 0);
				}

				static void TestDynamicMetaClasses()
				{
					// Use factory to create generically an object
					// Dynamic access to on of its field and print its value
					constexpr kref::NameCRC className = kref::StringCRC("A");
					const auto metaClass = kun::SinglePtr<kref::Registry>::Get()->FindMetaClass(className);

					KUTEST_CHECK(metaClass != nullptr);

					auto pa = metaClass->New<A>();

					KUTEST_CHECK(typeid(*pa) == typeid(A));

					constexpr kref::NameCRC fieldName = kref::StringCRC("m_dummy");
					const auto& fields = kref::MetaClass<A>::m_info.m_fields;

					auto itfield = std::find(fields.begin(), fields.end(), fieldName);
					KUTEST_CHECK(itfield != fields.end());
					KUTEST_CHECK(123 == *static_cast<decltype(A::m_dummy)*>(itfield->getfield(pa)));

					pa->text = "testtext";
					pa->m_variants.push_back(new B);
					pa->m_variants.push_back(new D);

					// Serialize object
					{
						kref::StdFile file("D:\\dev\\framework\\test\\jsontest.json", kref::StdFile::WRITE);
						kref::JSonWriterStream stream(file);

						printf("Serialization:\n");

						pa->m_subobjectsVector.resize(2);
						pa->m_shortVector.resize(4);

						//kref::Serializer seralizer(stream, kref::SerializeClass);
						kref::Serializer seralizer(stream, kref::SerializeClassLight);

						kref::Serialize(seralizer, *pa);
					}
					{
						kref::BinStream<kref::StdFile> binstream("D:\\dev\\framework\\test\\metaclass.bin", kref::StdFile::WRITE);
						kref::StdFile tracefile ("tracefile2.log", kref::StdFile::WRITE);
						auto stream = binstream.CreateTracer(&tracefile);

						//kref::Serializer serializer(*stream, kref::SerializeClass);
						kref::Serializer serializer(*stream, kref::SerializeClassLight);

						kref::Serialize(serializer, *pa);

						delete stream;
					}

					{
						kref::Stream* jsonstream;

						{
							kref::StdFile tracefileout ("tracefile2.log", kref::StdFile::WRITE);
							jsonstream = kref::JsonRead("D:\\dev\\framework\\test\\jsontest.json", &tracefileout);
						}

						kref::StdFile tracefile ("tracefile2.log", kref::StdFile::READ);

						kref::Stream* stream = jsonstream->CreateTracer(nullptr, &tracefile, kref::TracerBinStream::Check::STRONG);

						void* object;

						//kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonly);
						kref::Deserializer deserializer(*stream, kref::DeserializeClass, kref::DeserializeClassBodyonlyLight);

						auto metaclass = kun::SinglePtr<kref::Registry>::Get()->TryDeserializeClass(deserializer, object);
						assert(metaclass != nullptr);
						assert(strcmp(metaclass->m_debugname, "A") == 0);

						A* typedobject = static_cast<A*>(object);

						typedobject->m_dummy = typedobject->m_dummy;

						delete stream;
					}
				}

				static void TestMethods()
				{
					A a;

					const auto& methods = kref::MetaClass<A>::m_info.m_methods;

					const auto& itmethod = std::find(methods.begin(), methods.end(), kref::StringCRC("test"));
					KUTEST_CHECK(itmethod != methods.end());
					KUTEST_CHECK(itmethod->invoke<MyMethodPrototype>(&a, 1234) == 1236);

					//printf("%s\n\n", typeid(std::result_of<decltype(&A::test)(A*, int)>::type).name());

					const auto& itfunction = std::find(methods.begin(), methods.end(), kref::StringCRC("statictest"));
					KUTEST_CHECK(itfunction != methods.end());
					KUTEST_CHECK(itfunction->invoke<MyFunctionPrototype>(1379) == 1380);
					KUTEST_CHECK(1379 == ((MyFunctionPrototype::TypedCall)itfunction->funcptr)(1378));
				}


				template<unsigned ID, typename Result, typename... Params> struct FunctionProto
				{
					typedef Result(*CallConvention)(Params...);
					static const unsigned id = ID;
				};
			};
		}
	}
}

// Unit test class registration
KREF_CLASS(kun::test::reflect::UnitTest);

KREF_CLASS_SUPERS_BEGIN(kun::test::reflect::UnitTest)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::test::reflect::UnitTest)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::test::reflect::UnitTest)
KREF_CLASS_FUNCTION(kun::test::reflect::UnitTest, Register				, kun::reflect::TestPrototype),
KREF_CLASS_FUNCTION(kun::test::reflect::UnitTest, TestStaticMetaClasses	, kun::reflect::TestPrototype),
KREF_CLASS_FUNCTION(kun::test::reflect::UnitTest, TestStaticFields		, kun::reflect::TestPrototype),
KREF_CLASS_FUNCTION(kun::test::reflect::UnitTest, TestDynamicMetaClasses, kun::reflect::TestPrototype),
KREF_CLASS_FUNCTION(kun::test::reflect::UnitTest, TestMethods			, kun::reflect::TestPrototype),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::test::reflect::UnitTest>::Register()
{
	kref::Registry::Register<kun::test::reflect::UnitTest>();

	KREF_CLASS_ATTRIBUTES(kun::reflect::UnitTestAttr("kref"));
}
