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

#include "implement\kun\ui\widget.h"
#include "implement\kun\ui\events.h"

namespace kun
{
	namespace grafx
	{
		class Texture;
		class Font;
	}

	namespace ui
	{
		struct ButtonConfig : public WidgetConfig
		{
			bool					 m_checkButton;
			std::string				 m_text;
			std::vector<std::string> m_filenames;
			u8						 m_type;

			virtual Widget* Create(Widget* _parent, const Skin& _skin) const;
		};

		class Button : public Widget
		{
			bool    m_checkButton;
			bool 	m_pressed;
			bool    m_validated;

			const kref::Method* m_methodButtonPress;
			reflect::Polymorph*	m_targetButtonPress;

			const kref::Method* m_methodButtonRelease;
			reflect::Polymorph* m_targetButtonRelease;

			static const u32 NBTEXTURES = 3;

			grafx::Texture* m_texttex;
			grafx::Texture* m_textures[NBTEXTURES];

			typedef Button ThisClass;

		public:

			Button(NameCRC _name, Widget* _parent, bool _checkButton, float _x, float _y, float _w, float _h, grafx::Font* _font, const char* _text, const std::vector<std::string>& _filenames);
			virtual ~Button();

			virtual bool processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice) override;
			virtual void update(float _dt) override;

			void setState			(bool _validated)	{ m_validated = _validated; }
			void setCheckButtonType	(bool _checkButton) { m_checkButton = _checkButton; }

			void assignButtonPress(reflect::Polymorph* _target, const kref::Method* _calleemethod) 
			{ 
				m_targetButtonPress = _target;
				m_methodButtonPress = _calleemethod;
			}

			void assignButtonRelease(reflect::Polymorph* _target, const kref::Method* _calleemethod) 
			{ 
				m_targetButtonRelease = _target;
				m_methodButtonRelease = _calleemethod;
			}
			
			KREF_POLYMORPH_METHOD_WRAPPER2(assignButtonPress, reflect::Polymorph*, const kref::Method*);
			KREF_POLYMORPH_METHOD_WRAPPER2(assignButtonRelease, reflect::Polymorph*, const kref::Method*);

			void press();
		};
	}
}
