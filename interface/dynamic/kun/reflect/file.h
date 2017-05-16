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
	namespace reflect
	{
		struct File
		{
			virtual ~File() {}

			virtual bool   IsValid() const = 0;
			virtual size_t GetPos () const = 0;
			virtual size_t Size	  ()	   = 0;

			virtual bool Write	(const void* _adr, size_t _size) = 0;
			virtual bool Read	(void* _adr, size_t _size)		 = 0;

			virtual bool SeekSet(size_t _pos)		= 0;
			virtual bool SeekRel(ptrdiff_t _offset) = 0; 
		};

		class StdFile : public File
		{
			FILE* m_file;

		public:
			
			enum Mode
			{
				READ,
				WRITE,
				APPEND
			};

			StdFile(const char* _filename, Mode _mode)
			{
				const char* mode = nullptr;

				switch (_mode)
				{
				case READ:	 mode = "rb"; break;
				case WRITE:  mode = "wb"; break;
				case APPEND: mode = "a+"; break;
				default: assert (0);
				}

				fopen_s (&m_file, _filename, mode);
			}

			~StdFile()
			{
				if (m_file != nullptr)
				{
					fclose(m_file);
					m_file = nullptr;
				}
			}

			bool IsValid() const { return m_file != nullptr; }

			bool Write	(const void* _adr, size_t _size)	{ return fwrite(_adr, _size, 1, m_file) == 1; }
			bool Read	(void* _adr, size_t _size)			{ return fread(_adr, _size, 1, m_file) == 1; }

			template <class I> bool Write(const I& _o)	{ return fwrite(&_o, sizeof(I), 1, m_file) == 1; }
			template <class I> bool Read(I& _o)			{ return fread(&_o, sizeof(I), 1, m_file) == 1; }

			size_t GetPos() const { return ftell(m_file); }

			bool SeekSet(size_t _pos)		{ return fseek (m_file, (u32) _pos, SEEK_SET) == 0; }
			bool SeekRel(ptrdiff_t _offset) { return fseek (m_file, (s32) _offset, SEEK_CUR) == 0; }

			size_t Size()
			{
				auto bak = ftell(m_file);
				
				fseek (m_file, 0, SEEK_END);
				auto size = ftell(m_file);
				fseek (m_file, bak, SEEK_SET);

				return size;
			}
		};


		class InPlaceFile : public File
		{
			u8*		m_buffer;
			size_t  m_buffersize;
			size_t  m_size;
			size_t	m_p;

		public:

			InPlaceFile(u8* _buffer, size_t _buffersize, size_t _size = 0)
				: m_buffer		(_buffer)
				, m_buffersize  (_buffersize)
				, m_size		(_size)
				, m_p			(0)
			{}

			bool IsValid() const { return true; }

			bool Write(const void* _adr, size_t _size)
			{
				if ((m_p + _size) <= m_size)
				{
					memcpy (&m_buffer[m_p], _adr, _size);
					m_p += _size;
					m_size = std::max(m_size, m_p);
					assert(m_size <= m_buffersize);

					return true;
				}
				else
				{
					return false;
				}
			}

			bool Read(void* _adr, size_t _size)
			{
				if ((m_p + _size) <= m_size)
				{
					memcpy (_adr, &m_buffer[m_p], _size);
					m_p += _size;
					return true;
				}
				else
				{
					return false;
				}
			}

			size_t Size	() { return m_size; }

			bool SeekSet(size_t _pos)		
			{  
				if (_pos <= m_size)
				{
					m_p = _pos;
					return true;
				}
				else
				{
					return false;
				}
			}

			bool SeekRel(ptrdiff_t _offset) 
			{  
				size_t newpos = m_p + _offset;

				if (newpos <= m_size)
				{
					m_p = newpos;
					return true;
				}
				else
				{
					return false;
				}
			}

			size_t GetPos () const { return m_p; }
		};


		class MemFile : public File
		{
			std::vector<u8>	m_bytes;
			size_t			m_p;

		public:

			MemFile(size_t _size = 2048)
				: m_p (0)
			{
				m_bytes.reserve(_size);
			}

			bool IsValid() const { return true; }

			bool Write(const void* _adr, size_t _size)
			{
				auto currentsize = m_bytes.size();
				auto newsize     = m_p + _size;

				if (newsize > currentsize)
				{
					m_bytes.resize(newsize);
				}

				memcpy(&m_bytes[m_p], _adr, _size);
				m_p += _size;
				return true;
			}

			bool Read(void* _adr, size_t _size)
			{
				auto currentsize = m_bytes.size();

				if ((m_p + _size) <= currentsize)
				{
					memcpy (_adr, &m_bytes[m_p], _size);
					m_p += _size;
					return true;
				}
				else
				{
					return false;
				}
			}

			size_t Size	() { return m_bytes.size(); }

			bool SeekSet(size_t _pos)		
			{  
				if (_pos <= m_bytes.size())
				{
					m_p = _pos;
					return true;
				}
				else
				{
					return false;
				}
			}

			bool SeekRel(ptrdiff_t _offset) 
			{  
				size_t newpos = m_p + _offset;

				if (newpos <= m_bytes.size())
				{
					m_p = newpos;
					return true;
				}
				else
				{
					return false;
				}
			}

			size_t GetPos () const { return m_p; }
		};


	} // reflect::
} // kun::
