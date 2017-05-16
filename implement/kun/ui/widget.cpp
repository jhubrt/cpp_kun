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

#include "node.h"
#include "widget.h"
#include "inputevent.h"


#include "interface\dynamic\kun\reflect\metaclass.h"


KREF_ABSTRACT_CLASS(kun::ui::WidgetConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::WidgetConfig)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::WidgetConfig)
KREF_CLASS_FIELD(kun::ui::WidgetConfig, m_name),
KREF_CLASS_FIELD(kun::ui::WidgetConfig, m_x),
KREF_CLASS_FIELD(kun::ui::WidgetConfig, m_y),
KREF_CLASS_FIELD(kun::ui::WidgetConfig, m_w),
KREF_CLASS_FIELD(kun::ui::WidgetConfig, m_h),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::WidgetConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::WidgetConfig>::Register()
{
	kref::Registry::Register<kun::ui::WidgetConfig>();
}


KREF_CLASS(kun::ui::FrameConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_SUPER(kun::ui::FrameConfig,kun::ui::WidgetConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_FIELD(kun::ui::FrameConfig, m_children),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::FrameConfig>::Register()
{
	kref::Registry::Register<kun::ui::FrameConfig>();
}




KREF_CLASS(kun::ui::Config);

KREF_CLASS_SUPERS_BEGIN(kun::ui::Config)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::Config)
KREF_CLASS_FIELD(kun::ui::Config, m_config),
KREF_CLASS_FIELD(kun::ui::Config, m_eventsmap),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::Config)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::Config>::Register()
{
	kref::Registry::Register<kun::ui::Config>();
}


KREF_CLASS(kun::ui::EventMapping);

KREF_CLASS_SUPERS_BEGIN(kun::ui::EventMapping)
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::EventMapping)
KREF_CLASS_FIELD(kun::ui::EventMapping, m_sourcepath),
KREF_CLASS_FIELD(kun::ui::EventMapping, m_sourcecallassign),
KREF_CLASS_FIELD(kun::ui::EventMapping, m_targetpath),
KREF_CLASS_FIELD(kun::ui::EventMapping, m_targetcallreceiver),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::EventMapping)
KREF_CLASS_METHODS_END

template<> void kref::MetaClass<kun::ui::EventMapping>::Register()
{
	kref::Registry::Register<kun::ui::EventMapping>();
}


namespace kun
{
	namespace ui
	{
		// définir les proto EventAssignPrototype et Receive
		Widget* Config::Create(Widget* _parent, const Skin& _skin) const
		{
			const auto registry = kun::SinglePtr<kref::Registry>::Get();
			auto frame = m_config->Create(_parent, _skin);

//			{
//				kui::DebugWidgetDumper dumper("uidebug.txt");
//				_parent->m_node->visit(dumper);
//			}

			for (auto& eventmap : m_eventsmap)
			{
				auto sourcenode     = _parent->m_node->retrieve(eventmap.m_sourcepath); assert (sourcenode != nullptr);				
				auto sourcewidget	= sourcenode->m_widget;
				auto assignmethod   = sourcewidget->FindMethod(eventmap.m_sourcecallassign); assert(assignmethod != nullptr);				

				const reflect::Method*	  receivemethod = nullptr;
				reflect::Polymorph*		  target		= nullptr;

				if ((eventmap.m_targetpath.size() == 2) && (eventmap.m_targetpath.getName(0) == 0xFFFFFFFF)) // trick to identify app objects events
				{
					auto it = _skin.m_objectsMap.find(eventmap.m_targetpath.getName(1));
					assert(it != _skin.m_objectsMap.end());

					target = it->second;
				}
				else
				{
					target = _parent->m_node->retrieve(eventmap.m_targetpath)->m_widget;
				}

				auto classinfo = registry->FindMetaClass(typeid(*target));
				receivemethod = classinfo->FindMethodRecurs(eventmap.m_targetcallreceiver);
				assert(receivemethod != nullptr);

				assignmethod->invoke<EventAssignPrototype>(sourcewidget, target, receivemethod);
			}

			return frame;
		}

		Widget::Widget(NameCRC _name, Widget* _parent, float _x, float _y, float _w, float _h)
			: reflect::Polymorph(this)
			, m_hasFocus(false)
			, m_input(InputEvent::Type::NONE)			
			, m_node (static_cast<Node*>(Node::Allocate()))
		{
			new(m_node) Node(_name, this, _parent != nullptr ? _parent->m_node : nullptr, _x, _y, _w, _h);

			if (_parent != nullptr)
			{
				m_shared = _parent->m_shared;
				assert(m_shared != nullptr);
			}
		}

		const kref::Method* Widget::FindMethod(kref::NameCRC _methodname) const
		{
			const auto registry = kun::SinglePtr<kref::Registry>::Get();

			auto classinfo = registry->FindMetaClass(typeid(*this));
			assert(classinfo != nullptr);

			return classinfo->FindMethodRecurs(_methodname);
		}

		// Debug: widget tree dumper
		DebugWidgetDumper::DebugWidgetDumper(const char* _filename)
			: m_depth(0)
		{
			fopen_s (&m_file, _filename, "wt");
			assert(m_file != nullptr);
		}

		DebugWidgetDumper::~DebugWidgetDumper()
		{
			if (m_file != nullptr)
			{
				fclose(m_file);
				m_file = nullptr;
			}
		}

		bool DebugWidgetDumper::enter(Node* _node)
		{
			Coord pos;


			_node->getLocalTransform().getTranslation(pos);

			fprintf(m_file, "%*c%s (0x%p) '%s' >%s< ", int(m_depth * 4), ' ', typeid(*_node->m_widget).name(), _node, _node->m_widget->m_debugName.c_str(), _node->isVisible() ? "visible" : "-");
			fprintf(m_file, "(%f ; %f) [%f ; %f] ", pos.m_x, pos.m_y, _node->getSize().m_x, _node->getSize().m_y);
			fprintf(m_file, "transform (%f ; %f) (%f ; %f)\n", 
				_node->getLocalTransform().m[0][2],
				_node->getLocalTransform().m[1][2],
				_node->getTransform().m[0][2],
				_node->getTransform().m[1][2]);

			m_depth++;

			return true;
		}

		void DebugWidgetDumper::leave(Node* _node)
		{
			assert(m_depth >= 0);
			m_depth--;
		}
	}
}
