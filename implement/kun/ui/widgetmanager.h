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

#include "implement\kun\ui\transform.h"

#include "implement\kun\ui\inputdevice.h"
#include "implement\kun\ui\inputevent.h"

#include "implement\kun\ui\widgetmanagerinterface.h"

namespace kun
{
	namespace ui
	{
		class Node;
		class Tree;
		struct Shared;


		class WidgetManager : public WidgetManagerInterface
		{
			Tree*	m_tree;
			Widget* m_grab;
			Widget*	m_root;

			std::vector<InputDevice*>		m_inputDevices;
			std::vector<InputEvent>			m_eventsQueue;

			Widget* findPointedWidget(const Coord& _p) const;

			void processInputs();
			void updateWidgets(float _dt);
			void manageFocus(const InputEvent& _inputEvent, InputDevice& _inputDevice);

			Widget* nav(Widget* _focus, int _direction);
			//Node* nav_(Widget* _focus, int _direction);

		public:

			WidgetManager(float _w, float _h, Shared* _shared);
			~WidgetManager();

			u32 addInputDevice(const InputDevice& _inputDevice);
			void pushInputEvent(const InputEvent& _event);

			void update(float _dt);

			Widget* getGrab() const { return m_grab; }
			Widget* getRoot() const { return m_root; }

			void grab(Widget* _widget) 
			{ 
				assert(m_grab == nullptr); 
				m_grab = _widget; 
			}
			
			void removeGrab() 
			{ 
				assert(m_grab != nullptr); 
				m_grab = nullptr; 
			}

			void DebugDumpActiveWidgetsList(const char* _filename);
		};
	}
}
