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

#include "widgetmanager.h"
#include "inputevent.h"
#include "inputdevice.h"
#include "node.h"
#include "widget.h"
#include "tree.h"

#include <algorithm>


namespace kun
{
	namespace ui
	{
		//void debugDraw(float _x, float _y, float _w, float _h, const char* _text);

		class RootWidget : public Widget
		{
			//WidgetManager& m_widgetManager;

			virtual bool processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice)
			{
				/*if (_inputEvent.m_event.m_type == InputEvent::Type::KEY_PRESS)
				{
					if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateLeft)
					{
						m_widgetManager.navigate(LEFT, NONE, _inputDevice);
					}
					else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateRight)
					{
						m_widgetManager.navigate(RIGHT, NONE, _inputDevice);
					}
					else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateUp)
					{
						m_widgetManager.navigate(NONE, UP, _inputDevice);
					}
					else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateDown)
					{
						m_widgetManager.navigate(NONE, DOWN, _inputDevice);
					}
				}*/

				return false;
			}

			virtual void update(float) {}

		public:
			RootWidget(WidgetManager& _widgetManager, float _x, float _y, float _w, float _h, Shared* _shared)
				: Widget(0, nullptr, _x, _y, _w, _h)
				//, m_widgetManager(_widgetManager)
			{
				m_shared = _shared;
			}
		};


		WidgetManager::WidgetManager(float _w, float _h, Shared* _shared)
			: m_grab(nullptr)
		{
			m_root = new RootWidget(*this, 0, 0, _w, _h, _shared);
			
			m_tree = new Tree(m_root->m_node);
		}

		WidgetManager::~WidgetManager()
		{
			for (u32 t = 0; t < m_inputDevices.size(); t++)
			{
				delete m_inputDevices[t];
			}

			m_inputDevices.clear();

			delete m_root;
			m_root = nullptr;
		}

		u32 WidgetManager::addInputDevice(const InputDevice& _inputDevice)
		{
			m_inputDevices.push_back(new InputDevice(_inputDevice));
			return u32(m_inputDevices.size() - 1);
		}

		void WidgetManager::pushInputEvent(const InputEvent& _event)
		{
			bool push = true;
			const u32 queueSize = (u32) m_eventsQueue.size();


			if (queueSize > 0)
			{
				if (_event.m_event.m_type == InputEvent::Type::AXIS_2D_CHANGED)
				{
					InputEvent& lastEvent = m_eventsQueue.back();

					if (lastEvent.m_event.m_type == InputEvent::Type::AXIS_2D_CHANGED)
					{
						if (_event.m_event.m_deviceId == lastEvent.m_event.m_deviceId)
						{
							lastEvent.m_event2D.m_x = _event.m_event2D.m_x;
							lastEvent.m_event2D.m_y = _event.m_event2D.m_y;
						}
					}
				}
			}

			if (push)
			{
				m_eventsQueue.resize(queueSize + 1);
				memcpy(&m_eventsQueue.back(), &_event, sizeof(InputEvent));
			}
		}

		void WidgetManager::update(float _dt)
		{
			m_tree->fillActiveNodesList();
			processInputs();
			updateWidgets(_dt);
		}

		void WidgetManager::processInputs()
		{
			for (u32 t = 0, size = (u32) m_eventsQueue.size(); t < size; t++)
			{
				const InputEvent& inputEvent = m_eventsQueue[t];
				InputDevice& inputDevice = *m_inputDevices[inputEvent.m_event.m_deviceId];

				Widget* widget = nullptr;

				if ((inputEvent.m_event.m_type & InputEvent::Type::MASK_FOCUS_FROM_POSITION) != 0)
				{
					Coord p;

					p.m_x = inputEvent.m_event2D.m_x;
					p.m_y = inputEvent.m_event2D.m_y;

					inputDevice.m_currentFocus = findPointedWidget(p);
				}
				else
				{
					widget = inputDevice.m_currentFocus;
				}

				bool propagate = true;

				if (widget != nullptr)
				{
					do
					{
						if (widget->processInput(inputEvent, inputDevice) == false)	// process input, if not managed by current widget, propagate to parents
						{
							auto parent = widget->m_node->getParent();
							
							widget = parent != nullptr ? parent->m_widget : nullptr;

							if (widget == nullptr)
							{
								break;
							}
						}
						else
						{
							propagate = false;
						}
					} 
					while (propagate);
				}

				if (propagate)
				{
					manageFocus(inputEvent, inputDevice);
				}
			}

			m_eventsQueue.clear();
		}

