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

#include <algorithm>

#include "interface\dynamic\kun\reflect\metaclass.h"

namespace kun
{
	namespace reflect
	{
		template<bool usefieldsection> void SerializeClassImpl(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info)
		{
			Stream& stream = _serializer.m_stream;

			stream.Open(_info.m_debugname);
			{
				stream.WriteCRC(_info.m_name, _info.m_debugname, "classname");

				{
					const u32 nbbaseclasses = u32(_info.m_bases.size());

					stream.Write( KREF_STRINGIZE(nbbaseclasses) );

					if (nbbaseclasses > 0)
					{
						stream.StartArray(0, "baseclasses");

						for (const Base* it = _info.m_bases.m_begin; it != _info.m_bases.m_end; it++)
						{
							// this method only work with normal inheritance
							// with virtual inheritance, I suppose we would have to register a callack on a template cast operator
							_serializer.m_serializeClassFunc(_serializer, it->translate(_adr), *it->base);
						}

						stream.EndArray();
					}
				}

				{
					const u32 nbfields = u32(_info.m_fields.size());

					stream.Write( KREF_STRINGIZE(nbfields) );
					
					for (const Field* it = _info.m_fields.m_begin; it != _info.m_fields.m_end; it++)
					{
						stream.WriteCRC(it->name, it->debugname, "name");

						if (usefieldsection)
						{
							stream.Open(it->debugname);
							stream.Write(it->nameoftype, "type");
						}

						it->serialize(_serializer, it->getfield(const_cast<void*>(_adr)), "value");

						if (usefieldsection)
						{
							stream.Close(it->debugname);
						}
					}
				}
			}
			
			stream.Close(_info.m_debugname);
		}

		void SerializeClass(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info)
		{
			SerializeClassImpl<true>(_serializer, _adr, _info);
		}

		void SerializeClassLight(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info)
		{
			SerializeClassImpl<false>(_serializer, _adr, _info);
		}

		void SerializeClassUltraLight(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info)
		{
			Stream& stream = _serializer.m_stream;

			stream.Open(_info.m_debugname);
			{
				stream.WriteCRC(_info.m_name, _info.m_debugname, "classname");
				{
					const u32 nbbaseclasses = u32(_info.m_bases.size());

					if (nbbaseclasses > 0)
					{
						stream.StartArray(0, "baseclasses");

						for (const Base* it = _info.m_bases.m_begin; it != _info.m_bases.m_end; it++)
						{
							// this method only work with normal inheritance
							// with virtual inheritance, I suppose we would have to register a callack on a template cast operator
							_serializer.m_serializeClassFunc(_serializer, it->translate(_adr), *it->base);
						}

						stream.EndArray();
					}
				}

				for (const Field* it = _info.m_fields.m_begin; it != _info.m_fields.m_end; it++)
				{
					it->serialize(_serializer, it->getfield(const_cast<void*>(_adr)), it->debugname);
				}
			}

			stream.Close(_info.m_debugname);
		}


		template<bool usefieldsection> void DeserializeClassBodyonlyImpl (Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info)		
		{
			Stream& stream = _deserializer.m_stream;
			kref::Stream::Section section;
			u32 nbbaseclasses = 0;


			stream.Read( KREF_STRINGIZE(nbbaseclasses) );

			for (u32 i = 0; i < nbbaseclasses; i++)
			{
				stream.ReadSection (section);

				NameCRC classname;

				stream.Read(classname);

				kref::Bases::const_iterator it = _info.m_bases.find (classname);

				if (it != _info.m_bases.end())
				{
					_deserializer.m_deserializeClassFunc(_deserializer, it->translate(_adr), *it->base);
				}
				else
				{
					stream.m_file.SeekRel(section.m_size - sizeof(classname));
				}
			}

			u32 nbfields = 0;

			stream.Read( KREF_STRINGIZE(nbfields) );

			for (u32 i = 0; i < nbfields; i++)
			{
				NameCRC name;

				stream.Read(name, "name");

				kref::Fields::const_iterator it = _info.m_fields.find(name);

				if (it != _info.m_fields.end())
				{
					if (usefieldsection)
					{
						u32 type;

						stream.ReadSection (section, it->debugname);
						stream.Read( KREF_STRINGIZE(type) );
						assert(type == it->nameoftype);
					}

					it->deserialize(_deserializer, it->getfield(_adr), "value");
				}
				else
				{
					assert(usefieldsection);
					stream.ReadSection (section, "?");
					stream.m_file.SeekRel(section.m_size);
				}
			}
		}

		void DeserializeClassBodyonly(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info)
		{
			DeserializeClassBodyonlyImpl<true>(_deserializer, _adr, _info);
		}

		void DeserializeClassBodyonlyLight(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info)
		{
			DeserializeClassBodyonlyImpl<false>(_deserializer, _adr, _info);
		}

		void DeserializeClass(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info)
		{
			kref::Stream::Section objectsection;
			kref::NameCRC classname;

			_deserializer.m_stream.ReadSection(objectsection);
			_deserializer.m_stream.Read(classname, "classname");
			assert(classname == _info.m_name);

			_deserializer.m_deserializeClassBodyFunc(_deserializer, _adr, _info);
		}


		void DeserializeClassBodyonlyUltraLight (Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info)		
		{
			for (const Base* it = _info.m_bases.m_begin; it != _info.m_bases.m_end; it++)
			{
				// this method only work with normal inheritance
				// with virtual inheritance, I suppose we would have to register a callack on a template cast operator
				_deserializer.m_deserializeClassFunc(_deserializer, it->translate(_adr), *it->base);
			}

			for (auto& f : _info.m_fields)
			{
				f.deserialize(_deserializer, f.getfield(_adr), f.debugname);				
			}
		}


	} // reflect::
} // kun::
