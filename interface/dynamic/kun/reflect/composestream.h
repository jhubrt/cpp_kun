// The MIT License (MIT)
//
// Copyright(c) 2016 Jerome Hubert
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

#include "interface\dynamic\kun\reflect\stream.h"

namespace kun
{
	namespace reflect
	{
		class CompositeStream : public Stream
		{
			std::vector<Stream*> m_streams;
		
		public:
			virtual bool Open (const char* _tag) override { bool result = true; for (auto s : m_streams){ result &= s->Open();  } return result; }
			virtual bool Close(const char* _tag) override { bool result = true; for (auto s : m_streams){ result &= s->Close(); } return result; }

			virtual void StartArray() override {}
			virtual void EndArray  () override {}

			virtual bool Write(const u64*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const s64*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const u32*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const s32*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const u16*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const s16*    _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const u8*     _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const s8*     _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const bool*   _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const float*  _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }
			virtual bool Write(const double* _v, size_t _num) override { bool result = true; for (auto s : m_streams){ result &= s->Write(_v, _num); } return result; }

			virtual bool WriteString(const char* _string)	  override { bool result = true; for (auto s : m_streams){ result &= s->WriteString(_string); } return result; }

			virtual bool ReadSection(Section& _section)					  override { assert(0); return false; }
			virtual bool ReadSection(Section& _section, u64& _sectionEnd) override { assert(0); return false; }

			virtual bool Read(u64*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(s64*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(u32*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(s32*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(u16*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(s16*    _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(u8*     _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(s8*     _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(bool*   _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(float*  _v, size_t _num) override { assert(0); return false; }
			virtual bool Read(double* _v, size_t _num) override { assert(0); return false; }

			virtual bool ReadString(const char* _string, size_t _maxsize) override { assert(0); return false; }

			File* GetFile() const { return m_streams[0]->GetFile(); }
		};

	} // reflect::
} // kun::
