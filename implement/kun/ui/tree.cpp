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
#include "tree.h"

namespace kun
{
	namespace ui
	{
		void Tree::fillActiveNodesList()
		{
			Node*	node = m_root;
			bool    goingUp = false;
			Coord   pos;
			int     stack[256];
			int		depth = 0;


			m_root->getLocalTransform().getTranslation(pos);

			m_activeNodes.clear();

#		ifdef _DEBUG
			memset(stack, -1, sizeof(stack));
#		endif

			do
			{
				const bool isvisiblefirstvisit = node->m_visible && (goingUp == false);


				if (isvisiblefirstvisit)
				{
					const u32 widgetFlatIndex = (u32) m_activeNodes.size();

					m_activeNodes.emplace_back(node, -1);
					node->m_transform = node->m_localTransform;

					if (node->m_parent != nullptr)
					{
						node->m_transform.mul(node->m_parent->m_transform);
					}

					if (stack[depth] >= 0)
					{
						m_activeNodes[stack[depth]].m_nextIndex = widgetFlatIndex;
					}
					stack[depth] = widgetFlatIndex;
				}

				const bool godown = isvisiblefirstvisit && (node->m_firstChild != nullptr);
				Coord wpos;

				if (godown)
				{
					depth++;
					stack[depth] = -1;

					node->getLocalTransform().getTranslation(wpos);

					pos += wpos;
					node = node->m_firstChild;
				}
				else if (node->m_next != nullptr)	// go right
				{
					node = node->m_next;
					goingUp = false;
				}
				else // go up
				{
					depth--;

					node->getLocalTransform().getTranslation(wpos);

					node = node->m_parent;
					pos -= wpos;

					goingUp = true;
				}
			} 
			while (node != m_root);
		}

	} // ui::
} // kun::

