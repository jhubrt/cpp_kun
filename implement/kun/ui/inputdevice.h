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
		struct InputDeviceType
		{
			enum Enum
			{
				NONE = 0x0,

				DEVICE_2D = 0x1,
				DEVICE_TOFOCUS = 0x2,

				MOUSE = 0x10000 | DEVICE_2D,
				TOUCH_SCREEN = 0x20000 | DEVICE_2D,
				KEYBOARD = 0x30000 | DEVICE_TOFOCUS,
				GAMEPAD = 0x40000 | DEVICE_TOFOCUS
			};
		};

		class Widget;

		class InputDevice
		{
			InputDeviceType::Enum m_type;

		public:

			Widget*	m_currentFocus;

			u32 m_deviceId;

			u32	m_navigateLeft;
			u32	m_navigateRight;
			u32	m_navigateUp;
			u32	m_navigateDown;

			u32	m_validate;
			u32	m_cancel;

			InputDevice(InputDeviceType::Enum _type)
				: m_type(_type)
				, m_currentFocus(nullptr)
			{}
		};
	}
}


