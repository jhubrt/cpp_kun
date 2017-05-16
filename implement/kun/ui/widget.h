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

#include "implement\kun\ui\node.h"

#include "interface\dynamic\kun\reflect\method.h"
#include "interface\dynamic\kun\reflect\rttivector.h"
#include "implement\kun\ui\events.h"

#include <map>

namespace kun
{
	namespace grafx
	{
		class DeviceInterface;
		class Font;
	}

	namespace reflect 
	{
		template <class T> class MetaClass;
	}

	namespace ui
	{
		union InputEvent;
		class InputDevice;
		class Node;
		class GraphicInstanceInterface;
		class Widget;
		class WidgetManager;
		class AssetLibrary;


		struct Shared
		{
			GraphicInstanceInterface&	m_graphicInstanceInterface;
			grafx::DeviceInterface&		m_deviceInterface;
			AssetLibrary&				m_assetLibrary;

			Shared(GraphicInstanceInterface& _graphicInstanceInterface, grafx::DeviceInterface& _deviceInterface, AssetLibrary& _assetLibrary)
				: m_graphicInstanceInterface (_graphicInstanceInterface)
				, m_deviceInterface (_deviceInterface)
				, m_assetLibrary (_assetLibrary)
			{}
		};

		typedef std::map<NameCRC, kref::Polymorph*> ObjectsMap;

		struct Skin
		{
			grafx::Font*	m_font;
			WidgetManager*	m_widgetManager;
			ObjectsMap		m_objectsMap;
		};

		struct WidgetConfig : reflect::Polymorph
		{
			WidgetConfig() : reflect::Polymorph(this) {}

			virtual Widget* Create(Widget* _parent, const Skin& _skin) const = 0;

			NameCRC m_name;

			float m_x;
			float m_y;
			float m_w;
			float m_h;
		};

		struct FrameConfig : public WidgetConfig
		{			
			kref::rttivector m_children;

			virtual Widget* Create(Widget* _parent, const Skin& _skin) const;
		};

		struct EventMapping
		{
			Path8	m_sourcepath;
			NameCRC m_sourcecallassign;
			Path8	m_targetpath;
			NameCRC m_targetcallreceiver;

			EventMapping ()
				: m_sourcecallassign(0) 
				, m_targetcallreceiver(0)
			{}

			EventMapping (const char* _sourcepath, NameCRC _assignmethod, const char* _targetpath, NameCRC _callmethod) 
				: m_sourcecallassign   (_assignmethod)
				, m_targetcallreceiver (_callmethod)
			{
				m_sourcepath.set(_sourcepath);
				m_targetpath.set(_targetpath);
			}
		};

		class Config : public kref::Polymorph
		{
		public:
			std::unique_ptr<FrameConfig>		m_config;
			std::vector<EventMapping>			m_eventsmap;

			Widget* Create(Widget* _parent, const Skin& _skin) const;

			Config() : Polymorph(this) {}
		};

		class Widget : public kref::Polymorph
		{
			friend class WidgetManager;

			bool		m_hasFocus;
			u32			m_input;
			void		setHasFocus(bool _hasFocus) { m_hasFocus = _hasFocus; }

		protected:
			void		setInputMask(u32 _inputmask) { m_input = _inputmask; }

			Shared*		m_shared;

		public:
			Node* const m_node;

			std::string m_debugName;

			Widget(NameCRC _name, Widget* _parent, float _x, float _y, float _w, float _h);

			bool isInputActivated	()			const { return m_input != 0; }
			bool isHandlingInput	(u32 _type) const { return (m_input & _type) != 0; }
			void deactivateInput	()				  { m_input = 0; }
			bool hasFocus			()			const { return m_hasFocus; }

			Widget* getParent() const 
			{ 
				return m_node->getParent()->m_widget; 
			}

			Widget* getFirstChild() const
			{
				Node* node = m_node->getFirstChild();

				if (node != nullptr)
				{
					return node->m_widget;
				}
				
				return nullptr;
			}

			Widget* getNext() const
			{
				Node* node = m_node->getNext();

				if (node != nullptr)
				{
					return node->m_widget;
				}

				return nullptr;
			}

			const kref::Method* FindMethod(kref::NameCRC _methodname) const;

			// events
			virtual bool processInput(const InputEvent& _inputEvent, const InputDevice& _inputDevice) { return false; }
			virtual void update(float _dt)			{}
			virtual void focusLost()				{}
			virtual void focusReceived()			{}
			virtual void visibleStateChanged(bool)	{}
		};


		class DebugWidgetDumper : public Node::Visitor
		{
			FILE* m_file;
			u32 m_depth;

		public:

			DebugWidgetDumper(const char* _filename);

			virtual ~DebugWidgetDumper();

			virtual bool enter(Node* _node);
			virtual void leave(Node* _node);
		};
	}
}

namespace kui = kun::ui;
