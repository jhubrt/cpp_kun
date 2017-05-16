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

#include "interface\dynamic\kun\reflect\stream.h"

namespace kun
{
	namespace reflect
	{
		class BinStreamImpl : public Stream
		{
			friend class TracerBinStream;

		protected:
			struct SectionStack
			{
				u64  m_offset;
				u32  m_nbChild;
			};

			kstd::vector<SectionStack> m_sections;

			BinStreamImpl(File& _file) 
				: Stream(_file)
			{
				m_sections.reserve(32);
			}
		};
		
		template<class T> 
		class BinStream : public BinStreamImpl
		{
		public:
			
			T m_file;

			template<class... Args> BinStream(Args... args) 
				: BinStreamImpl(m_file)
				, m_file(args...) 
			{
				m_sections.reserve(32);
			}

			virtual Stream* CreateTracer(File* _outfile, File* _infile, u32 _checkflags) override;
			Stream* CreateTracer(File* _outfile) { return CreateTracer(_outfile, nullptr, 0); }

			virtual bool Open (const char* _tag = nullptr) override	
			{ 
				SectionStack section;

				section.m_offset  = m_file.T::GetPos();
				section.m_nbChild = 0;

				m_sections.push_back (section);

				return Write(u64(0));  // space for size + child
			}

			virtual bool Close (const char* _tag = nullptr) override
			{ 
				auto& section = m_sections.back();

				u64 size = m_file.GetPos() - section.m_offset;
				assert( size < 0x100000000ULL ); // size should not exceed 4gb

				auto bakPos = m_file.GetPos();

				m_file.T::SeekSet(section.m_offset);

				bool success;

				success  = Write(u32(size));
				success &= Write(section.m_nbChild);
				success &= m_file.T::SeekSet(bakPos);

				m_sections.pop_back();

				if ( m_sections.size() > 0 )
				{
					m_sections.back().m_nbChild++;
				}

				return success;
			}

			virtual void StartArray	(NameCRC _typename, const char* _tag = nullptr) override	{}
			virtual void EndArray	(const char* _tag = nullptr)					override	{}

			virtual bool Write(const u64*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const s64*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const u32*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const s32*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const u16*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const s16*  _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const u8*   _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const s8*   _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const bool* _v, size_t _num, const char* _tag = nullptr)		override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const float*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Write(_v, sizeof(*_v) *_num); }
			virtual bool Write(const double* _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Write(_v, sizeof(*_v) *_num); }

			virtual bool WriteString(const char* _string, u32 _length, const char* _tag = nullptr) override
			{
				bool success = Write(_length, _tag);
				success		&= Write((const u8*)_string, _length, _tag);

				return success;
			}

