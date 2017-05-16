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
#include "inputevent.h"
#include "widget.h"		// TEMPORARY


KREF_CLASS(kun::ui::FrameConfig);

KREF_CLASS_SUPERS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_SUPER(kun::ui::FrameConfig,kun::ui::WidgetConfig),
KREF_CLASS_SUPERS_END

KREF_CLASS_FIELDS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_FIELD(kun::ui::FrameConfig, m_children),
KREF_CLASS_FIELDS_END

KREF_CLASS_METHODS_BEGIN(kun::ui::FrameConfig)
KREF_CLASS_METHODS_END


namespace kun
{
	namespace ui
	{
		static void* allocateNode () { return malloc(sizeof(Node)); }

		Node::NewNode Node::ms_nodeFactory = allocateNode;	// TEMPORARY

		Node::Node(NameCRC _name, Widget* _this, Node* _parent, float _x, float _y, float _w, float _h)
			: m_name(_name)
			, m_prev(nullptr)
			, m_firstChild(nullptr)
			, m_lastChild(nullptr)
			, m_parent(_parent)
			, m_widget(_this)
		{
			m_localTransform.setIndentity();
			m_localTransform.setTranslation(_x, _y);

			m_size.m_x = _w;
			m_size.m_y = _h;

			if (_parent != nullptr)
			{
				m_next = _parent->m_firstChild;

				if (m_next != nullptr)
				{
					m_next->m_prev = this;
				}
				else
				{
					_parent->m_lastChild = this;
				}

				_parent->m_firstChild = this;

				m_visible = _parent->m_visible;
				m_mapped = true;
			}
			else
			{
				m_next = nullptr;
				m_visible = true;
				m_mapped = true;
			}
		}

		Node* Node::retrieve(const Path& _path) const 
		{
			const Node* node = this;

			for (u32 t = 0, size = _path.size() ; t < size ; t++)
			{
				Node* child = node->m_firstChild;
				const u32 name = _path.getName(t);
				bool found = false;


				while (child != nullptr)
				{
					if (child->m_name == name)
					{
						node = child;
						found = true;
						break;
					}

					child = child->m_next;
				}

				if (found == false)
				{
					return nullptr;
				}
			}

			return const_cast<Node*>(node);
		}

		void Node::raise()
		{
			if ((m_prev != nullptr) && (m_parent != nullptr))
			{
				if (m_next != nullptr)
				{
					m_next->m_prev = m_prev;
				}

				m_prev->m_next = m_next;

				if (m_parent->m_lastChild == this)
				{
					m_parent->m_lastChild = m_prev;
				}

				m_next = m_parent->m_firstChild;
				m_next->m_prev = this;
				m_prev = nullptr;
				m_parent->m_firstChild = this;
			}
		}

		void Node::move(float _x, float _y)
		{
			m_localTransform.setTranslation(_x, _y);
		}

		void Node::move(float _x, float _y, float _w, float _h)
		{
			m_localTransform.setTranslation(_x, _y);

			m_size.m_x = _w;
			m_size.m_y = _h;
		}

		void Node::resize(float _w, float _h)
		{
			m_size.m_x = _w;
			m_size.m_y = _h;
		}

		// Recompute visible flag and optionally set mapped flag recursively
		// - _setIt     - true  : m_mapped flags will be set 
		//				- false : m_mapped flags are not modified
		// - _flag      value to set into m_mapped flags when _setIt is true
		void Node::recursSetMapState(bool _flag, bool _setrecursively)
		{
			auto node = m_lastChild;

			while (node != nullptr)
			{
				if (_setrecursively)
				{
					node->m_mapped = _flag;
				}

				bool visible = node->m_parent->m_visible & node->m_mapped;

				if (visible != node->m_visible)
				{
					node->m_visible = visible;
					node->m_widget->visibleStateChanged(visible);
				}

				node->recursSetMapState(_flag, _setrecursively);
				node = node->m_prev;
			}
		}

		void Node::visit(Visitor& _visitor)
		{
			const bool shouldRecurse = _visitor.enter(this);

			if (shouldRecurse)
			{
				Node* node = m_firstChild;

				while (node != nullptr)
				{
					node->visit(_visitor);
					node = node->m_next;
				}
			}

			_visitor.leave(this);
		}


		u32 Node::debugComputeDepth() const
		{
			const Node* node = this;
			int depth = -1;


			while (node != nullptr)
			{
				depth++;
				node = node->m_parent;
			}

			return depth;
		}


		void Node::setMapState(bool _show, bool _setrecursively)
		{
			bool visible = m_parent->m_visible & _show;

			m_mapped = _show;

			if (visible != m_visible)
			{
				m_visible = visible;
				m_widget->visibleStateChanged(visible);
			}

			recursSetMapState(_show, _setrecursively);
		}


		bool Node::isParent(Node* _widget)
		{
			while (_widget != this)
			{
				_widget = _widget->m_parent;

				if (_widget == nullptr)
				{
					return false;
				}
			}

			return true;
		}

		bool Node::toParent(Node* _parent, float& _x, float& _y)
		{
			if (this != _parent)
			{
				Node* node = m_parent;


				while (node != _parent)
				{
					_x += node->m_localTransform.m[0][2];
					_y += node->m_localTransform.m[1][2];

					node = node->m_parent;

					if (node == nullptr)
					{
						return false;
					}
				}
			}

			return true;
		}
	}
}
