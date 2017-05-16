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

#include "implement\kun\ui\button.h"
#include "implement\kun\ui\inputevent.h"
#include "implement\kun\ui\inputdevice.h"
#include "implement\kun\ui\graphicinstanceinterface.h"
#include "implement\kun\ui\assetlib.h"

#include "interface\dynamic\kun\display\grafxdevice.h"
#include "interface\dynamic\kun\reflect\metaclass.h"


KREF_CLASS(kun::ui::ButtonConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::ButtonConfig)
KREF_CLASS_SUPER(kun::ui::ButtonConfig,kun::ui::WidgetConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::ButtonConfig)
KREF_CLASS_FIELD(kun::ui::ButtonConfig, m_checkButton),
KREF_CLASS_FIELD(kun::ui::ButtonConfig, m_filenames),
KREF_CLASS_FIELD(kun::ui::ButtonConfig, m_text),
KREF_CLASS_FIELD(kun::ui::ButtonConfig, m_type),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::ButtonConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::ButtonConfig>::Register()
{
	kref::Registry::Register<kun::ui::ButtonConfig>();
}


KREF_ABSTRACT_CLASS(kun::ui::Button);

KREF_CLASS_SUPERS_BEGIN(kun::ui::Button)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::Button)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::Button)
KREF_CLASS_FUNCTION(kun::ui::Button, s_assignButtonPress,	kun::ui::EventAssignPrototype),
KREF_CLASS_FUNCTION(kun::ui::Button, s_assignButtonRelease, kun::ui::EventAssignPrototype),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::Button>::Register()
{
	kref::Registry::Register<kun::ui::Button>();
}

namespace kun
{
	namespace ui
	{
		Button::Button(NameCRC _name, Widget* _parent, bool _checkButton, float _x, float _y, float _w, float _h, grafx::Font* _font, const char* _text, const std::vector<std::string>& _filenames)
			: Widget(_name, _parent, _x, _y, _w, _h)
			, m_checkButton(_checkButton)
			, m_pressed(false)
			, m_validated(false)
			, m_methodButtonPress(nullptr)
			, m_targetButtonPress(nullptr)
			, m_methodButtonRelease(nullptr)
			, m_targetButtonRelease(nullptr)
			, m_texttex(nullptr)
		{
			for (auto& t : m_textures)
			{
				t = nullptr;
			}

			setInputMask ( InputEvent::Type::KEY_PRESS | InputEvent::Type::KEY_RELEASE | InputEvent::Type::AXIS_2D_CHANGED );

			if (_text != nullptr)
			{
				m_texttex = m_shared->m_deviceInterface.CreateTextureFromText(_font, _text);
			}

			if (_filenames.size() > 0)
			{
				u32 t = 0;

				for (auto& f : _filenames)
				{
					assert(t < KUN_ARRAYSIZE(m_textures));
					m_textures[t++] = m_shared->m_assetLibrary.RetrieveTexture(reflect::StringCRC(f.c_str()));
				}
			}
		}

		Button::~Button()
		{
			if (m_texttex != nullptr)
			{
				m_shared->m_deviceInterface.DestroyTexture(m_texttex);
			}

			for (auto t : m_textures)
			{
				if (t != nullptr)
				{
					m_shared->m_deviceInterface.DestroyTexture(t);
				}
			}
		}

		bool Button::processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice)
		{
			if (_inputEvent.m_event.m_type == InputEvent::Type::AXIS_2D_CHANGED)
			{
				return true;
			}

			if (_inputEvent.m_event.m_type == InputEvent::Type::KEY_PRESS)
			{
				if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_validate)
				{	
					m_pressed = true;

					if (m_checkButton)
					{
						m_validated = !m_validated;
					}
					else
					{
						m_validated = true;
					}

					if (m_methodButtonPress != nullptr)
					{
						ui::Message message;

						message.m_sender = this;

						m_methodButtonPress->invoke<EventCallPrototype>(m_targetButtonPress, message);
					}
					
					return true;
				}
			}

			if (_inputEvent.m_event.m_type == InputEvent::Type::KEY_RELEASE)
			{
				if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_validate)
				{
					m_pressed = false;

					if (m_checkButton == false)
					{
						m_validated = false;
					}

					if (m_methodButtonRelease != nullptr)
					{
						ui::Message message;

						message.m_sender = this;

						m_methodButtonRelease->invoke<EventCallPrototype>(m_targetButtonRelease, message);
					}

					return true;
				}
			}

			return false;
		}


		void Button::press()
		{
			Message message;

			message.m_type = Message::Type::BUTTON_PRESS;
			message.m_sender = this;

			m_methodButtonPress->invoke<EventCallPrototype>(m_targetButtonPress, message);
			setState(false);
		}


		void Button::update(float _dt)
		{
			Coord pos;


			m_node->getTransform().getTranslation(pos);

			u32 color = m_validated ? 0x7F0000 : 0x7F7F7F;

			if (hasFocus())
			{
				color *= 2;
			}

			m_shared->m_graphicInstanceInterface.pushCommand(color, pos, m_node->getSize());

			if (m_textures[m_validated] != nullptr)
			{
				m_shared->m_deviceInterface.CopyTexture(m_textures[m_validated], (int)pos.m_x, (int)pos.m_y);
			}

			if (m_texttex != nullptr)
			{
				m_shared->m_deviceInterface.CopyTexture(m_texttex, (int)pos.m_x, (int)pos.m_y);
			}
		}


		Widget* ButtonConfig::Create(Widget* _parent, const Skin& _skin) const
		{
			return new Button(m_name, _parent, m_checkButton, m_x, m_y, m_w, m_h, _skin.m_font, m_text.c_str(), m_filenames );
		}
	}
}