			virtual bool Read(u64*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(s64*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(u32*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(s32*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(u16*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(s16*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(u8*   _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(s8*   _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(bool* _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(float*  _v, size_t _num, const char* _tag = nullptr)	override { return m_file.T::Read(_v, sizeof(*_v) *_num); }
			virtual bool Read(double* _v, size_t _num, const char* _tag = nullptr)  override { return m_file.T::Read(_v, sizeof(*_v) *_num); }

			virtual bool ReadSection(Section& _section, const char* _tag = nullptr) override
			{
				bool success;

				success  = Read(_section.m_size,	_tag);
				success &= Read(_section.m_nbChild,	_tag);

				return true;
			}

			virtual bool ReadSection(Section& _section, u64& _sectionEnd, const char* _tag = nullptr) override 
			{ 
				bool success = ReadSection(_section, _tag);

				_sectionEnd = m_file.T::GetPos() + _section.m_size;

				return success;
			}

			virtual bool ReadStringLength(u32& _length, const char* _tag = nullptr) override
			{
				return Read(_length, _tag);
			}

			virtual bool ReadString(char* _string, u32 _length, const char* _tag = nullptr) override
			{
				// static_cast<Stream*>(this)->
				return Read((u8*)_string, _length, _tag);
			}

			template<class I> bool Read (I& _v, const char* _tag = nullptr)       { return BinStream::Read (&_v, 1, _tag); } 
			template<class I> bool Write(const I& _v, const char* _tag = nullptr) { return BinStream::Write(&_v, 1, _tag); } 
		};


		typedef BinStream<StdFile>	BinStreamFile;
		typedef BinStream<MemFile>	BinStreamMemFile;

		class TracerBinStream : public Stream
		{
		public:
			enum class OpType
			{
				Op_u64,
				Op_s64,
				Op_u32,
				Op_s32,
				Op_u16,
				Op_s16,
				Op_u8,
				Op_s8,
				Op_bool,
				Op_float,
				Op_double,
				Op_string,
				Op_section,
				Op_undefined,
				NBOPTYPES
			};

			struct Check
			{
				enum Enum
				{
					OFFSET		= 1,
					ORDER		= 2,
					SIZE		= 4,
					TAG			= 8,
					TYPE		= 16,
					SIGNATURES	= 32,
					DEFAULT		= OFFSET | SIZE | TYPE | SIGNATURES,
					STRONG      = DEFAULT | TAG,
					ALL			= STRONG | ORDER
				};
			};

		private:
			Stream& m_stream;
			File*	m_outTracefile;
			File*	m_inTracefile;

			struct OutOp
			{
				u32			m_stringindex;
				size_t		m_offset;

				union 
				{
					u32		m_myindex;
					u32		m_order;
				};

				u32			m_size;

				union
				{
					u8		m_signature[15];
					u16		m_signature_16;
					u32		m_signature_32;
					u64		m_signature_64;
					float   m_signature_float;
					double  m_signature_double;
				};
				u8			m_op;

				u32 GetNbSignatures() const { return (u32) std::min<size_t>(m_size, sizeof(m_signature)); }
			};

			struct OutOpMap
			{
				kstd::vector<OutOp>		   m_ops;
				kstd::vector<u32>		   m_map;
				kstd::vector<char>		   m_stringspool;
				u32						   m_count;

				static const char* const ms_sectionname[];

				OutOpMap() : m_count(0) {}

				void AddOp(OpType _optype, const void* _adr, size_t _nbbytes, const char* _tag, u32 _order, size_t _pos)
				{
					OutOp op;

					op.m_op			 = static_cast<u8>(_optype);
					op.m_myindex     = (u32) m_ops.size();
					op.m_size	     = (u32) _nbbytes;

					if (_tag == nullptr)
					{
						_tag = "<null>";
					}

					op.m_stringindex = PushTag (_tag);

					size_t newsize = _pos + _nbbytes;

					if (newsize > m_map.size())
					{
						m_map.resize(newsize, 0xFFFFFFFF);
					}

					const u8* p = static_cast<const u8*>(_adr);

					for (size_t t = 0 ; t < _nbbytes ; t++, p++)
					{
						m_map[t + _pos] = op.m_myindex;

						if (t < sizeof(op.m_signature))
						{
							op.m_signature[t] = *p;
						}
					}

					m_ops.push_back(op);
				}

				const char* GetTag(const OutOp&  _op) const 
				{ 
					return &m_stringspool[_op.m_stringindex]; 
				}

				u32 PushTag(const char* _tag)
				{
					u32 stringindex = (u32) m_stringspool.size();
					size_t len = strlen(_tag);

					m_stringspool.resize(stringindex + len + 1);
					strcpy (&m_stringspool[stringindex], _tag);

					return stringindex;
				}

				u32 SaveSignature (char*& l, u32 opindex, size_t& t)
				{
					char temp[256];
					auto& op = m_ops[opindex];
					u32 nbbytes = 0;


					while (t < m_map.size())
					{
						if (opindex != m_map[t])
						{
							t--;
							break;
						}

						if (nbbytes < sizeof(op.m_signature))
						{
							u8 b = op.m_signature[nbbytes];

							l += sprintf(l, "%02x ", b);

							if ((b >= 32) && (b <= 255))
							{
								temp[nbbytes] = b;
							}
							else
							{
								temp[nbbytes] = '.';
							}
						}

						t++;
						nbbytes++;
					}

					for (u32 j = nbbytes; j < sizeof(op.m_signature); j++)
					{
						l += sprintf(l, "   ");
						temp[j] = ' ';
					}

					temp[sizeof(op.m_signature)] = 0;

					l += sprintf(l, "%s", temp);

					return nbbytes + 1;
				}

				void SaveTrace (File& _file)
				{
					char line[1024];

					//               0          33               4         SECTION   A                                        71 00 00 00                                  q...           113
					sprintf_s(line, "; Offset   Order            Size      Type      Description                              Bytes (15 first)                             Ascii          Value\n;\n");
					_file.Write(line, strlen(line));

					for (size_t t = 0; t < m_map.size(); t++)
					{
						bool truncated = false;
						u32 opindex = m_map[t];

						if (opindex != 0xFFFFFFFF)
						{
							auto& op = m_ops[opindex];
							char* l = line;

							if (opindex != op.m_myindex)
							{
								l += sprintf(l, "<TRUNCATED... %lu bytes\n", opindex - op.m_myindex);
								truncated = true;
							}

							l += sprintf(l, "%-10llu %-8lu         %-9lu %-9s %-40s ", t, op.m_myindex, op.m_size, ms_sectionname[op.m_op], GetTag(op));

							const u32 nbbytes = SaveSignature(l, opindex, t);

							if (nbbytes < op.m_size)
							{
								l += sprintf(l, "\n...TRUNCATED %lu bytes>", op.m_size - nbbytes);
								truncated = true;
							}

							if (truncated == false)
							{
								switch (static_cast<OpType>(op.m_op))
								{
								case OpType::Op_bool:
									l += sprintf(l, "%s", op.m_signature[0] ? "true" : "false");
									break;
								case OpType::Op_float:
									l += sprintf(l, "%f", op.m_signature_float);
									break;
								case OpType::Op_double:
									l += sprintf(l, "%f", op.m_signature_double);
									break;
								case OpType::Op_u64:
									l += sprintf(l, "%llu", op.m_signature_64);
									break;
								case OpType::Op_s64:
									l += sprintf(l, "%lld", (s64)op.m_signature_64);
									break;
								case OpType::Op_section:
								case OpType::Op_u32:
									l += sprintf(l, "%lu", op.m_signature_32);
									break;
								case OpType::Op_s32:
									l += sprintf(l, "%ld", (s32)op.m_signature_32);
									break;
								case OpType::Op_u16:
									l += sprintf(l, "%u", op.m_signature_16);
									break;
								case OpType::Op_s16:
									l += sprintf(l, "%d", (s16)op.m_signature_16);
									break;
								case OpType::Op_u8:
									l += sprintf(l, "%u", op.m_signature[0]);
									break;
								case OpType::Op_s8:
									l += sprintf(l, "%d", (s8)op.m_signature[0]);
									break;
								case OpType::Op_string:
									l += sprintf(l, "%lu", op.m_signature_32);
									l += sprintf(l, "%11s", &op.m_signature[4]);
									break;
								default:
									assert(0);
								}
							}

							l += sprintf(l, "\n");

							_file.Write(line, strlen(line));
						}
					}
				}

				OpType GetOpType(const char* _string) const
				{
					auto it = std::find_if(ms_sectionname, &ms_sectionname[(u32)OpType::NBOPTYPES], [_string](const char* const _o){ return strcmp(_o, _string) == 0; } );

					if (it == &ms_sectionname[(u32)OpType::NBOPTYPES])
					{
						assert(0);
						return OpType::Op_undefined;
					}
					else
					{
						return OpType(it - ms_sectionname);
					}
				}

				void LoadTrace(File& _file)
				{				
					const auto size = _file.Size();

					if (size > 0)
					{
						m_ops.reserve(16384);

						char* buffer = new char[size + 1];

						bool result = _file.Read(buffer, size);
						assert(result);

						buffer[size] = 0;

						char* p = buffer;

						while(*p != 0)
						{
							OutOp op;
							char* pnext = strchr(p, '\n');

							*pnext++ = 0;

							if (p[0] != ';')
							{
								p = strtok(p, " \t\r\n"); assert(p != nullptr);
								assert(strcmp(p, "<TRUNCATED...") != 0);
								assert(strcmp(p, "...TRUNCATED") != 0);
								op.m_offset = atoll(p);

								p = strtok(nullptr, " \t\r\n"); assert(p != nullptr);
								op.m_order = atoi(p);

								p = strtok(nullptr, " \t\r\n"); assert(p != nullptr);
								op.m_size = atoi(p);

								p = strtok(nullptr, " \t\r\n"); assert(p != nullptr);
								op.m_op = (u8)GetOpType(p);

								p = strtok(nullptr, " \t\r\n"); assert(p != nullptr);
								op.m_stringindex = PushTag(p);

								for (u32 t = 0, nb = op.GetNbSignatures(); t < nb; t++)
								{
									p = strtok(nullptr, " \t\r\n"); assert(p != nullptr);
									op.m_signature[t] = (u8)strtol(p, nullptr, 16);
								}

								m_ops.push_back(op);
							}

							p = pnext;
						}
					}
				}

				void CheckOp(u32 _checkflags, OpType _optype, const void* _adr, size_t _nbbytes, const char* _tag, u32 _order, size_t _pos)
				{
					const OutOp& op = m_ops[_order];	// TODO => something more intelligent (to resynchronize when needed...)
					const char* s = nullptr;

					if (_checkflags & Check::TAG)		  
					{	
						s = GetTag(op); 
						if (strcmp(s, _tag) != 0) 
							assert(_tag[0] == '?'); 
					}

					if (_checkflags & Check::OFFSET)		
						assert(op.m_offset == _pos);
					
					if (_checkflags & Check::SIZE)			
						assert(op.m_size == _nbbytes);

					if (_checkflags & Check::TYPE)			
						assert(op.m_op == (u8)_optype);

					if (_checkflags & Check::ORDER)			
						assert(op.m_order == _order);

					if (_checkflags & Check::SIGNATURES)	
						assert(memcmp(op.m_signature, _adr, op.GetNbSignatures()) == 0);
				}
			};
			
			OutOpMap	m_current;
			OutOpMap	m_old;
			u32			m_checkflags;

		public:		
			TracerBinStream(Stream& _stream, File* _outTracefile, File* _inTracefile, u32 _checkflags) 
				: Stream(_stream.m_file)
				, m_stream(_stream) 
				, m_outTracefile(_outTracefile)
				, m_inTracefile(_inTracefile)
				, m_checkflags(_checkflags)
			{
				if (_inTracefile != nullptr)
				{
					m_old.LoadTrace(*_inTracefile);
				}
			}

			virtual ~TracerBinStream()
			{
				if (m_outTracefile != nullptr)
				{
					m_current.SaveTrace(*m_outTracefile);
				}
				
				if (m_inTracefile != nullptr)
				{
					assert(m_old.m_count == m_old.m_ops.size());
				}
			}

			virtual bool Open(const char* _tag = nullptr) override
			{
				u64 dummy = 0;

				m_current.AddOp(OpType::Op_section, &dummy, sizeof(dummy), _tag, m_current.m_count++, m_file.GetPos());
				return m_stream.Open(_tag);
			}

			virtual bool Close(const char* _tag = nullptr) override
			{	
				auto& section = static_cast<BinStreamImpl&>(m_stream).m_sections.back();

				u32 size = u32(m_stream.m_file.GetPos() - section.m_offset);

				m_current.AddOp(OpType::Op_section, &size, sizeof(size), _tag ? _tag : "size", m_current.m_count, section.m_offset);
				m_current.AddOp(OpType::Op_section, &section.m_nbChild, sizeof(section.m_nbChild), _tag ? _tag : "nbchild", m_current.m_count, section.m_offset + sizeof(size));
				m_current.m_count++;

				return m_stream.Close(_tag);
			}

			virtual void StartArray(NameCRC _typename, const char* _tag = nullptr) override {}
			virtual void EndArray  (const char* _tag = nullptr)					   override {}

#			define KREF_TRACER_STREAM_WRITE(OP)\
				m_current.AddOp(OP, _v, sizeof(*_v) * _num, _tag, m_current.m_count++, m_file.GetPos());\
				return m_stream.Write(_v, _num, _tag);

			virtual bool Write(const u64*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_u64)   }
			virtual bool Write(const s64*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_s64)   }
			virtual bool Write(const u32*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_u32)   }
			virtual bool Write(const s32*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_s32)   }
			virtual bool Write(const u16*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_u16)   }
			virtual bool Write(const s16*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_s16)   }
			virtual bool Write(const u8*     _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_u8)    }
			virtual bool Write(const s8*     _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_s8)    }
			virtual bool Write(const bool*   _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_bool)  }
			virtual bool Write(const float*  _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_float) }
			virtual bool Write(const double* _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_WRITE(OpType::Op_double)}

