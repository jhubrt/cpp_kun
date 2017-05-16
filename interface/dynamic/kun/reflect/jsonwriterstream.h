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

#define KREF_JSON_TYPESEPARATOR " "

namespace kun
{
	namespace reflect
	{
		class JSonWriterStream : public Stream
		{
			std::vector<bool> m_enum;
			std::vector<bool> m_intoarray;

			bool LineFeed(bool _allowenum = true)
			{
				u8 temp [4096];
				const size_t indent = m_enum.size();

				if (m_enum.back() && _allowenum)
				{
					WriteText(",");
				}

				WriteText("\n");

				assert(indent * 4 < sizeof(temp));
				memset(temp, ' ', indent * 4);

				m_enum.back() = true;

				return m_file.Write(temp, indent * 4);
			}
		
			template<class I> bool WriteNumber(const I* _v, size_t _num, const char* _format, const char* _prefix, const char* _tag)
			{
				if (m_intoarray.back())
				{
					LineFeed();
				}
				else
				{
					WriteKey(_prefix, _tag);
				}

				if (_num > 1)
				{
					WriteText("[");
				}

				for (size_t t = 0; t < _num; t++)
				{
					char temp[256];

					sprintf_s(temp, _format, _v[t]);
					WriteText(temp);

					if (_num > 1)
					{
						if ((t + 1) < _num)
						{
							WriteText(",");
						}
					}
				}

				if (_num > 1)
				{
					WriteText("]");
				}

				return true;
			}

			bool WriteKey(const char* _prefix, const char* _tag)
			{
				if (_tag != nullptr)
				{
					LineFeed();
					WriteText("\"");
					WriteText(_prefix);
					WriteText(_tag);
					WriteText("\" : ");
				}

				return true;
			}

			bool WriteText(const char* _line)
			{
				size_t length = strlen(_line);
				return m_file.Write((const u8*) _line, length);
			}

		public:

			File& m_file;

			JSonWriterStream (File& _file) 
				: Stream(_file)
				, m_file(_file) 
			{
				m_enum.reserve(128);
				m_enum.push_back(false);

				m_intoarray.reserve(128);
				m_intoarray.push_back(false);

				WriteText("{");
			}

			~JSonWriterStream()
			{
				WriteText("\n}\n");

				assert(m_intoarray.size() == 1);
				assert(m_enum.size() == 1);
			}

			virtual bool Open (const char* _tag = nullptr) override	
			{ 
				const auto isintoarray = m_intoarray.back();

				if (isintoarray == false)
				{
					WriteKey("", _tag);
				}

				LineFeed(isintoarray);

				WriteText("{");

				m_enum.push_back(false);
				m_intoarray.push_back(false);

				return true;
			}
			
			virtual bool Close(const char* _tag = nullptr) override	
			{ 
				m_intoarray.pop_back();
				m_enum.pop_back();

				LineFeed(false);
				WriteText("}");

				return true;
			}

			virtual void StartArray(NameCRC _typename, const char* _tag = nullptr) override
			{
				if (_tag != nullptr)
				{ 
					const char* type = "";

					switch (_typename)
					{
					case TypeTraits<u64> ::m_name:	type = "u64"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<s64> ::m_name:	type = "s64"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<u32> ::m_name:	type = "u32"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<s32> ::m_name:	type = "s32"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<u16> ::m_name:	type = "u16"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<s16> ::m_name:	type = "s16"	KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<u8>  ::m_name:	type = "u8"		KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<s8>  ::m_name:	type = "s8"		KREF_JSON_TYPESEPARATOR;	break;
					case TypeTraits<bool>::m_name:	type = "bool"	KREF_JSON_TYPESEPARATOR;	break;
					}

					WriteKey(type, _tag);
				}
				else
				{
					LineFeed();
				}

				WriteText("[");
				m_enum.push_back(false);
				m_intoarray.push_back(true);
			}

			virtual void EndArray(const char* _tag = nullptr) override
			{
				m_intoarray.pop_back();
				m_enum.pop_back();

				//LineFeed(false);
				WriteText("]");
			}

			virtual bool Write(const u64*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%llu", "u64"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const s64*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%lld", "s64"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const u32*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%lu" , "u32"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const s32*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%ld" , "s32"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const u16*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%u"  , "u16"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const s16*    _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%d"  , "s16"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const u8*     _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%u"  , "u8"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const s8*     _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%d"  , "s8"		KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const bool*   _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%u"  , "bool"	KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const float*  _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%f"  , "float"	KREF_JSON_TYPESEPARATOR	,_tag); }
			virtual bool Write(const double* _v, size_t _num, const char* _tag = nullptr) override { return WriteNumber(_v, _num, "%f"  , "double"	KREF_JSON_TYPESEPARATOR ,_tag); }
			
			virtual bool WriteString(const char* _string, u32 _length, const char* _tag = nullptr) override
			{
				WriteKey("string" KREF_JSON_TYPESEPARATOR, _tag);

				WriteText("\"");
				WriteText(_string);
				WriteText("\"");

				return true;
			}

			template<class I> bool Write(const I& _v, const char* _tag = nullptr) { return JSonWriterStream::Write(&_v, 1, _tag); } 			
			template<class I> bool Read (I& _v, const char* _tag = nullptr)       { assert(0) ; return false; } 

			virtual bool WriteCRC(const u32& _v, const char* _sourcestring, const char* _tag = nullptr) override
			{ 
				char temp[1024];
				sprintf (temp, "%s#%s", _tag, _sourcestring);
				u32 dummy = 0;
				return WriteNumber(&dummy, 1, "\"\"", "u32" KREF_JSON_TYPESEPARATOR, temp);
			}
			
			virtual bool WriteCRC(const u64& _v, const char* _sourcestring, const char* _tag = nullptr) override
			{ 
				char temp[1024];
				sprintf (temp, "#%s#%s", _tag, _sourcestring);
				u64 dummy = 0;
				return WriteNumber(&dummy, 1, "\"\"", "u64" KREF_JSON_TYPESEPARATOR, temp);
			}

			virtual bool Read(u64*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(s64*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(u32*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(s32*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(u16*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(s16*  _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(u8*   _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(s8*   _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(bool* _v, size_t _num, const char* _tag = nullptr)	override { assert(0); return false; }
			virtual bool Read(float*  _v, size_t _num, const char* _tag = nullptr)  override { assert(0); return false; }
			virtual bool Read(double* _v, size_t _num, const char* _tag = nullptr)  override { assert(0); return false; }

			virtual bool ReadStringLength (u32& _length, const char* _tag = nullptr)					override { assert(0); return false; }
			virtual bool ReadString (char* _string, u32 _length, const char* _tag = nullptr)			override { assert(0); return false; }
			virtual bool ReadSection(Section& _section, const char* _tag = nullptr)						override { assert(0); return false; }
			virtual bool ReadSection(Section& _section, u64& _sectionEnd, const char* _tag = nullptr)	override { assert(0); return false; }
		};

	} // reflect::
} // kun::
