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

#include "implement\kun\ui\canvas.h"

#include "interface\dynamic\kun\reflect\metaclass.h"


KREF_CLASS(kun::ui::CanvasConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::CanvasConfig)
KREF_CLASS_SUPER(kun::ui::CanvasConfig,kun::ui::FrameConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::CanvasConfig)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::CanvasConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::CanvasConfig>::Register()
{
	kref::Registry::Register<kun::ui::CanvasConfig>();
}



namespace kun
{
	namespace ui
	{
		Canvas::Canvas(NameCRC _name, Widget* _parent, float _x, float _y, float _w, float _h)
			: Widget(_name, _parent, _x, _y, _w, _h)
		{}

		Widget* CanvasConfig::Create(Widget* _parent, const Skin& _skin) const
		{
			auto canvas = new Canvas(m_name, _parent, m_x, m_y, m_w, m_h);

			for (auto& child : m_children)
			{
				auto w = safe_cast<WidgetConfig*>(child);
				w->Create(canvas, _skin);
			}

			return canvas;
		}
	}
}
