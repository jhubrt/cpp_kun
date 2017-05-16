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

#include "interface\dynamic\kun\math\vector.h"

namespace kun
{
	namespace ui
	{
		typedef float			DeltaTime;
		typedef math::vector2	Coord;
		typedef math::matrix3	Transform;

		class MoveAnim
		{
			Coord				m_pos;
			Coord				m_target;
			Coord				m_coef;

			int                 m_targetAlpha;
			int                 m_alpha;

		public:

			static DeltaTime OPEN_TIME;
			static DeltaTime INV_OPEN_TIME_SQR;
			static DeltaTime INV_OPEN_TIME;

			const Coord&	getPos  () const { return m_pos; }
			int				getAlpha() const { return m_alpha; }

			void setPos(const Coord& _pos) { m_pos = _pos; }
			void setAlpha(int _alpha) { m_alpha = _alpha; }

			void start(const Coord& _target, int _targetAlpha);
			void update(DeltaTime _elapsedTime);
		};
	}
}
