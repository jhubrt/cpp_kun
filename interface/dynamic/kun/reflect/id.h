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

#include "interface\dynamic\kun\types.h"

namespace kun
{
	namespace reflect
	{
		template <class ...Params> void log(const char* _format, Params...args)
		{
			printf(_format, args...);
		}

		typedef u32 NameCRC;
		typedef u32 TypeCRC;

		constexpr u64 StringCRC64_ROL(u64 _val)
		{
			return (_val ^ (_val >> 32)) & 0xFFFFFFFF;
		}

		constexpr u64 StringCRC64(const char* _name)
		{
			return *_name != 0 ? StringCRC64_ROL( (61 * StringCRC64(_name + 1)) ^ (*_name - ' ') ) : 0;
		}

		constexpr u64 StringCRC64(const char* _name, size_t _size, size_t _index)
		{
			return _index < _size ? StringCRC64_ROL( (61 * StringCRC64(_name, _size, _index + 1)) ^ (_name[_index] - ' ') ) : 0;
		}

		constexpr u32 StringCRC(const char* _name)
		{
			return static_cast<u32>(StringCRC64(_name));
		}

		constexpr u32 StringCRC(const char* _name, size_t _size)
		{
			return static_cast<u32>(StringCRC64(_name, _size, 0));
		}

	} // refl::
} //kun::


