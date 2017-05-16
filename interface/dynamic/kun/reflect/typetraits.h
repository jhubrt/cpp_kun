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

#include "interface\dynamic\kun\reflect\stream.h"

namespace kun
{
	namespace reflect
	{
		class MetaClassInfo;
		
		class Serializer
		{
			typedef void (*SerializeFunc)(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info);

		public:

			Stream&				m_stream;
			const SerializeFunc m_serializeClassFunc;			

			Serializer(Stream& _stream, SerializeFunc _serializeClassFunc)
				: m_stream			   (_stream)
				, m_serializeClassFunc (_serializeClassFunc)
			{}

			Serializer() = delete;
			Serializer(const Serializer&) = delete;

			void Run(const void* _adr, const MetaClassInfo& _info)
			{
				m_serializeClassFunc(*this, _adr, _info);
			}
		};

		class Deserializer
		{
			typedef void (*DeserializeFunc)(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info);

		public:

			Stream&					m_stream;
			const DeserializeFunc	m_deserializeClassFunc;			
			const DeserializeFunc	m_deserializeClassBodyFunc;			

			Deserializer(Stream& _stream, DeserializeFunc _deserializeClassFunc, DeserializeFunc _deserializeClassBodyFunc)
				: m_stream					 (_stream)
				, m_deserializeClassFunc     (_deserializeClassFunc)
				, m_deserializeClassBodyFunc (_deserializeClassBodyFunc)
			{}

			void Run(void* _adr, const MetaClassInfo& _info)
			{
				m_deserializeClassFunc(*this, _adr, _info);
			}
		};

		template<class T> struct TypeTraits 
		{
			static void Serialize   (Serializer&   _serializer  , const void* _adr, const char*) { _serializer  .m_serializeClassFunc	(_serializer  , _adr, MetaClass<T>::m_info); }
			static void Deserialize (Deserializer& _deserializer, void*       _adr, const char*) { _deserializer.m_deserializeClassFunc	(_deserializer, _adr, MetaClass<T>::m_info); }

			static const size_t	 m_size	= sizeof(T);
			static const NameCRC m_name	= 0;
		};

#	define KREF_DECLARE_BASICTYPE_TRAIT(TYPE)\
		template<> struct TypeTraits<TYPE>\
		{\
			static void  Serialize	 (Serializer&   _serializer  , const void* _adr, const char* _name) { _serializer  .m_stream.Write(*(const TYPE*)_adr, _name); }\
			static void  Deserialize (Deserializer& _deserializer, void* _adr	  , const char* _name) { _deserializer.m_stream.Read (*(TYPE*)_adr		, _name); }\
			static const size_t			m_size   = sizeof(TYPE);\
			static const NameCRC		m_name   = StringCRC(#TYPE);\
		}

		KREF_DECLARE_BASICTYPE_TRAIT(u64);
		KREF_DECLARE_BASICTYPE_TRAIT(s64);
		KREF_DECLARE_BASICTYPE_TRAIT(u32);
		KREF_DECLARE_BASICTYPE_TRAIT(s32);
		KREF_DECLARE_BASICTYPE_TRAIT(u16);
		KREF_DECLARE_BASICTYPE_TRAIT(s16);
		KREF_DECLARE_BASICTYPE_TRAIT(u8);
		KREF_DECLARE_BASICTYPE_TRAIT(s8);
		KREF_DECLARE_BASICTYPE_TRAIT(bool);
		KREF_DECLARE_BASICTYPE_TRAIT(float);
		KREF_DECLARE_BASICTYPE_TRAIT(double);

		template<> struct TypeTraits<unsigned>
		{
			static void Serialize   (Serializer&   _serializer  , const void* _adr, const char* _name) { _serializer  .m_stream.Write(*(const u32*)_adr, _name); }
			static void Deserialize (Deserializer& _deserializer, void* _adr      , const char* _name) { _deserializer.m_stream.Read (*(u32*)_adr	   , _name); }

			static const size_t	 m_size = sizeof(unsigned);
			static const NameCRC m_name = StringCRC("unsigned");
		};

		template<> struct TypeTraits<int>
		{
			static void Serialize   (Serializer&   _serializer  , const void* _adr, const char* _name) { _serializer  .m_stream.Write(*(const s32*)_adr, _name); }
			static void Deserialize (Deserializer& _deserializer, void* _adr      , const char* _name) { _deserializer.m_stream.Read (*(s32*)_adr	   , _name); }

			static const size_t	 m_size = sizeof(int);
			static const NameCRC m_name = StringCRC("int");
		};

		template<> struct TypeTraits<char>
		{
			static void Serialize   (Serializer&   _serializer  , const void* _adr, const char* _name) { _serializer  .m_stream.Write(*(const u8*)_adr, _name); }
			static void Deserialize (Deserializer& _deserializer, void* _adr      , const char* _name) { _deserializer.m_stream.Read (*(u8*)_adr	  , _name); }

			static const size_t	 m_size = sizeof(char);
			static const NameCRC m_name = StringCRC("char");
		};

		template<> struct TypeTraits<std::string>
		{
			static void Serialize (Serializer& _serializer, const void* _adr, const char* _name) 
			{ 
				auto str = static_cast<const std::string*>(_adr);

				u32 len = u32(str->size() + 1);

				_serializer.m_stream.WriteString(str->c_str(), len, _name); 
			}

			static void Deserialize (Deserializer& _deserializer, void* _adr, const char* _name) 
			{ 
				auto str = static_cast<std::string*>(_adr);

				u32 len = 0;

				_deserializer.m_stream.ReadStringLength(len, _name); 
				str->resize(len); 
				_deserializer.m_stream.ReadString(const_cast<char*>(str->c_str()), len, _name); 
			}

			static const size_t	 m_size = sizeof(std::string);
			static const NameCRC m_name = StringCRC("std::string");
		};

		template<class T> struct TypeTraits<std::vector<T>>
		{
			static void Serialize (Serializer& _serializer, const void* _adr, const char* _name) 
			{ 
				auto vec = static_cast<const std::vector<T>*>(_adr);
				
				const u32 size = u32(vec->size());

				_serializer.m_stream.Write(size, _name); 

				_serializer.m_stream.StartArray(TypeTraits<T>::m_name, _name);

				for (u32 t = 0 ; t < size ; t++)	// can be optimized for basic types by specialization...
				{
					TypeTraits<T>::Serialize(_serializer, &((*vec)[t]), _name);
				}

				_serializer.m_stream.EndArray(_name);
			}

			static void Deserialize (Deserializer& _deserializer, void* _adr, const char* _name) 
			{ 
				auto vec = static_cast<std::vector<T>*>(_adr);

				u32 size = 0;

				_deserializer.m_stream.Read(size, _name); 
				vec->clear();

				if (size > 0)
				{
					vec->resize(size);

					for (u32 t = 0; t < size; t++)	// can be optimized for basic types by specialization...
					{
						TypeTraits<T>::Deserialize(_deserializer, &((*vec)[t]), _name);
					}
				}
			}

			static const size_t	 m_size = sizeof(std::vector<T>);
			static const NameCRC m_name = StringCRC("std::vector");
		};


		struct Polymorph
		{
			Polymorph(void* _adr) { assert(_adr == this); }
			virtual ~Polymorph() {}
		};

	} // reflect::
} // kun::

namespace kref = kun::reflect;
