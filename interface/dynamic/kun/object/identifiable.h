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

#include "interface\dynamic\kun\singleptr.h"
#include "interface\dynamic\kun\reflect\stream.h"
#include "interface\dynamic\kun\reflect\typetraits.h"

namespace kun
{
	namespace reflect
	{
		class MetaClassInfo;
	}

	namespace object
	{
		class Identifiable;

		class IdentifiableSet
		{
		public:
			virtual ~IdentifiableSet() {}
			virtual void		  AddIdentifiable		(u64 _id, Identifiable* _object)					   = 0;
			virtual Identifiable* RetrieveIdentifiable	(u64 _id)											   = 0;
			virtual void		  RemoveIdentifiable	(u64 _id)					  						   = 0;
			virtual void		  Resolve				(void* _adr, const reflect::MetaClassInfo& _metaclass) = 0;
		};


		class Identifiable : public reflect::Polymorph
		{
			friend class reflect::MetaClass<Identifiable>;

			u64	m_id;
		
		public:

			u64 GetId() const { return m_id; }

			Identifiable (void* _adr) : reflect::Polymorph(_adr) {}
	
			virtual ~Identifiable()
			{
				SinglePtr<IdentifiableSet>::Get()->RemoveIdentifiable (m_id);
				m_id = 0;
			}
		};


		template <class T> class IdentifiablePtr
		{
			static const u64 ISOWNER	= 1;
			static const u64 ISRESOLVED = 2;

			u64 m_id;
			u64 m_flags;
			T*  m_p;

		public:

			IdentifiablePtr() 
				: m_id	 (0)
				, m_flags(0)
				, m_p	 (nullptr)
			{
				static_assert( static_cast<Identifiable>(static_cast<T*>(nullptr)) == nullptr );	// Check T is an Identifiable
			}

			u64  GetId()		const	{ return m_id; }
			T*   Get()			const	{ return m_p;  }
			T&   operator->()	const	{ assert(m_p != nullptr); return *m_p; }
			T&   operator*()	const	{ assert(m_p != nullptr); return *m_p; }
			bool IsOwner()		const	{ return (m_flags & ISOWNER	  ) != 0; }
			bool IsResolved()	const	{ return (m_flags & ISRESOLVED) != 0; }

			void SetOwnership (bool _isowner) 
			{ 
				m_flags = _isowner ? m_flags | ISOWNER : m_flags & (~ISOWNER);
			}

			void Assign(T* _p)
			{
				m_p = _p;
			}

			void Assign(T* _p, bool _isowner)
			{
				assert((m_p == nullptr) || (IsOwner() == false));
				m_p = _p;
				SetOwnership(_isowner);
			}

			void AssignId(u64 _id, bool _isowner)
			{
				m_id = _id;
				SetOwnership(_isowner);
			}
		};

	} // object

	namespace reflect
	{
		class MetaClassInfo;

		template<class T> struct TypeTraits< object::IdentifiablePtr<T> >
		{
			static void Serialize(Serializer& _serializer, const void* _adr)
			{
				const auto ptr = static_cast<const object::IdentifiablePtr<T>*>(_adr);

				const auto id		= ptr->GetId();
				const bool isowner	= ptr->IsOwner();

				_serializer.m_stream.Write( KREF_STRINGIZE(id) );
				_serializer.m_stream.Write( KREF_STRINGIZE(isowner) );

				if (isowner)
				{
					const T* p = ptr->Get();
					auto classinfo = SinglePtr<Registry>::Get()->FindMetaClass(typeid(*p));
					assert(classinfo != nullptr);
					_serializer.Run(p, *classinfo);
				}
			}

			static void Deserialize(Deserializer& _deserializer, void* _adr)
			{
				auto ptr = static_cast<object::IdentifiablePtr<T>*>(_adr);


				if (ptr->IsOwner())
				{
					delete ptr->Get();
					ptr->Assign(nullptr, false);
				}

				u64  id			= 0;
				bool isowner	= false;

				_deserializer.m_stream.Read( KREF_STRINGIZE(id) );
				_deserializer.m_stream.Read( KREF_STRINGIZE(isowner) );

				ptr->AssignId(id, isowner);

				if (isowner)
				{
					void* object = nullptr;
					SinglePtr<Registry>::Get()->TryDeserializeClass(_deserializer, object);
					assert(object != nullptr);
					T* typedobject = static_cast<T*>(object);
					ptr->Assign(typedobject);
					SinglePtr<object::IdentifiableSet>::Get()->AddIdentifiable(id, typedobject);
				}
			}

			static const size_t	 m_size = sizeof(object::IdentifiablePtr<T>);
			static const NameCRC m_name = StringCRC("kun::object::IdentifiablePtr<T>");
		};

	} // reflect
} // kun

namespace kobj = kun::object;

