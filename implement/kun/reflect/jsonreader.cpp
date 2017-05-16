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

#include <list>

#include "implement\kun\reflect\jsonreader.h"

#include "implement\extern\rapidjson\include\rapidjson\reader.h"
#include "implement\extern\rapidjson\include\rapidjson\stream.h"

#include "interface\dynamic\kun\reflect\binstream.h"

namespace kun
{
	namespace reflect
	{
#		ifdef _DEBUG
		DebugNamesCRC DebugNamesCRC::ms_instance;
#		endif

		const char JSON_TYPESEPARATOR = ' ';

#		define KREF_TRACE_JSONREAD

		// TEMPORARY: put instanciation there...
		const char* const TracerBinStream::OutOpMap::ms_sectionname[] = { "U64", "S64", "U32", "S32", "U16", "S16", "U8", "S8", "BOOL", "FLOAT", "DOUBLE", "STRING", "SECTION", "UNDEFINED" };

		struct JsonHandler 
		{	
			Stream&					 m_stream;
			std::list<std::string>   m_stringpool;
			char*					 m_key;
			TracerBinStream::OpType	 m_keyType;
			bool					 m_keyIsCRC;
			u64						 m_CRCvalue;
			std::vector<const char*> m_sectionname;
			int						 m_depth;
			int						 m_lastArrayDepth;

			std::vector<u8>			 m_arrayU8;
			std::vector<u16>		 m_arrayU16;
			std::vector<u32>		 m_arrayU32;
			std::vector<u64>		 m_arrayU64;
			std::vector<float>		 m_arrayFloat;
			std::vector<double>		 m_arrayDouble;

#			ifdef KREF_TRACE_JSONREAD
			void indent()	{ printf("%*c", m_depth * 4, ' '); }
#			endif


			JsonHandler(Stream& _stream) 
				: m_stream	(_stream)
				, m_key(nullptr)
				, m_depth(0)
				, m_lastArrayDepth(-1)
			{}

			void WriteValue(u64 _i, double _f)
			{
				if (m_keyIsCRC)
				{
					assert(m_keyType == TracerBinStream::OpType::Op_u32);
					_i = (u32) m_CRCvalue;
				}

/*				if (m_lastArrayDepth == m_depth)
				{
					switch (m_keyType)
					{
					case TracerBinStream::OpType::Op_bool:      
						m_arrayU8.push_back(_i != 0);						
						break;
					
					case TracerBinStream::OpType::Op_s8:		
					case TracerBinStream::OpType::Op_u8:		
						m_arrayU8.push_back(static_cast<u8>(_i));			
						break;
					
					case TracerBinStream::OpType::Op_s16:		
					case TracerBinStream::OpType::Op_u16:		
						m_arrayU16.push_back(static_cast<u16>(_i));			
						break;

					case TracerBinStream::OpType::Op_s32:
					case TracerBinStream::OpType::Op_section:
					case TracerBinStream::OpType::Op_u32:		
						m_arrayU32.push_back(static_cast<u32>(_i));			
						break;

					case TracerBinStream::OpType::Op_s64:		
					case TracerBinStream::OpType::Op_u64:		
						m_arrayU64.push_back(static_cast<u64>(_i));			
						break;

					case TracerBinStream::OpType::Op_float:		
						m_arrayFloat.push_back(static_cast<float>(_f));		
						break;

					case TracerBinStream::OpType::Op_double:	
						m_arrayDouble.push_back(_f);		
						break;
					}
				}
				else*/
				{
					switch (m_keyType)
					{
					case TracerBinStream::OpType::Op_bool:      m_stream.Write(_i != 0, m_key);						break;
					case TracerBinStream::OpType::Op_s8:		m_stream.Write(static_cast<s8>(_i), m_key);			break;
					case TracerBinStream::OpType::Op_u8:		m_stream.Write(static_cast<u8>(_i), m_key);			break;
					case TracerBinStream::OpType::Op_s16:		m_stream.Write(static_cast<s16>(_i), m_key);		break;
					case TracerBinStream::OpType::Op_u16:		m_stream.Write(static_cast<u16>(_i), m_key);		break;
					case TracerBinStream::OpType::Op_s32:		m_stream.Write(static_cast<s32>(_i), m_key);		break;
					case TracerBinStream::OpType::Op_section:
					case TracerBinStream::OpType::Op_u32:		m_stream.Write(static_cast<u32>(_i), m_key);		break;
					case TracerBinStream::OpType::Op_s64:		m_stream.Write(static_cast<s64>(_i), m_key);		break;
					case TracerBinStream::OpType::Op_u64:		m_stream.Write(_i, m_key);		break;
					case TracerBinStream::OpType::Op_float:		m_stream.Write(static_cast<float>(_f), m_key);		break;
					case TracerBinStream::OpType::Op_double:	m_stream.Write(_f, m_key);		break;
					default:
						assert(0);
					}
				}
			}

