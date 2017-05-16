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

namespace kun
{
	namespace test
	{
		namespace reflect	{ struct UnitTest; }
		namespace ui		{ struct UnitTest; struct App; }
	}

	namespace ui
	{
		struct WidgetConfig;
		struct ButtonConfig;
		struct CanvasConfig;
		struct PopMenuConfig;
		struct FrameConfig;
		struct PopMenuFrameConfig;
		struct EventMapping;

		class Config;

		class Button;
		class PopMenu;
		class PopMenuFrame;
		class AssetLibrary;
	}
}

void MainRegistry()
{
	kref::MetaClass<kun::test::reflect::UnitTest>::Register();
	kref::MetaClass<kun::test::ui::UnitTest>	 ::Register();
	kref::MetaClass<kun::test::ui::App>			 ::Register();

	kref::MetaClass<kun::ui::AssetLibrary>::Register();
	kref::MetaClass<kun::ui::WidgetConfig>::Register();
	kref::MetaClass<kun::ui::ButtonConfig>::Register();
	kref::MetaClass<kun::ui::CanvasConfig>::Register();
	kref::MetaClass<kun::ui::PopMenuConfig>::Register();
	kref::MetaClass<kun::ui::FrameConfig>::Register();
	kref::MetaClass<kun::ui::PopMenuFrameConfig>::Register();

	kref::MetaClass<kun::ui::EventMapping>::Register();
	kref::MetaClass<kun::ui::Config>::Register();

	kref::MetaClass<kun::ui::Button>::Register();
	kref::MetaClass<kun::ui::PopMenu>::Register();
	kref::MetaClass<kun::ui::PopMenuFrame>::Register();
}
