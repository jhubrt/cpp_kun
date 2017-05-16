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

#include "implement\kun\ui\transform.h"

namespace kun
{
	namespace ui
	{
		const float F_OPEN_TIME = 0.35f;

		DeltaTime MoveAnim::OPEN_TIME(F_OPEN_TIME);
		DeltaTime MoveAnim::INV_OPEN_TIME_SQR((1.0f / F_OPEN_TIME) * (1.0f / F_OPEN_TIME));
		DeltaTime MoveAnim::INV_OPEN_TIME(1.0f / F_OPEN_TIME);

		void MoveAnim::start(const Coord& _target, int _targetAlpha)
		{
			m_targetAlpha = _targetAlpha;
			m_target = _target;

			m_coef = m_pos;
			m_coef -= m_target;
			m_coef *= INV_OPEN_TIME_SQR;
		}

		void MoveAnim::update(DeltaTime _elapsedTime)
		{
			if (_elapsedTime >= OPEN_TIME)
			{
				m_alpha = m_targetAlpha;
				m_pos = m_target;
			}
			else
			{
				DeltaTime lapsToEnd(OPEN_TIME);

				lapsToEnd -= _elapsedTime;

				// Compute alpha
				DeltaTime alphaF = DeltaTime(m_targetAlpha - m_alpha);

				alphaF *= lapsToEnd;
				alphaF *= INV_OPEN_TIME;

				m_pos = m_coef;

				m_pos *= lapsToEnd;
				m_pos *= lapsToEnd;

				m_pos += m_target;
			}
		}
	}
}