			bool Null() 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf("Null()\n"); 
#				endif

				return true; 
			}

			bool Bool(bool v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Bool(%s)\n", v ? "true" : "false"); 
#				endif

				WriteValue(v, v);

				return true; 
			}

			bool Int(int v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Int(%d)\n", v); 
#				endif

				WriteValue(v, v);

				return true; 
			}

			bool Uint(unsigned v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Uint(%u)\n", v); 
#				endif

				WriteValue(v, v);

				return true; 
			}

			bool Int64(int64_t v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf("Int64(%lld)\n", v); 
#				endif

				WriteValue(v, (double)v);

				return true; 
			}

			bool Uint64(uint64_t v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Uint64(%llu)\n", v);
#				endif

				WriteValue(v, (double)v);

				return true; 
			}

			bool Double(double v) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Double(%f)", v); 
#				endif

				WriteValue(static_cast<s64>(v), v);

				return true; 
			}

			bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Number(%s, %u, copy=%s)\n", str, length, copy ? "true" : "false");
#				endif

				assert(m_keyType == TracerBinStream::OpType::Op_string);
				m_stream.WriteString(str, m_key);

				return true;
			}

			bool String(const char* str, rapidjson::SizeType length, bool copy) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("String(%s, %u, copy=%s)\n", str, length, copy ? "true" : "false");
#				endif

				if (m_keyIsCRC)
				{
					WriteValue(m_CRCvalue, 0.0f);
				}
				else
				{
					assert(m_keyType == TracerBinStream::OpType::Op_string);
					m_stream.WriteString(str, m_key);
				}

				return true;
			}

			bool StartObject() 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("StartObject()\n"); 
