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

#include "interface\dynamic\kun\reflect\id.h"

#define KREF_GETFIELD(CLASS,MEMBER)	kun::reflect::GetField<CLASS, decltype(CLASS::MEMBER), &CLASS::MEMBER>

namespace kun
{
	namespace reflect
	{
		class Stream;
		class Serializer;
		class Deserializer;
		struct AttributesPack;

		typedef	void* (*GetFieldCall)(void*);

		template <class T, class M, M T::*FieldType> void* GetField(void* _base)
		{
			T* object = static_cast<T*>(_base);		// in case of virtual inheritance, needs dynamic_cast here
			return &(object->*FieldType);
		}

		struct Type
		{
			typedef void(*Serializer)  (Serializer& _stream	 , const void* _adr, const char* _name);
			typedef void(*Deserializer)(Deserializer& _stream, void* _adr	   , const char* _name);

			const Serializer	m_serializer;
			const Deserializer	m_deserializer;
			const NameCRC		m_name;
			const size_t		m_size;
			const Type*			m_subtype;

			Type(NameCRC _name, size_t _size, Type* _subtype)
				: m_serializer(nullptr)		// propre au type
				, m_deserializer(nullptr)
				, m_name(_name)				// propre au type
				, m_size(_size)				// potentiellement différent pour les types complexes
				, m_subtype(_subtype)		// diffère pour les types complexes
			{}
		};

		/*
		template<class T> struct AnonymousType : public Type
		{
			AnonymousType(NameCRC _name) : Type(_name, sizeof(T), nullptr) {}

			static AnonymousType<T>	m_info;
		};

		template<unsigned TypeID> struct AnonymousTypeComplex : public Type
		{
			AnonymousTypeComplex() : Type(TypeID, 0, &m_info) {}

			static const NameCRC				m_typeid = TypeID;
			static AnonymousTypeComplex<TypeID> m_info;
		};

		template<class T> struct AnonymousType<std::vector<T>> : public AnonymousTypeComplex<StringCRC("std::vector")>
		{
			AnonymousType(NameCRC _name) : AnonymousTypeComplex(_name, sizeof(T), &AnonymousType<T>::m_info) {}
		};*/

		struct Field
		{
			NameCRC				name;
			NameCRC				nameoftype;
			GetFieldCall		getfield;
			size_t				size;
			Type::Serializer	serialize;
			Type::Deserializer	deserialize;
			AttributesPack*     attributes;
			const char*			debugname;

			bool operator == (NameCRC _name) const { return _name == name; }
		};
	} // reflect::
} // kun::

namespace kref = kun::reflect;
