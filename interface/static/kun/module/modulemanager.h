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

#include "interface\dynamic\kun\types.h"
#include "interface\dynamic\kun\module\moduleinterface.h"

#include <set>


namespace kun
{
    namespace module
    {
		struct LoadResult
		{
			enum Enum
			{
				SUCCESS,
				FAILED,
				BOOTSTRAP_NOTFOUND
			};
		};
		
		class ModuleDescriptor
		{
			friend class Manager;
			friend LoadResult::Enum LoadDynamicLibrary(ModuleDescriptor* _module);

			// static info
			kstd::string                    m_name;
			kstd::string                    m_filename;

			// dynamic info (modified at each load)
			ModuleID                        m_moduleID;
			void*                           m_handle;
			int								m_errorCode;	// set if error occured
			ModuleInitCall					m_init;				
			ModuleShutdownCall				m_shutdown;
			ModuleExit						m_dllexit;

			// bools are packed
			bool							m_isLoaded;
			bool                            m_isDynamic;

			ModuleDescriptor()
				: m_moduleID	(MODULEID_INVALID)
				, m_handle		(nullptr)
				, m_init		(nullptr)
				, m_shutdown	(nullptr)
				, m_dllexit     (nullptr)
				, m_isLoaded	(false)
				, m_isDynamic	(false)
			{}
		};


		class Manager
        {
			kstd::set<ModuleDescriptor*> m_modules;
			
			ModuleID m_moduleCounter;

		public:
			Manager();
            ~Manager();

			ModuleDescriptor*		RegisterDynamicLinkModule(const char* _moduleName, const char* _moduleFilename);
			ModuleDescriptor*		RegisterStaticLinkModule (const char* _moduleName, ModuleInitCall _initCall, ModuleShutdownCall _shutdownCall);

            LoadResult::Enum		LoadModule				(ModuleDescriptor*, void* _param = nullptr);
            bool					UnloadModule			(ModuleDescriptor*, void* _param = nullptr);
			const ModuleDescriptor*	RetrieveModuleFromID	(ModuleID _moduleId);
			const ModuleDescriptor*	RetrieveModuleFromName	(const char* _moduleName);

			bool					IsLoaded				(const ModuleDescriptor* _module) const { return _module->m_isLoaded;  }
			bool					IsDynamicLink		    (const ModuleDescriptor* _module) const { return _module->m_isDynamic; }
			void*					GetModuleHandle         (const ModuleDescriptor* _module) const { return _module->m_handle;	   }
			ModuleID				GetModuleID             (const ModuleDescriptor* _module) const { return _module->m_moduleID;  }
            void					Dump ();
        };
    }
}

