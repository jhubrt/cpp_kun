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

namespace kun
{
	namespace ui
	{
		class Node;

		class Tree
		{
		public:
			struct NodesUnrolled
			{
				Node* m_node;
				int	  m_nextIndex;	// next node index

				NodesUnrolled(Node* _node, int _nextIndex)
					: m_node(_node)
					, m_nextIndex(_nextIndex)
				{}
			};
		
		private:
			Node* m_root;
			std::vector<NodesUnrolled> m_activeNodes;
		
		public:

			Tree(Node* _root)
				: m_root(_root)
			{
				m_activeNodes.reserve(512);
			}

			const std::vector<NodesUnrolled>& getActiveNodes() const { return m_activeNodes; }
			
			Node* getRoot() const { return m_root; }

			bool hasChild(size_t _index) const
			{
				assert(_index < m_activeNodes.size());

				bool isnotlast = (_index + 1) < m_activeNodes.size();
				int  nextindex = m_activeNodes[_index].m_nextIndex;

				if (nextindex == -1)
				{
					return isnotlast;
				}
				else if (isnotlast)
				{
					return nextindex != static_cast<int>(_index + 1);
				}
				else
				{
					return false;
				}
			}

			void fillActiveNodesList();
		};
	}
}
