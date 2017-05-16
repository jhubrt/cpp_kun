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

namespace kun
{
	namespace ui
	{
		struct InputEventBase
		{
			struct Type
			{
				static const u32 MASK_FOCUS_FROM_POSITION = 0x1;
				static const u32 MASK_FOCUS_NAVIGABLE = 0x2;

				enum Enum
				{
					AXIS_2D_CHANGED = 0x10 | MASK_FOCUS_FROM_POSITION,
					AXIS_1D_CHANGED = 0x20 | MASK_FOCUS_NAVIGABLE,
					KEY_PRESS = 0x30 | MASK_FOCUS_NAVIGABLE,
					KEY_RELEASE = 0x40 | MASK_FOCUS_NAVIGABLE,
					NONE = 0
				};
			};

			Type::Enum m_type;

			u32		m_deviceId;
			u32		m_triggerId;

			double	m_time;
		};

		struct InputEvent2D : public InputEventBase
		{
			float m_x;
			float m_y;
		};

		struct InputEventKey : public InputEventBase
		{
			int m_keyCode;
		};

		union InputEvent
		{
			typedef InputEventBase::Type Type;	// shortcut

			InputEventBase		m_event;
			InputEvent2D		m_event2D;
			InputEventKey		m_eventKey;
		};
	}
}
