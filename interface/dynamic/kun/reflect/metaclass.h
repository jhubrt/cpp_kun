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

#include "interface\dynamic\kun\reflect\attribute.h"
#include "interface\dynamic\kun\reflect\stream.h"
#include "interface\dynamic\kun\reflect\field.h"
#include "interface\dynamic\kun\reflect\method.h"
#include "interface\dynamic\kun\reflect\typetraits.h"

// Seems clang put const pods into protected memory space ? (cannot write there...)
#ifdef __clang__
#	define KREF_CONST
#else
#	define KREF_CONST const
#endif


#define KREF_GETFIELD(CLASS,MEMBER)	kun::reflect::GetField<CLASS, decltype(CLASS::MEMBER), &CLASS::MEMBER>
#define KREF_ALLOW_METACLASS(CLASS)	friend class kun::reflect::MetaClass<CLASS>;

// Strangely this does not work into constexpr expression nor struct static int... (not evaluated as constant => I suppose because of "if 0" condition into static_cast)
#define KREF_CLASS_OFFSET(BASE,DERIVED) ( reinterpret_cast<ptrdiff_t>( static_cast<DERIVED*>( reinterpret_cast<BASE*>((void*)8) ) ) - 8)

#define KREF_ABSTRACT_CLASS(CLASS) template<> const kref::MetaClass<CLASS> kref::MetaClass<CLASS>::m_info(StringCRC(#CLASS), nullptr, nullptr, #CLASS)
#define KREF_CLASS(CLASS) template<> const kref::MetaClass<CLASS> kref::MetaClass<CLASS>::m_info(StringCRC(#CLASS), [](){ return static_cast<void*>(new CLASS); }, [](void* _adr){ return static_cast<void*>(new(_adr) CLASS); }, #CLASS)

// Use PODS init to avoid constructor calls (other possibility is initializers list)
#define KREF_CLASS_SUPERS_BEGIN(CLASS) template<> const kref::Base kref::MetaClass<CLASS>::m_basesContainer[] = { 
#define KREF_CLASS_SUPER(CLASS,BASE) { &kref::MetaClass<BASE>::m_info, KREF_CLASS_OFFSET(BASE,CLASS) } 
#define KREF_CLASS_SUPERS_END { nullptr, 0 } };

#define KREF_CLASS_FIELDS_BEGIN(CLASS) template<> KREF_CONST kref::Field kref::MetaClass<CLASS>::m_fieldsContainer[] = { 
#define KREF_CLASS_FIELD(CLASS,MEMBER) { kref::StringCRC(#MEMBER), kref::TypeTraits<decltype(CLASS::MEMBER)>::m_name, &KREF_GETFIELD(CLASS, MEMBER), sizeof(CLASS::MEMBER), kref::TypeTraits<decltype(CLASS::MEMBER)>::Serialize, kref::TypeTraits<decltype(CLASS::MEMBER)>::Deserialize, nullptr, #MEMBER }
#define KREF_CLASS_FIELDS_END { 0, 0, nullptr, 0,nullptr, nullptr, nullptr, nullptr } };

#define KREF_CLASS_METHODS_BEGIN(CLASS) template<> KREF_CONST kref::Method kref::MetaClass<CLASS>::m_methodsContainer[] = { 
#define KREF_CLASS_METHOD(CLASS,MEMBER,PROTOTYPE)	{ kref::StringCRC(#MEMBER), KREF_METHOD_STORAGE(PROTOTYPE, &CLASS::MEMBER) , 0							, nullptr, #MEMBER }
#define KREF_CLASS_FUNCTION(CLASS,MEMBER,PROTOTYPE) { kref::StringCRC(#MEMBER), KREF_FUNCTION_STORAGE(PROTOTYPE, CLASS::MEMBER), kref::Method::Flags::STATIC, nullptr, #MEMBER }
#define KREF_CLASS_METHODS_END { 0, 0, 0, 0, 0, 0 } };

