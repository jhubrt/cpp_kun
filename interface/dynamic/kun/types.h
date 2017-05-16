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

namespace std {} // clang needs this to make an alias...

namespace kun
{
	typedef unsigned char	   u8;
	typedef	signed char		   s8;
	typedef unsigned short     u16;
	typedef	signed short	   s16;
	typedef unsigned long	   u32;
	typedef	signed long		   s32;
	typedef unsigned long long u64;
	typedef signed long long   s64;

	typedef size_t ModuleID;

	template<class O, class I> O safe_cast(I _type)
	{
		assert(dynamic_cast<O>(_type) != nullptr);
		return static_cast<O>(_type);
	}

	struct FalseType {};
	struct TrueType {};

	template <bool> struct Test
	{
		typedef TrueType type;
	};

	template <> struct Test<false>
	{
		typedef FalseType type;
	};

	struct Log
	{
		typedef void(*LoggerCall)(const char* const _message);
		static LoggerCall m_func;

		static void log (const char* const _message) { m_func(_message); }
	};

	namespace kstd = ::std;

#	define KUN_ARRAYSIZE(A)	(sizeof(A)/sizeof(*(A)))
}
