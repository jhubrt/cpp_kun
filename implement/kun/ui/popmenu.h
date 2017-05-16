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

#include "implement\kun\ui\events.h"
#include "implement\kun\ui\widget.h"
#include "implement\kun\ui\button.h"

namespace kun
{

	class Palette;
	class Surface;

	namespace ui
	{
		class WidgetManager;
		struct ButtonConfig;
		class Button;
		class GraphicInstance;


		struct PopMenuFrameConfig : public FrameConfig
		{			
			virtual Widget* Create(Widget* _parent, const Skin& _skin) const;
		};

		struct PopMenuConfig : public WidgetConfig
		{
			bool						m_horizontal;
			float						m_padding;
			//grafx::Font*				_font,
			std::vector<ButtonConfig>	m_buttons;
			std::vector<PopMenuConfig>	m_popmenus;

			Widget* Create(Widget* _parent, const Skin& _skin) const;
		};

		class PopMenuFrame : public Widget
		{
			WidgetManager*	m_widgetManager;

			virtual bool processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice);
			//virtual void update (DeltaTime _time);

			Button* m_popButton;

			virtual void update(float _dt);

			typedef PopMenuFrame ThisClass;

		public:
			PopMenuFrame(NameCRC _name, WidgetManager* _manager, Widget* _parent);

			void assignPopButton(Button* _popButton);
			bool closeAll(const Message& _message);

			KREF_POLYMORPH_METHOD_WRAPPER1(closeAll, const Message&)
		};


		class PopMenu : public Widget
		{
			enum State
			{
				State_CLOSED,
				State_OPENING,
				State_OPENED,
				State_CLOSING
			};

			State           m_state;
			bool            m_horizontal;
			unsigned char   m_alpha;
			float		    m_padding;

			DeltaTime       m_elapsedTime;

			WidgetManager*	m_widgetManager;
			PopMenu*        m_parentMenu;

		protected:

			struct ButtonData
			{
				// Animation
				MoveAnim m_moveAnim;
				PopMenu* m_subPopMenu;
				Button*  m_button;

				ButtonData()
					: m_subPopMenu(nullptr)
				{}
			};

			std::vector<ButtonData> m_buttonsData;

			void DrawNormal();
			void DrawPushed();

			void updatePoping(DeltaTime _elapsedTime);
			void closeOtherPopMenus(PopMenu* _subPopMenu);

			virtual void update(float _dt);
			virtual bool processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice) { return false; }

			bool pop(const Message& _message);

			typedef PopMenu ThisClass;

		public:

			struct ButtonInfo
			{
				float		m_width;
				float		m_height;
				const char* m_text;
				bool		m_checkButton;

				ButtonInfo()
					: m_width(-1.0f)
					, m_height(-1.0f)
					, m_text(nullptr)
					, m_checkButton(false)
				{ } // Default values 
			};

			PopMenu(
				NameCRC			_name,
				WidgetManager*	_manager,
				Widget*			_parent,
				bool            _horizontal,
				float           _padding,
				grafx::Font*	_font,
				const std::vector<ButtonConfig>& _buttonconfigs);

			virtual ~PopMenu();

			void assignSubPopMenu(unsigned int _index, PopMenu* _popMenu);
			void close();
			
			KREF_POLYMORPH_METHOD_WRAPPER1(pop, const Message&)

			unsigned int	getNbButtons	()						const	{ return (unsigned) m_buttonsData.size(); }
			Button*			getButton		(unsigned int _index)	const	{ return m_buttonsData[_index].m_button; }			
		};
	}
}