// Attributes
#define KREF_CLASS_ATTRIBUTES(...)			{ static auto attrib = kref::StoreAttributes(__VA_ARGS__); m_info.SetAttributes(&attrib); }
#define KREF_FIELD_ATTRIBUTES(FIELD,...)	{ static auto attrib = kref::StoreAttributes(__VA_ARGS__); m_info.SetFieldAttributes(StringCRC(#FIELD), &attrib); }
#define KREF_METHOD_ATTRIBUTES(METHOD,...)	{ static auto attrib = kref::StoreAttributes(__VA_ARGS__); m_info.SetMethodAttributes(StringCRC(#METHOD), &attrib); }


namespace kun
{
	namespace reflect
	{
		class Fields
		{
		public:
			typedef Field*			iterator;
			typedef const Field*	const_iterator;

			const Field* const m_begin;
			const Field* const m_end;

			Fields(const Field* _fields, size_t _size)
				: m_begin(_fields)		
				, m_end(_fields + _size - 1) // last is empty to manage zero size array problem
			{
				log("construct kref::Fields: begin = %p ; end = %p\n", m_begin, m_end);
			}

			const_iterator begin() const { return m_begin; }
			const_iterator end()   const { return m_end;	 }
			const size_t   size()  const { return m_end - m_begin; }
			
			const_iterator find(NameCRC _name) const { return std::find(m_begin, m_end, _name); }

			void checkNoCRCDuplicate () const
			{
				for (auto f = m_begin; f < m_end; f++)
				{
					assert(std::find(f + 1, m_end, f->name) == end());
				}
			}

			/*constexpr FieldsList(const std::initializer_list<Field>& _list)
			: m_begin(_list.begin())
			, m_end(_list.end())
			{}*/
		};

		class Methods
		{
		public:
			typedef Method*			iterator;
			typedef const Method*	const_iterator;

			const Method* const m_begin;
			const Method* const m_end;

			Methods(const Method* _methods, size_t _size)
				: m_begin(_methods)		
				, m_end(_methods + _size - 1) // last is empty to manage zero size array problem
			{
				log("construct kref::Methods: begin = %p ; end = %p\n", m_begin, m_end);
			}

			const Method* begin() const { return m_begin; }
			const Method* end()	  const { return m_end; }
			const size_t  size()  const { return m_end - m_begin; }

			const_iterator find(NameCRC _name) const { return std::find(begin(), end(), _name); }

			/*constexpr FieldsList(const std::initializer_list<Field>& _list)
			: m_begin(_list.begin())
			, m_end(_list.end())
			{}*/
		};

		struct Base
		{
			const MetaClassInfo*	base;
			const ptrdiff_t			offset;

			bool operator == (NameCRC _name) const;
			void*		translate(void*		  _derivedadr) const { return static_cast<u8*>(_derivedadr) + offset; }
			const void* translate(const void* _derivedadr) const { return static_cast<const u8*>(_derivedadr) + offset; }
		};

		class Bases
		{
		public:
			typedef Base*		iterator;
			typedef const Base*	const_iterator;

			const Base* const m_begin;
			const Base* const m_end;

			Bases(const Base* _bases, size_t _size)
				: m_begin(_bases)		
				, m_end(_bases + _size - 1)		// last is empty to manage zero size array problem
			{}

			const Base*  begin()  const { return m_begin; }
			const Base*  end  ()  const { return m_end;	 }
			const size_t size ()  const { return m_end - m_begin; }

			const_iterator find(NameCRC _name) const { return std::find(m_begin, m_end, _name); }
		};

		class MetaClassInfo
		{
		protected:

			void SetFieldAttributes(NameCRC _name, AttributesPack* _attribute) const
			{
				const Field* result = m_fields.find(_name);
				assert(result != m_fields.end());

				log("fields attributes adr = %p\n", &(result->attributes));
				const_cast<AttributesPack*&>(result->attributes) = _attribute;
			}

			void SetMethodAttributes(NameCRC _name, AttributesPack*_attribute) const
			{
				const Method* result = m_methods.find(_name);
				assert(result != m_methods.end());

				const_cast<AttributesPack*&>(result->attributes) = _attribute;
			}

