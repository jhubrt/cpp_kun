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
	namespace math
	{
		struct vector2
		{
			float m_x;
			float m_y;

			vector2() {}

			vector2(float _x, float _y)
				: m_x(_x)
				, m_y(_y)
			{}

			void operator += (const vector2& _o)
			{
				m_x += _o.m_x;
				m_y += _o.m_y;
			}

			void operator -= (const vector2& _o)
			{
				m_x -= _o.m_x;
				m_y -= _o.m_y;
			}

			void operator *= (float _m)
			{
				m_x *= _m;
				m_y *= _m;
			}
		};


		struct vector3
		{
			float m_x;
			float m_y;
			float m_z;

			vector3() {}

			vector3(const vector2& _in)
				: m_x(_in.m_x)
				, m_y(_in.m_y)
				, m_z(0.0f)
			{}

			vector3(float _x, float _y, float _z)
			{
				m_x = _x;
				m_y = _y;
				m_z = _z;
			}

			void operator += (const vector3& _o)
			{
				m_x += _o.m_x;
				m_y += _o.m_y;
				m_z += _o.m_z;
			}

			void operator -= (const vector3& _o)
			{
				m_x -= _o.m_x;
				m_y -= _o.m_y;
				m_z -= _o.m_z;
			}

			void operator *= (float _m)
			{
				m_x *= _m;
				m_y *= _m;
				m_z *= _m;
			}
		};

		struct matrix3
		{
			typedef float Array[3][3];

			Array m;

			void mul(const vector2& _in, vector2& _out)
			{
				_out.m_x = m[0][0] * _in.m_x + m[0][1] * _in.m_y + m[0][2];
				_out.m_y = m[1][0] * _in.m_x + m[1][1] * _in.m_y + m[1][2];
			}

			void mul(const matrix3& _in)
			{
				const Array& a = m;
				const Array& b = _in.m;

				Array t;

				t[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
				t[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
				t[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];

				t[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
				t[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
				t[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];

				t[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];
				t[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];
				t[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];

				memcpy(m, t, sizeof(t));
			}

			void setIndentity()
			{
				m[0][0] = m[1][1] = m[2][2] = 1.0f;
				m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0.0f;
			}

			void getTranslation(vector2& _pos) const
			{
				_pos.m_x = m[0][2];
				_pos.m_y = m[1][2];
			}

			void setTranslation(float _x, float _y)
			{
				m[0][2] = _x;
				m[1][2] = _y;
			}
		};
	}
}
