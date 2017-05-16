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

#include "interface\dynamic\kun\reflect\id.h"
#include "interface\dynamic\kun\reflect\typetraits.h"

#include "implement\kun\ui\transform.h"

namespace kun
{
	namespace ui
	{
		union InputEvent;
		class InputDevice;
		class Appearance;
		class Widget;

		typedef reflect::NameCRC NameCRC;

		class Path
		{
		protected:
			NameCRC* const	m_path;
			const u32		m_capacity;
			u32				m_size;

			Path(NameCRC* _buffer, u32 _capacity)
				: m_path	(_buffer)
				, m_capacity(_capacity)
				, m_size	(0)
			{}

		public:

			u32 size()		const { return m_size;		}
			u32 capacity()	const { return m_capacity;	}
			
			u32 getName(u32 _index) const
			{
				assert (_index < m_size);
				return m_path[_index];
			}

			const u32* getPath() const { return m_path; }

			void set(const char* _path)
			{
				const char* p = _path;

				m_size = 0;

				do
				{
					const char* pnext = strchr(p, '.');

					assert(m_size < m_capacity);

					if (pnext != nullptr)
					{
						m_path[m_size] = reflect::StringCRC(p, pnext - p);
						pnext++;	// jump .
					}
					else
					{
						m_path[m_size] = reflect::StringCRC(p, strlen(p));
					}
					
					m_size++;

					p = pnext;
				}
				while (p != nullptr);
			}

			bool operator< (const Path& _o) const
			{
				u32 t, i;

				for (t = 0, i = 0 ; (t < m_size) && (i < _o.m_size) ; t++, i++)
				{
					if (m_path[t] < m_path[i])
					{
						return true;
					}
					else if (m_path[t] > m_path[i])
					{
						return false;
					}
				}

				return m_size < _o.m_size;
			}
		};

		template <unsigned Capacity> class PathContainer : public Path
		{
			NameCRC m_buffer[Capacity];
		
		public:
			PathContainer() : Path(m_buffer, Capacity) 
			{}

			PathContainer(const char* _path) : Path(m_buffer, Capacity) 
			{
				set (_path);
			}

			PathContainer(const PathContainer& _other) : Path(m_buffer, Capacity) 
			{
				assert(_other.m_size <= Capacity);
				memcpy(m_buffer, _other.m_buffer, _other.m_size * sizeof(*m_buffer));
				m_size = _other.m_size;
			}

			u32* getPath() { return m_buffer; }

			void setSize(unsigned _size) 
			{ 
				assert(_size <= Capacity);
				m_size = _size;
			}

			template<unsigned Capacity2> bool operator< (const PathContainer<Capacity2>& _o) const
			{
				return static_cast<const Path&>(*this) < static_cast<const Path&>(_o);	// call inherited operator
			}

		};

		typedef PathContainer<8> Path8;

		class Node
		{
			friend class Tree;

			NameCRC		m_name;
			Node*		m_prev;
			Node*		m_next;
			Node*		m_firstChild;
			Node*		m_lastChild;
			Node*		m_parent;
			Transform	m_localTransform;
			Transform	m_transform;
			Coord		m_size;
			bool		m_visible;
			bool		m_mapped;

			void recursSetMapState(bool _flag, bool _setIt);

			typedef void*(*NewNode)();
			static NewNode ms_nodeFactory;

		public:

			Widget* const m_widget;

			static void* Allocate() { return ms_nodeFactory(); }

			struct Visitor
			{
				virtual bool enter(Node* _node) = 0;	// return true if you want to recurse this node
				virtual void leave(Node* _node) = 0;
				virtual ~Visitor() {}
			};

			Node(NameCRC _name, Widget* _this, Node* _parent, float _x, float _y, float _w, float _h);

			NameCRC			 getName()			 const { return m_name; }
			const Transform& getLocalTransform() const { return m_localTransform; }
			const Transform& getTransform()		 const { return m_transform; }
			const Coord&     getSize()			 const { return m_size; }

			Node* retrieve(const Path& _path) const;

			bool isVisible()				const { return m_visible; }
			bool isMapped()					const { return m_mapped; }
			bool isParent(Node* _node);

			// global coordinates
			bool isHit(const Coord& _testpoint) const
			{
				Coord pos;

				m_transform.getTranslation(pos);

				float x = _testpoint.m_x - pos.m_x;
				float y = _testpoint.m_y - pos.m_y;

				return (x >= 0.0f) && (x <= m_size.m_x) && (y >= 0.0f) && (y <= m_size.m_y);
			}

			// transform coordinates into parent frame of reference
			// _parent is the New reference node
			// _x, _y these input coordinates will be modified to be expressed into _parent reference
			bool toParent(Node* _parent, float& _x, float& _y);

			void move(float _x, float _y);
			void move(float _x, float _y, float _w, float _h);
			void resize(float _w, float _h);
			void raise();
			void setMapState(bool _show, bool _recurs);
			void visit(Visitor& _visitor);

			Node* getNext			() const { return m_next;		}
			Node* getFirstChild		() const { return m_firstChild; }
			Node* getParent			() const { return m_parent;		}	

			u32 debugComputeDepth() const;
		};
	}

	namespace reflect
	{
		template <unsigned Capacity> struct TypeTraits<::kun::ui::PathContainer<Capacity>>
		{
			static void Serialize (Serializer& _serializer, const void* _adr, const char* _name) 
			{ 
				const auto& path = *static_cast<const ui::PathContainer<Capacity>*>(_adr);
				u32 size = path.size(); 

				_serializer.m_stream.Write(size, _name); 

				// can be optimized this way but need to 
				// - reactivate code in json reader to decode basic arrays
				// - specialize code in type traits to support mutiple values write ...
				// _serializer.m_stream.Write(path.Path::getPath(), size, _name); 

				for (u32 t = 0 ; t < size ; t++)
				{
					_serializer.m_stream.Write(path.getName(t), _name); 	
				}
			}

			static void Deserialize (Deserializer& _deserializer, void* _adr, const char* _name) 
			{ 
				auto& path = *static_cast<ui::PathContainer<Capacity>*>(_adr);
				u32 size = 0; 

				_deserializer.m_stream.Read(size, _name); 

				path.setSize(size);

				for (u32 t = 0 ; t < size ; t++)
				{
					_deserializer.m_stream.Read (path.getPath()[t], _name); 	
				}
			}

			static const size_t	 m_size = sizeof(ui::PathContainer<Capacity>);
			static const NameCRC m_name = StringCRC("kun::ui::PathContainer");
		};
	}
}


namespace kui = kun::ui;
