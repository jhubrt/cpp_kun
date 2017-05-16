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

#include "interface\dynamic\kun\singleptr.h"
#include "interface\dynamic\kun\module\moduleinterface.h"
#include "interface\dynamic\kun\reflect\registry.h"

#include "implement\kun\display\grafxplugin.h"

namespace kun
{
	template<> reflect::Registry* SinglePtr<reflect::Registry>::ms_instance = nullptr;
	ModuleID reflect::Registry::ms_moduleID = 0;
}

static void Plugin_exit()
{
	kun::reflect::Registry::Unregister ();
}

KMOD_BOOTSTRAP void PluginGrafx_bootstrap (size_t _argsize, void* _arg)
{
	kmod::BootstrapParam* params = static_cast<kmod::BootstrapParam*>(_arg);

	assert (_argsize == sizeof(kmod::BootstrapParam));

	params->m_init     = kun::grafx::init;
	params->m_shutdown = kun::grafx::shutdown;
	params->m_dllexit  = Plugin_exit;

	kun::SinglePtr<kun::reflect::Registry>::Assign(params->m_registry);
	kun::reflect::Registry::InitModuleID(params->m_moduleID);
}
