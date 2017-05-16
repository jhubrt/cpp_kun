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
#include "implement\kun\ui\widgetmanager.h"

#include "implement\kun\ui\button.h"
#include "implement\kun\ui\popmenu.h"
#include "implement\kun\ui\events.h"


KREF_CLASS(kun::ui::PopMenuFrameConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::PopMenuFrameConfig)
KREF_CLASS_SUPER(kun::ui::PopMenuFrameConfig,kun::ui::FrameConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::PopMenuFrameConfig)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::PopMenuFrameConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::PopMenuFrameConfig>::Register()
{
	kref::Registry::Register<kun::ui::PopMenuFrameConfig>();
}


KREF_CLASS(kun::ui::PopMenuConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::PopMenuConfig)
KREF_CLASS_SUPER(kun::ui::PopMenuConfig,kun::ui::WidgetConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::PopMenuConfig)
KREF_CLASS_FIELD(kun::ui::PopMenuConfig, m_horizontal),
KREF_CLASS_FIELD(kun::ui::PopMenuConfig, m_padding),
KREF_CLASS_FIELD(kun::ui::PopMenuConfig, m_buttons),
KREF_CLASS_FIELD(kun::ui::PopMenuConfig, m_popmenus),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::PopMenuConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::PopMenuConfig>::Register()
{
	kref::Registry::Register<kun::ui::PopMenuConfig>();
}


KREF_ABSTRACT_CLASS(kun::ui::PopMenuFrame);

KREF_CLASS_SUPERS_BEGIN(kun::ui::PopMenuFrame)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::PopMenuFrame)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::PopMenuFrame)
KREF_CLASS_FUNCTION(kun::ui::PopMenuFrame, s_closeAll, kun::ui::EventCallPrototype),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::PopMenuFrame>::Register()
{
	kref::Registry::Register<kun::ui::PopMenuFrame>();
}



KREF_ABSTRACT_CLASS(kun::ui::PopMenu);

KREF_CLASS_SUPERS_BEGIN(kun::ui::PopMenu)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::PopMenu)
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::PopMenu)
KREF_CLASS_FUNCTION(kun::ui::PopMenu, s_pop, kun::ui::EventCallPrototype),
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::PopMenu>::Register()
{
	kref::Registry::Register<kun::ui::PopMenu>();
}



namespace kun
{
	namespace ui
	{
		PopMenu::PopMenu(
			NameCRC				_name,
			WidgetManager*      _manager,
			Widget*				_parent,
			bool                _horizontal,
			float		        _padding,
			grafx::Font*		_font,
			const std::vector<ButtonConfig>& _buttonconfigs)

			: Widget(_name, _parent, 0, 0, _buttonconfigs[0].m_w, _buttonconfigs[0].m_h)
			, m_state(State_CLOSED)
			, m_horizontal(_horizontal)
			, m_alpha(255)
			, m_padding(_padding)
			, m_elapsedTime(0)
			, m_widgetManager(_manager)
			, m_parentMenu(nullptr)
		{
			int i;

			m_node->setMapState(false, false);

			m_buttonsData.resize(_buttonconfigs.size());

			for (i = (int) _buttonconfigs.size() - 1; i >= 0; i--)
			{
				auto& buttonConfig = _buttonconfigs[i];
				auto& buttonData   = m_buttonsData[i];


				buttonData.m_button = new Button(buttonConfig.m_name, this, false, 0, 0, buttonConfig.m_w, buttonConfig.m_h, _font, buttonConfig.m_text.c_str(), std::vector<std::string>()); // buttonInfo.m_checkButton, buttonInfo.m_surface, buttonInfo.m_palette);

				buttonData.m_button->m_debugName = "popmenubut";

				if ((buttonConfig.m_w > 0) || (buttonConfig.m_h > 0))
				{
					buttonData.m_button->m_node->resize(buttonConfig.m_w, buttonConfig.m_h);
				}

				buttonData.m_button->m_node->setMapState(true, false);
				//buttonData.m_button->setAlpha (0);

				if (buttonConfig.m_type == false)
				{
					assert( typeid(*_parent) == typeid(PopMenuFrame) );
					buttonData.m_button->assignButtonRelease (_parent, _parent->FindMethod(kref::StringCRC("s_closeAll")));
				}
			}

			for (i = 0 ; i < (int) m_buttonsData.size() ; i++)
			{
				m_buttonsData[i].m_moveAnim.setPos(Coord(0, 0));
				m_buttonsData[i].m_moveAnim.setAlpha(0);
			}

			deactivateInput();
		}