#				endif

				m_depth++;

				m_sectionname.push_back(m_key);

				if (m_sectionname.size() > 1)
				{
					m_stream.Open(m_key);
				}

				return true; 
			}

			bool Key(const char* str, rapidjson::SizeType length, bool copy) 
			{
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("Key(%s, %u, copy=%s)\n", str, length, copy ? "true" : "false");
#				endif

				m_stringpool.push_back(str);
				m_key = (char*) m_stringpool.back().c_str();
				
				char* p = strchr(m_key, JSON_TYPESEPARATOR);
				if (p == nullptr)
				{
					m_keyType = TracerBinStream::OpType::Op_undefined;
				}
				else
				{
					if		(memcmp("u64"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_u64;
					else if (memcmp("s64"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_s64;
					else if	(memcmp("u32"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_u32;
					else if (memcmp("s32"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_s32;
					else if (memcmp("u16"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_u16;
					else if (memcmp("s16"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_s16;
					else if (memcmp("u8"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_u8; 
					else if (memcmp("s8"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_s8; 
					else if (memcmp("bool"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_bool;
					else if (memcmp("float"	, m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_float;
					else if (memcmp("double", m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_double;
					else if (memcmp("string", m_key, p - m_key) == 0)	m_keyType = TracerBinStream::OpType::Op_string;
					else assert(0);
					
					char* start = p + 1;

					m_key = start;
					p = strchr(start, '#');
					if (p != nullptr)
					{
						assert (m_keyType == TracerBinStream::OpType::Op_u32);

						*p = 0;
						m_CRCvalue = StringCRC(p + 1);
						m_keyIsCRC = true;

#						ifdef _DEBUG
						DebugNamesCRC::ms_instance.CheckName((u32) m_CRCvalue, p + 1);
#						endif
					}
					else
					{
						m_keyIsCRC = false;
					}
				}

				return true;
			}

			bool EndObject(rapidjson::SizeType memberCount) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("EndObject(%u members)\n", memberCount); 
#				endif

				m_depth--;

				if (m_sectionname.size() > 1)
				{
					m_stream.Close(m_sectionname.back());
				}

				m_sectionname.pop_back();

				return true; 
			}

			bool StartArray() 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("StartArray()\n"); 
#				endif

				m_depth++;
				m_lastArrayDepth = m_depth;

				return true; 
			}

			bool EndArray(rapidjson::SizeType elementCount) 
			{ 
#				ifdef KREF_TRACE_JSONREAD
				indent();
				printf ("EndArray(%u elements)\n", elementCount); 
#				endif
				
/*				if ((m_arrayU8.size() > 0) || (m_arrayU16.size() > 0) || (m_arrayU32.size() > 0) || (m_arrayU64.size() > 0) || (m_arrayFloat.size() > 0) || (m_arrayDouble.size() > 0))
				{
					switch (m_keyType)
					{
					case TracerBinStream::OpType::Op_bool:      m_stream.Write(reinterpret_cast<bool*>(&m_arrayU8 [0]), m_arrayU8 .size(), m_key);		break;
					case TracerBinStream::OpType::Op_s8:		m_stream.Write(reinterpret_cast<s8*  >(&m_arrayU8 [0]), m_arrayU8 .size(), m_key);		break;
					case TracerBinStream::OpType::Op_u8:		m_stream.Write(reinterpret_cast<u8*  >(&m_arrayU8 [0]), m_arrayU8 .size(), m_key);		break;
					
					case TracerBinStream::OpType::Op_s16:		m_stream.Write(reinterpret_cast<s16* >(&m_arrayU16[0]), m_arrayU16.size(), m_key);		break;
					case TracerBinStream::OpType::Op_u16:		m_stream.Write(reinterpret_cast<u16* >(&m_arrayU16[0]), m_arrayU16.size(), m_key);		break;
					
					case TracerBinStream::OpType::Op_s32:		m_stream.Write(reinterpret_cast<s32* >(&m_arrayU32[0]), m_arrayU32.size(), m_key);		break;
					case TracerBinStream::OpType::Op_section:
					case TracerBinStream::OpType::Op_u32:		m_stream.Write(reinterpret_cast<u32* >(&m_arrayU32[0]), m_arrayU32.size(), m_key);		break;
					
					case TracerBinStream::OpType::Op_s64:		m_stream.Write(reinterpret_cast<s64* >(&m_arrayU64[0]), m_arrayU64.size(), m_key);		break;
					case TracerBinStream::OpType::Op_u64:		m_stream.Write(reinterpret_cast<u64* >(&m_arrayU64[0]), m_arrayU64.size(), m_key);		break;

					case TracerBinStream::OpType::Op_float:		m_stream.Write(reinterpret_cast<float* >(&m_arrayFloat[0]) , m_arrayFloat.size(), m_key);	break;
					case TracerBinStream::OpType::Op_double:	m_stream.Write(reinterpret_cast<float* >(&m_arrayDouble[0]),m_arrayDouble.size(), m_key);	break;
					default:
						assert(0);
					}
					
					m_arrayU8.clear();
					m_arrayU16.clear();
					m_arrayU32.clear();
					m_arrayU64.clear();
					m_arrayFloat.clear();
					m_arrayDouble.clear();
				}*/
				
				m_lastArrayDepth = -1;
				m_depth--;

				return true; 
			}
		};

		Stream* JsonRead (const char* _filename, StdFile* _stdfile)
		{
			StdFile file (_filename, StdFile::READ);

			if (file.IsValid())
			{				
				auto binstream = new BinStream<MemFile>();
				Stream* stream = binstream;

				if (_stdfile != nullptr)
				{
					stream = binstream->CreateTracer(_stdfile);
				}

				JsonHandler handler(*stream);

				size_t len = file.Size();

				char* buffer = new char[len + 1];

				bool success = file.Read(buffer, len);
				assert(success);

				buffer[len] = 0;

				rapidjson::InsituStringStream readstream(buffer);
				rapidjson::Reader reader;
				reader.Parse(readstream, handler);

				if (_stdfile != nullptr)
				{
					delete stream;
				}

				delete[] buffer;
				
				binstream->GetFile()->SeekSet(0);

				return binstream;
			}

			return nullptr;
		}
	}
}
