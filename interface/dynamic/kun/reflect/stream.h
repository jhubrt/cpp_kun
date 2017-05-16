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

#include <algorithm>

#ifdef _DEBUG
#	include<map>
#endif

#include "interface\dynamic\kun\reflect\id.h"
#include "interface\dynamic\kun\reflect\file.h"


#define KREF_STRINGIZE(VAL)	VAL,#VAL

namespace kun
{
	namespace reflect
	{
#		ifdef _DEBUG
		struct DebugNamesCRC
		{
			std::map<NameCRC, std::string> m_map;

			void CheckName(NameCRC _crc, const char* _name)
			{
				auto it = m_map.find(_crc);

				if (it == m_map.end())
				{
					m_map.insert(std::make_pair(_crc,_name));
				}
				else
				{
					assert(strcmp(_name, it->second.c_str()) == 0);
				}
			}

			void DumpCRCNames(const char* _filename)
			{
				FILE* file = nullptr;
				fopen_s (&file, _filename, "wt");
				assert(file != nullptr);

				std::map<std::string,u32> map;

				fprintf(file, "nbkeys = %lu\n\n", (u32) m_map.size());

				for (auto& i : m_map)
				{
					map.insert(std::make_pair(i.second, i.first));
				}

				for (auto& i : map)
				{
					fprintf(file, "0x%08lx %s\n", i.second, i.first.c_str());
				}

				fprintf(file, "\n\n");

				for (auto& i : m_map)
				{
					fprintf(file, "0x%08lx %s\n", i.first, i.second.c_str());
				}

				fclose(file);
			}

			static DebugNamesCRC ms_instance;
		};
#		endif

		class Stream
		{
		protected:
			Stream(File& _file) : m_file(_file) {}

		public:

			struct Section
			{
				u32     m_size;         
				u32     m_nbChild;
			};

			File& m_file;

			virtual ~Stream () {}

			virtual Stream* CreateTracer(File* _outfile, File* _infile, u32 _checkflags) { return nullptr; }
			Stream* CreateTracer(File* _outfile) { return CreateTracer(_outfile, nullptr, 0); }

			virtual bool Open (const char* _tag = nullptr) = 0;
			virtual bool Close(const char* _tag = nullptr) = 0;
			
			virtual void StartArray(NameCRC _typename, const char* _tag = nullptr) = 0;
			virtual void EndArray  (const char* _tag = nullptr) = 0;

			virtual bool Write(const u64*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const s64*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const u32*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const s32*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const u16*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const s16*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const u8*     _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const s8*     _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const bool*   _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const float*  _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Write(const double* _v, size_t _num, const char* _tag = nullptr) = 0;

			virtual bool WriteString(const char* _string, u32 _len, const char* _tag = nullptr) = 0;
					bool WriteString(const char* _string, const char* _tag = nullptr) { return WriteString(_string, u32(strlen(_string) + 1), _tag); }

			virtual bool WriteCRC(const u32& _v, const char* _sourcestring, const char* _tag = nullptr)	{ return Write(_v, _tag); } //DebugNamesCRC::ms_instance.CheckName(_v, _sourcestring);
			virtual bool WriteCRC(const u64& _v, const char* _sourcestring, const char* _tag = nullptr) { return Write(_v, _tag); }

			virtual bool ReadSection(Section& _section, const char* _tag = nullptr) = 0;
			virtual bool ReadSection(Section& _section, u64& _sectionEnd, const char* _tag = nullptr) = 0;

			virtual bool Read(u64*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(s64*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(u32*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(s32*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(u16*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(s16*    _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(u8*     _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(s8*     _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(bool*   _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(float*  _v, size_t _num, const char* _tag = nullptr) = 0;
			virtual bool Read(double* _v, size_t _num, const char* _tag = nullptr) = 0;

			virtual bool ReadStringLength (u32& _length, const char* _tag = nullptr) = 0;
			virtual bool ReadString		  (char* _string, u32 _maxsize, const char* _tag = nullptr) = 0;

			File* GetFile() const { return &m_file; }

			template<class I> bool Read (I& _v, const char* _tag = nullptr)       { return Read (&_v, 1, _tag); } 
			template<class I> bool Write(const I& _v, const char* _tag = nullptr) { return Write(&_v, 1, _tag); } 
		};

	} // reflect::
} // kun::