		void WidgetManager::manageFocus(const InputEvent& _inputEvent, InputDevice& _inputDevice)
		{
			if (_inputEvent.m_event.m_type == InputEventBase::Type::KEY_PRESS)
			{
				Widget* oldFocus = _inputDevice.m_currentFocus;
				Widget* newFocus = nullptr;

				if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateLeft)
				{
					newFocus = nav(oldFocus, 2);
				}
				else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateRight)
				{
					newFocus = nav(oldFocus, 0);
				}
				else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateUp)
				{
					newFocus = nav(oldFocus, 1);
				}
				else if (_inputEvent.m_eventKey.m_keyCode == _inputDevice.m_navigateDown)
				{
					newFocus = nav(oldFocus, 3);
				}

				if (newFocus != nullptr)
				{
					if (oldFocus != nullptr)
					{
						oldFocus->setHasFocus(false);
						oldFocus->focusLost();
					}

					if (oldFocus != newFocus)
					{
						_inputDevice.m_currentFocus = newFocus;
						newFocus->setHasFocus(true);
						newFocus->focusReceived();
					}
				}
			}
		}

		struct NavChoice
		{
			Widget* m_focus;
			Coord	m_min;
		};


		Widget* WidgetManager::nav(Widget* _focus, int _direction)
		{
			std::vector<NavChoice> choices;
			Coord pos;

			Widget* start = m_grab != nullptr ? m_grab : m_root;

			start->m_node->getTransform().getTranslation(pos); // toParent (m_root, x, y);

			auto activeNodes = m_tree->getActiveNodes();

			auto result = std::find_if(activeNodes.begin(), activeNodes.end(), [start](const auto& item)->bool { return item.m_node->m_widget == start; });
			assert(result != activeNodes.end());

			auto end = result->m_nextIndex == -1 ? activeNodes.end() : activeNodes.begin() + result->m_nextIndex;

			for (auto it = result; it != end; it++)
			{
				Widget*	widget = it->m_node->m_widget;

				if (widget->isInputActivated())
				{
					if (widget != _focus)
					{
						Coord p;

						widget->m_node->getTransform().getTranslation(p);

						NavChoice c;

						c.m_focus = widget;
						c.m_min = p;

						choices.push_back(c);
					}
				}
			}

			Coord p(0.0f, 0.0f);

			if (_focus != nullptr)
			{
				_focus->m_node->getTransform().getTranslation(p);

				p.m_x += _focus->m_node->getSize().m_x / 2;
				p.m_y += _focus->m_node->getSize().m_y / 2;
			}

			float best = 10000000.0f;
			Widget* bestw = nullptr;

			for (u32 i = 0, nb = (u32) choices.size(); i < nb; i++)
			{
				const NavChoice& n = choices[i];
				Coord p2 = n.m_min;

				p2.m_x += n.m_focus->m_node->getSize().m_x / 2;
				p2.m_y += n.m_focus->m_node->getSize().m_y / 2;

				float dx = p.m_x - p2.m_x;
				float dy = p.m_y - p2.m_y;

				float adx = fabs(dx);
				float ady = fabs(dy);

				float offset = 0.0f;

				const float EPSILON = 0.05f;

				switch (_direction)
				{
				case 2:
					if (dx <= EPSILON)
						continue;
					if (ady > adx)
					{
						offset = ady - adx;
						offset *= offset;
					}
					break;

				case 1:
					if (dy <= EPSILON)
						continue;
					if (adx > ady)
					{
						offset = adx - ady;
						offset *= offset;
					}
					break;

				case 0:
					if (dx >= -EPSILON)
						continue;
					if (ady > adx)
					{
						offset = ady - adx;
						offset *= offset;
					}
					break;

				case 3:
					if (dy >= -EPSILON)
						continue;
					if (adx > ady)
					{
						offset = adx - ady;
						offset *= offset;
					}
					break;
				}

				float dist2 = (dx * dx) + (dy * dy) + offset;

				if (dist2 < best)
				{
					best = dist2;
					bestw = n.m_focus;
				}
			}

			return bestw;
		}

		void WidgetManager::updateWidgets(float _dt)
		{
			for (auto& i : m_tree->getActiveNodes())
			{
				i.m_node->m_widget->update(_dt);
			}
		}

		Widget* WidgetManager::findPointedWidget(const Coord& _p) const
		{
			Widget* matchingWidget = m_root;

			auto activeNodes = m_tree->getActiveNodes();

			for (size_t t = 0, end = activeNodes.size(); t < end; )
			{
				Node*	  node = activeNodes[t].m_node;
				const int nextindex = activeNodes[t].m_nextIndex;
				bool	  widgetMatch = false;


				if (node->isHit(_p))
				{
					Widget* widget = node->m_widget;

					widgetMatch = true;

					if (widget->isInputActivated())
					{
						{
							Coord pos;
							node->getTransform().getTranslation(pos);
							//debugDraw(pos.m_x - 1, pos.m_y - 1, node->getSize().m_x + 2, node->getSize().m_y + 2, widget->m_debugName.c_str());
						}

						matchingWidget = widget;

						if (nextindex != -1)
						{
							end = nextindex;
						}
					}

					t++;
				}
				else if (nextindex != -1)
				{
					t = nextindex;
				}
				else
				{
					break;
				}
			}

			// Manager grab
			if (m_grab != nullptr)
			{
				if (m_grab->m_node->isParent(matchingWidget->m_node) == false)
				{
					matchingWidget = m_grab;
				}
			}

			return matchingWidget;
		}

		void WidgetManager::DebugDumpActiveWidgetsList(const char* _filename)
		{
			auto activeNodes = m_tree->getActiveNodes();

			FILE* file;

			fopen_s (&file, _filename, "wt");
			assert(file != nullptr);

			for (size_t t = 0; t < activeNodes.size(); t++)
			{
				const Node* node = activeNodes[t].m_node;

				fprintf(file, "%4d: %*c (0x%p) '%s' =>%d\n", (int) t, int(node->debugComputeDepth() * 4), ' ', node, node->m_widget->m_debugName.c_str(), activeNodes[t].m_nextIndex);
			}

			fclose(file);
		}

	} // ui::
} // kun::