		PopMenu::~PopMenu()
		{
			// m_buttons will be destructed by WidgetManager
			m_buttonsData.clear();
		}

		bool PopMenu::pop(const Message& _message)
		{
			unsigned int i;


			m_elapsedTime = 0.0f;

			switch (m_state)
			{
			case State_CLOSED:

				m_node->setMapState(true, false);

				if (m_parentMenu == nullptr) // If first level menu
				{
					m_widgetManager->grab(m_node->getParent()->m_widget);  // Grab the pop menu frame (be carefull : m_parent here)
				}

				// NO BREAK HERE !!!

			case State_CLOSING:
			{
				Coord pos;

				_message.m_sender->m_node->getLocalTransform().getTranslation(pos);

				float x = pos.m_x;
				float y = pos.m_y;

				Node* parentNode = m_node->getParent();

				bool result = _message.m_sender->m_node->toParent(parentNode, x, y);    // Convert into PopUpFrame reference as we do not know if button is child of PopUpFrame or is a menu button
				assert(result);

				m_state = State_OPENING;

				m_node->move(x, y);

				float areaw = parentNode->getSize().m_x - x;
				float areah = parentNode->getSize().m_y - y;

				float x1 = _message.m_sender->m_node->getSize().m_x + m_padding;
				float y1 = _message.m_sender->m_node->getSize().m_y + m_padding;

				x = y = 0;

				if (m_horizontal)
				{
					x = x1;
				}
				else
				{
					y = y1;
				}

				for (auto& buttonData : m_buttonsData)
				{
					if (m_horizontal)
					{
						if ((x + buttonData.m_button->m_node->getSize().m_x) >= areaw)
						{
							x = x1;
							y += y1;
						}
					}
					else
					{
						if ((y + buttonData.m_button->m_node->getSize().m_y) >= areah)
						{
							x += x1;
							y = y1;
						}
					}

					buttonData.m_moveAnim.start(Coord(x, y), m_alpha);

					if (m_horizontal)
					{
						x += buttonData.m_button->m_node->getSize().m_x + m_padding;
					}
					else
					{
						y += buttonData.m_button->m_node->getSize().m_y + m_padding;
					}
				}

				if (m_parentMenu != nullptr)
				{
					m_parentMenu->closeOtherPopMenus(this);
				}
			}

			break;

			case State_OPENING:
			case State_OPENED:

				m_state = State_CLOSING;

				for (i = 0; i < m_buttonsData.size() ; i++)
				{
					m_buttonsData[i].m_moveAnim.start(Coord(0, 0), 0);

					if (m_buttonsData[i].m_subPopMenu != nullptr)
					{
						m_buttonsData[i].m_button->setState(false);
						m_buttonsData[i].m_subPopMenu->close();
					}
				}

				break;
			}

			return true;
		}


		void PopMenu::closeOtherPopMenus(PopMenu* _popMenu)
		{
			for (size_t i = 0; i < m_buttonsData.size() ; i++)
			{
				if (m_buttonsData[i].m_subPopMenu != nullptr)
				{
					if (m_buttonsData[i].m_subPopMenu != _popMenu)
					{
						m_buttonsData[i].m_subPopMenu->close();
						m_buttonsData[i].m_button->setState(false);
					}
				}
			}
		}

		void PopMenu::updatePoping(DeltaTime _elapsedTime)
		{
			float xmax = 0;
			float ymax = 0;



			for (auto& buttonData : m_buttonsData)
			{
				Coord pos;


				buttonData.m_moveAnim.update(_elapsedTime);

				Button* button = buttonData.m_button;

				button->m_node->move(
					buttonData.m_moveAnim.getPos().m_x,
					buttonData.m_moveAnim.getPos().m_y);

				//buttonData.m_button->setAlpha (buttonData.m_moveAnim.getAlpha());
				button->m_node->getLocalTransform().getTranslation(pos);

				const float x = pos.m_x + button->m_node->getSize().m_x;
				if (x > xmax)
					xmax = x;

				const float y = pos.m_y + button->m_node->getSize().m_y;
				if (y > ymax)
					ymax = y;
			}

			// Resize the pop up to bound buttons (necessary for inputs)
			m_node->resize(xmax, ymax);
		}

