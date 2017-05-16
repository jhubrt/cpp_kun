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

#include "interface\static\kun\module\modulemanager.h"

#include "interface\dynamic\kun\singleptr.h"

#include <windows.h>

namespace kun
{
	namespace reflect
	{
		class Registry;
	}

	static void logFunc(const char* const _message)
	{
		OutputDebugString(_message);
	}

	Log::LoggerCall Log::m_func = logFunc;		// logger definition should be put elsewhere

	template<> module::Manager* SinglePtr<module::Manager>::ms_instance = nullptr;

	namespace module
    {
		Manager::Manager()
			: m_moduleCounter (MODULEID_MAINEXE + 1)
		{}

		Manager::~Manager()
		{
			for (auto module : m_modules)
			{
				if ( module->m_isLoaded )
				{
					bool success = UnloadModule (module);
					assert(success);
				}
			}
		}

		ModuleDescriptor* Manager::RegisterDynamicLinkModule (const char* _moduleName, const char* _filename)
		{
			assert(RetrieveModuleFromName(_moduleName) == nullptr);

			ModuleDescriptor* module = new ModuleDescriptor;

			module->m_name		= _moduleName;
			module->m_filename		= _filename;
			module->m_isDynamic		= true;

			m_modules.insert(module);

			return module;
		}

		ModuleDescriptor* Manager::RegisterStaticLinkModule(const char* _moduleName, ModuleInitCall _initCall, ModuleShutdownCall _shutdownCall)
		{
			assert(RetrieveModuleFromName(_moduleName) == nullptr);

			ModuleDescriptor* module = new ModuleDescriptor;

			module->m_name		= _moduleName;
			module->m_init		= _initCall;
			module->m_shutdown	= _shutdownCall;

			m_modules.insert(module);

			return module;
		}
	
		static LoadResult::Enum LoadDynamicLibrary(ModuleDescriptor* _module)
		{
			assert(_module->m_isLoaded == false);

			LoadResult::Enum result = LoadResult::FAILED;

			BootstrapParam		bootParams (_module->m_moduleID, SinglePtr<reflect::Registry>::Get());
			ModuleBootstrapCall bootstrap = nullptr;

			_module->m_handle = LoadLibrary(_module->m_filename.c_str());

			if (_module->m_handle != nullptr)
			{
				char bootstrapFuncName[512];

				sprintf_s (bootstrapFuncName, "%s_bootstrap", _module->m_name.c_str());

				// Get the function entry point
				bootstrap = (ModuleBootstrapCall) GetProcAddress((HMODULE)_module->m_handle, bootstrapFuncName);

				if (bootstrap != nullptr)
				{
					result = LoadResult::SUCCESS;
					bootstrap(sizeof(bootParams), &bootParams);
					_module->m_init	    = bootParams.m_init;
					_module->m_shutdown = bootParams.m_shutdown;
					_module->m_dllexit  = bootParams.m_dllexit;
				}
				else
				{
					result = LoadResult::BOOTSTRAP_NOTFOUND;
				}
			}
			else
			{
				result				 = LoadResult::FAILED;
				_module->m_errorCode = GetLastError();
			}

			return result;
		}

   	    LoadResult::Enum Manager::LoadModule(ModuleDescriptor* _module, void* _param)
		{
            LoadResult::Enum result = LoadResult::FAILED;


            if ( _module->m_isDynamic )
            {
				_module->m_moduleID = m_moduleCounter++;
				
				result = LoadDynamicLibrary(_module);
				
				if (result != LoadResult::SUCCESS)
				{
					_module->m_moduleID = MODULEID_INVALID;
				}
            }
            else
            {
				_module->m_moduleID = MODULEID_MAINEXE;
				result = LoadResult::SUCCESS;
            }

            if (result == LoadResult::SUCCESS)
			{
				_module->m_isLoaded = true;

				if (_module->m_init != nullptr)
				{
					_module->m_init(_param);
				}
			}

			return result;
        }

        bool Manager::UnloadModule(ModuleDescriptor* _module, void* _param)
        {
			bool success = true;
			
			if (_module->m_isLoaded)
			{
				assert ( _module->m_moduleID != MODULEID_INVALID );

				if (_module->m_shutdown != nullptr)
				{
					_module->m_shutdown(_param);
				}

				if (_module->m_isDynamic)
				{
					if (_module->m_dllexit != nullptr)
					{
						_module->m_dllexit();
					}

					success = (FreeLibrary((HMODULE)_module->m_handle) != 0);
					
					if (success)
					{
						_module->m_handle   = nullptr;
						_module->m_init     = nullptr;
						_module->m_shutdown = nullptr;
					}
				}

				if (success)
				{
					_module->m_moduleID = MODULEID_INVALID;
					_module->m_isLoaded = false;
				}
			}

            return success;
        }

        const ModuleDescriptor* Manager::RetrieveModuleFromID (ModuleID _moduleId)
        {
            for (auto module : m_modules)
            {
                if ( module->m_moduleID == _moduleId )
                {
                    return module;
                }
            }

            return nullptr;
        }

		const ModuleDescriptor* Manager::RetrieveModuleFromName (const char* _moduleName)
		{
			for (auto module : m_modules)
			{
				if ( module->m_name == _moduleName )
				{
					return module;
				}
			}

			return nullptr;
		}
		
		void Manager::Dump ()
        {
            char line [512];
            u32  nbModules = 0;


            sprintf_s (line, "%s %-64s %-9s %s %-16s %s %s %s\n\n", "Module", "ModuleName", "FileName", "IsLoaded", "ModuleID", "Handle", "InitFunc", "ShutdownFunc");
			Log::log(line);

            for (auto module : m_modules)
            {	
                sprintf_s (line, "0x%p %-9s %-64s %d %-16llu 0x%p 0x%p 0x%p\n",
					module,
					module->m_name.c_str(),
					module->m_isDynamic ? module->m_filename.c_str() : "<static_lib>",
					module->m_isLoaded,
					module->m_moduleID,
					module->m_handle,
					module->m_init,
					module->m_shutdown
				);
        
				Log::log(line);

                nbModules++;
            }

            sprintf_s (line, "\n%lu libraries(s) declared through com system\n\n", nbModules);
			Log::log(line);
		}
    } 
}
