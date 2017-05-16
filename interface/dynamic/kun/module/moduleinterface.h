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

#define KMOD_BOOTSTRAP extern "C" __declspec(dllexport)

namespace kun
{
	namespace reflect
	{
		class Registry;
	}

    namespace module
    {
		const ModuleID MODULEID_INVALID = 0;
		const ModuleID MODULEID_MAINEXE = 1;

		typedef int (*ModuleBootstrapCall)	(size_t _argSize, void*_arg);
		typedef void(*ModuleInitCall)		(void*);
		typedef void(*ModuleShutdownCall)	(void*);
		typedef void(*ModuleExit)			(void);

		struct BootstrapParam
		{
			const ModuleID			m_moduleID;
			reflect::Registry*		m_registry;
			ModuleInitCall			m_init;
			ModuleShutdownCall		m_shutdown;
			ModuleExit				m_dllexit;
	
			BootstrapParam(ModuleID _moduleID, reflect::Registry* _registry)
				: m_moduleID(_moduleID)
				, m_registry(_registry)
				, m_init	(nullptr)
				, m_shutdown(nullptr)
				, m_dllexit (nullptr)
			{}
		};
	}
}

namespace kmod = kun::module;