		void PopMenu::close()
		{
			switch (m_state)
			{
			case State_OPENING:
			case State_OPENED:
				{
					Message message;

					pop(message);
				}

			default: ;
			}
		}

		void PopMenu::update(float _dt)
		{
			switch (m_state)
			{
			case State_OPENING:

				m_elapsedTime += _dt;

				if (m_elapsedTime >= MoveAnim::OPEN_TIME)
				{
					m_state = State_OPENED;
				}

				updatePoping(m_elapsedTime);

				break;

			case State_CLOSING:

				m_elapsedTime += _dt;

				if (m_elapsedTime >= MoveAnim::OPEN_TIME)
				{
					m_state = State_CLOSED;

					m_node->setMapState(false, false);
				}

				updatePoping(m_elapsedTime);

				break;
			}
		}

		void PopMenu::assignSubPopMenu(unsigned int _index, PopMenu* _popMenu)
		{
			_popMenu->m_parentMenu = this;

			auto method = _popMenu->FindMethod(kref::StringCRC("s_pop"));

			m_buttonsData[_index].m_subPopMenu = _popMenu;

			m_buttonsData[_index].m_button->assignButtonPress (nullptr, nullptr);
			m_buttonsData[_index].m_button->assignButtonRelease(_popMenu, method);
			m_buttonsData[_index].m_button->setCheckButtonType(true);
		}


		// PopMenuFrame 
		PopMenuFrame::PopMenuFrame(NameCRC _name, WidgetManager* _widgetManager, Widget* _parent)
			: Widget(_name, _parent, 0, 0, _parent->m_node->getSize().m_x, _parent->m_node->getSize().m_y)
			, m_widgetManager(_widgetManager)
		{
			m_node->setMapState(true, false);
		}

		void PopMenuFrame::assignPopButton(Button* _popButton)
		{
			m_popButton = _popButton;
			m_popButton->m_node->setMapState(true, false);
		}

		bool PopMenuFrame::processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice)  //mouseButtonPress
		{
			if (_inputEvent.m_event.m_type == InputEvent::Type::KEY_PRESS)
			{
				if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_validate)
				{
					Message message;

					message.m_sender = this;

					closeAll(message);
					return true;
				}
			}

			return false;
		}

		bool PopMenuFrame::closeAll(const Message& _message)
		{
			m_popButton->press();
			return true;
		}

		void PopMenuFrame::update(float _dt)
		{
			if (m_widgetManager->getGrab() == this)
			{
				Widget* widget = m_node->getFirstChild()->m_widget;
				bool    allUnmapped = true;


				while (widget != nullptr)
				{
					if (typeid(widget) == typeid(PopMenu))
					{
						bool visible, mapped;

						mapped  = widget->m_node->isMapped();
						visible = widget->m_node->isVisible();

						if (mapped)
						{
							allUnmapped = false;
						}
					}

					widget = widget->getNext();
				}

				if (allUnmapped)
				{
					m_widgetManager->removeGrab();
				}
			}
		}


		Widget* FrameConfig::Create(Widget* _parent, const Skin& _skin) const
		{
			for (auto& child : m_children)
			{
				auto w = safe_cast<WidgetConfig*>(child);
				w->Create(_parent, _skin);
			}

			return _parent;
		}

		Widget* PopMenuFrameConfig::Create(Widget* _parent, const Skin& _skin) const
		{
			auto frame = new PopMenuFrame(m_name, _skin.m_widgetManager, _parent);

			FrameConfig::Create(frame, _skin);

			// retrieve main pop button and assign popevent
			Button* button = nullptr;
			auto child = frame->m_node->getFirstChild();

			while (child != nullptr)
			{
				if (typeid(*child->m_widget) == typeid(Button))
				{
					button = safe_cast<Button*>(child->m_widget);
					break;
				}

				child = child->getNext();
			}

			assert(button != nullptr);
			frame->assignPopButton(button);

			return frame;
		}


		Widget* PopMenuConfig::Create(Widget* _parent, const Skin& _skin) const
		{
			auto popMenu = new PopMenu(m_name, _skin.m_widgetManager, _parent, m_horizontal, m_padding, _skin.m_font, m_buttons);
			u32 index = 0;

			for (auto& p : m_popmenus)
			{
				auto subPopMenu = safe_cast<PopMenu*>(p.Create(_parent, _skin));
				popMenu->assignSubPopMenu(index++, subPopMenu);
			}

			return popMenu;
		}

	}
}
