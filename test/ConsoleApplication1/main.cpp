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

#include "interface\static\kun\unittest\tester.h"
#include "interface\static\kun\module\modulemanager.h"

#include "interface\dynamic\kun\reflect\metaclass.h"
#include "interface\dynamic\kun\display\grafxdevice.h"

#include "implement\kun\reflect\registryimpl.h"
#include "implement\kun\object\identifiablesetimpl.h"

void Initialize(void)
{
	kun::SinglePtr<kref::Registry>		 ::Assign(new kref::RegistryImpl(kmod::MODULEID_MAINEXE));
	kun::SinglePtr<kmod::Manager>		 ::Assign(new kmod::Manager());
	kun::SinglePtr<kobj::IdentifiableSet>::Assign(new kobj::IdentificableSetImpl());
}

void Shutdown(void)
{
	kun::SinglePtr<kobj::IdentifiableSet>::Destroy();
	kun::SinglePtr<kmod::Manager>::Destroy();
	
	kun::SinglePtr<kref::Registry>::Get()->Dump();
	kun::SinglePtr<kref::Registry>::Destroy();
}

void MainRegistry();

int main()
{
	Initialize();

	MainRegistry();

	auto registry = kun::SinglePtr<kref::Registry>::Get();

	registry->Dump();

	auto moduleManager = kun::SinglePtr<kmod::Manager>::Get();

	kmod::ModuleDescriptor* plugin		= moduleManager->RegisterDynamicLinkModule("Plugin", "Plugin.dll");
	kmod::ModuleDescriptor* plugingrafx = moduleManager->RegisterDynamicLinkModule("PluginGrafx", "PluginGrafx.dll");

	moduleManager->LoadModule(plugin,	   nullptr);
	moduleManager->LoadModule(plugingrafx, nullptr);

	registry->Dump();

	kutest::Tester::Run("kref");
	kutest::Tester::Run("kui");

	Shutdown();

	printf ("press any key\n");
	_getch();

	return 0;
}

