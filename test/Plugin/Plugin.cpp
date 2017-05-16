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

#include "interface\dynamic\kun\reflect\metaclass.h"
#include "interface\dynamic\kun\reflect\registry.h"
#include "interface\dynamic\kun\reflect\unittestbase.h"


struct P
{
	friend class kref::MetaClass<P>;

	int m_test;

	static void test()
	{
		printf ("autotest plugin\n");
	}
};


// P
KREF_CLASS(P);

KREF_CLASS_SUPERS_BEGIN(P)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(P)
KREF_CLASS_FIELD(P, m_test),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(P)
KREF_CLASS_FUNCTION(P, test, TestPrototype),
KREF_CLASS_METHODS_END


template<>void kref::MetaClass<P>::Register()
{
	kref::Registry::Register<P>();

	KREF_CLASS_ATTRIBUTES(kun::reflect::UnitTestAttr("plugin"));
}

namespace Plugin
{
	void init (void*) 
	{
		kref::MetaClass<P>::Register();		
	}

	void shutdown(void*) 
	{
		printf ("shutdown\n");
	}
}
