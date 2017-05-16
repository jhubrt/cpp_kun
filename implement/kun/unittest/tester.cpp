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

#include "interface\static\kun\unittest\tester.h"

#include "interface\dynamic\kun\reflect\unittestbase.h"
#include "interface\dynamic\kun\reflect\metaclass.h"


namespace kun
{
	namespace unittest
	{
		bool Tester::Run(bool _filterScope, const char* _scope, bool _filterModuleID, ModuleID _moduleID)
		{
			bool ok = true;
			auto registry = kun::SinglePtr<kref::Registry>::Get();
			unsigned  testsprocessed = 0, testssucceeded = 0;

			for(auto itclass : *registry)
			{
				bool shouldRunTest = false;
				const kref::MetaClassInfo* info = itclass.m_info;

				
				if ((_filterModuleID == false) || (_moduleID == itclass.m_moduleID))
				{
					if (info->m_attributes != nullptr)
					{
						for (auto itattr : *(info->m_attributes))
						{
							if (typeid(*itattr) == typeid(kun::reflect::UnitTestAttr))
							{
								const kun::reflect::UnitTestAttr* currentattr = static_cast<const kun::reflect::UnitTestAttr*>(itattr);

								if ((_filterScope == false) || (strcmp(_scope, currentattr->scope) == 0))
								{
									shouldRunTest = true;
									break;
								}
							}
						}
					}
				}

				if (shouldRunTest)
				{
					printf("run unittests from class %s (%lx) module %llx\n", info->m_debugname, info->m_name, itclass.m_moduleID);

					for (auto& itmethod : info->m_methods)
					{						
						printf("	- run method %s (%lx) ", itmethod.debugname, itmethod.name);

						if (itmethod.isStatic())
						{
							testsprocessed++;

							try
							{
								itmethod.invoke<kun::reflect::TestPrototype>();
								testssucceeded++;
								printf("=> succeeded\n");
							}

							catch (...)
							{
								printf("=> failed\n");
								ok = false;
							}
						}
						else
						{
							printf("    ERROR => method not static\n");
							ok = false;
						}
					}
				}
			}

			printf("%d tests succeeded / %d tests\n", testssucceeded, testsprocessed);

			return ok;
		}
	}
}