			virtual bool WriteString(const char* _string, u32 _length, const char* _tag = nullptr) override
			{
				auto pos = m_file.GetPos();
			
				m_current.AddOp(OpType::Op_u32	  , &_length, sizeof(_length), _tag, m_current.m_count++, pos);
				m_current.AddOp(OpType::Op_string, _string, _length		 , _tag, m_current.m_count++, pos + sizeof(_length));

				return m_stream.WriteString(_string, _tag);
			}

#			define KREF_TRACER_STREAM_READ(OP)\
				auto pos = m_file.GetPos();\
				auto success = m_stream.Read(_v, _num, _tag);\
				m_old.CheckOp(m_checkflags, OP, _v, sizeof(*_v) * _num, _tag, m_old.m_count++, pos);\
				return success;

			virtual bool Read(u64*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_u64)    }
			virtual bool Read(s64*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_s64)    }
			virtual bool Read(u32*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_u32)    }
			virtual bool Read(s32*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_s32)    }
			virtual bool Read(u16*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_u16)    }
			virtual bool Read(s16*    _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_s16)    }
			virtual bool Read(u8*     _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_u8)     }
			virtual bool Read(s8*     _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_s8)     }
			virtual bool Read(bool*   _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_bool)   }
			virtual bool Read(float*  _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_float)  }
			virtual bool Read(double* _v, size_t _num, const char* _tag = nullptr) override { KREF_TRACER_STREAM_READ(OpType::Op_double) }

			virtual bool ReadSection(Section& _section, const char* _tag = nullptr) override 
			{ 
				auto pos = m_file.GetPos();
				auto success = m_stream.ReadSection(_section, _tag);

				m_old.CheckOp(m_checkflags, OpType::Op_section, &_section.m_size	, sizeof(_section.m_size)	, _tag ? _tag : "?size"	  , m_old.m_count++, pos);
				m_old.CheckOp(m_checkflags, OpType::Op_section, &_section.m_nbChild, sizeof(_section.m_nbChild), _tag ? _tag : "?nbchild", m_old.m_count++, pos + sizeof(_section.m_size));

				return success; 
			}

			virtual bool ReadSection(Section& _section, u64& _sectionEnd, const char* _tag = nullptr) override 
			{ 
				auto success = ReadSection(_section, _tag);			

				_sectionEnd = _section.m_size + m_file.GetPos();

				return success; 
			}

			virtual bool ReadStringLength (u32& _length, const char* _tag = nullptr) override
			{
				auto pos = m_file.GetPos();

				bool success = m_stream.Read(_length);	
				m_old.CheckOp(m_checkflags, OpType::Op_u32, &_length, sizeof(_length), _tag, m_old.m_count++, pos);
				
				return success;
			}

			virtual bool ReadString(char* _string, u32 _length, const char* _tag = nullptr) override 
			{ 
				auto pos = m_file.GetPos();

				bool success = m_stream.Read((u8*)_string, _length);	
				m_old.CheckOp(m_checkflags, OpType::Op_string, _string, _length, _tag, m_old.m_count++, pos);

				return success;
			}

			File* GetFile() const { return &m_file; }

			template<class I> bool Read (I& _v, const char* _tag = nullptr)       { return Read (&_v, 1, _tag); } 
			template<class I> bool Write(const I& _v, const char* _tag = nullptr) { return Write(&_v, 1, _tag); } 
		};

		template<class T> inline Stream* BinStream<T>::CreateTracer(File* _outfile, File* _infile, u32 _checkflags) 
		{
			return new TracerBinStream(*this, _outfile, _infile, _checkflags);
		}

	} // reflect::
} // kun::