			void SetAttributes(AttributesPack* _attribute) const
			{
				const_cast<AttributesPack*&>(m_attributes) = _attribute;
			}

		public:
			typedef void* (*Factory)();
			typedef void* (*FactoryInplace)(void*);

			MetaClassInfo(
				NameCRC _name, 
				const std::type_info& _typeid,
				const Base* _bases, size_t _nbbases, 
				const Field* _field, size_t _nbfields, 
				const Method* _method, size_t _nbmethods,
				Factory _factory, FactoryInplace _factoryinplace, 
				const char* _debugname)

				: m_name(_name)
				, m_typeid(_typeid)
				, m_bases(_bases, _nbbases)
				, m_fields(_field, _nbfields)
				, m_methods(_method, _nbmethods)
				, m_factory(_factory)
				, m_factoryinplace(_factoryinplace)
				, m_attributes(nullptr)
				, m_debugname(_debugname)
			{
				log("construct kref::MetaClassInfo: debugname = %s\n", _debugname);
			}

			const NameCRC			m_name;
			const std::type_info&   m_typeid;
			const Bases  			m_bases;
			const Fields			m_fields;
			const Methods			m_methods;
			const Factory			m_factory;
			const FactoryInplace	m_factoryinplace;
			AttributesPack*	const	m_attributes;
			const char*				m_debugname;

			template <class T> T* New() const
			{
				T* obj = static_cast<T*>(m_factory());
				assert(typeid(*obj) == typeid(T));
				return obj;
			}

			template <class T> T* NewAbstract() const
			{
				T* obj = static_cast<T*>(m_factory());
				assert(dynamic_cast<T*>(obj) != nullptr);
				return obj;
			}
			
			const Method* FindMethodRecurs(NameCRC _methodname) const
			{
				auto method = m_methods.find(_methodname);

				if (method == nullptr)
				{
					for (auto& c : m_bases)
					{
						method = c.base->FindMethodRecurs(_methodname);

						if (method != nullptr)
						{
							break;
						}
					}
				}

				return method;
			}
		};


		template <class T> class MetaClass : public MetaClassInfo
		{
			static const Base   m_basesContainer[];
			static KREF_CONST Field  m_fieldsContainer[];
			static KREF_CONST Method m_methodsContainer[];

		public:

			MetaClass(NameCRC _name, Factory _factory, FactoryInplace _factoryinplace, const char* _debugname = nullptr) 
				: MetaClassInfo (
					_name, 
					typeid(T),
					m_basesContainer  , sizeof(m_basesContainer)   / sizeof(m_basesContainer [0]),
					m_fieldsContainer , sizeof(m_fieldsContainer)  / sizeof(m_fieldsContainer[0]),
					m_methodsContainer, sizeof(m_methodsContainer) / sizeof(m_methodsContainer[0]),
					_factory, _factoryinplace,
					_debugname) 
			{
				log("construct kref::MetaClass: debugname = %s\n", _debugname);
			}

			const static MetaClass<T> m_info;

			static void Register();
			static bool IsType(const MetaClassInfo& _metaclass) { return _metaclass.m_name == m_info.m_name; }
		};

		//template <class T> const MetaClass<T> MetaClass<T>::m_info;

		void SerializeClass						(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info);
		void SerializeClassLight				(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info);
		void SerializeClassUltraLight			(Serializer& _serializer, const void* _adr, const MetaClassInfo& _info);

		void DeserializeClassBodyonly			(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info);
		void DeserializeClassBodyonlyLight		(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info);
		void DeserializeClassBodyonlyUltraLight (Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info);		
		void DeserializeClass					(Deserializer& _deserializer, void* _adr, const MetaClassInfo& _info);

		template <class T> void Serialize(Serializer& _serializer, const T& _object)
		{
			_serializer.m_serializeClassFunc(_serializer, &_object, MetaClass<T>::m_info);
		}

		inline bool Base::operator == (NameCRC _name) const { return _name == base->m_name; }

	} // reflect::
} // kun::

namespace kref = kun::reflect;
