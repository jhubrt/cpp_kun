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

#include "interface\dynamic\kun\reflect\typetraits.h"
#include "interface\dynamic\kun\reflect\metaclass.h"
#include "interface\dynamic\kun\reflect\registry.h"

namespace kun
{
	namespace reflect
	{
		class MetaClassInfo;

		typedef std::vector<Polymorph*> rttivector;

		template<> struct TypeTraits<rttivector>
		{
			static void Serialize(Serializer& _serializer, const void* _adr, const char* _name)
			{
				auto vec = static_cast<const rttivector*>(_adr);

				const u32 size = u32(vec->size());

				_serializer.m_stream.Write(size, _name);

				_serializer.m_stream.StartArray(0, _name);

				for (u32 t = 0; t < size; t++)	// can be optimized for basic types by specialization...
				{
					auto object = (*vec)[t];
					auto classinfo = SinglePtr<Registry>::Get()->FindMetaClass(typeid(*object));
					assert(classinfo != nullptr);
					_serializer.Run((*vec)[t], *classinfo);
				}

				_serializer.m_stream.EndArray(_name);
			}

			static void Deserialize(Deserializer& _deserializer, void* _adr, const char* _name)
			{
				auto vec = static_cast<rttivector*>(_adr);

				u32 size = 0;

				_deserializer.m_stream.Read(size, _name);
				vec->clear();

				if (size > 0)
				{
					vec->resize(size);

					for (u32 t = 0; t < size; t++)	// can be optimized for basic types by specialization...
					{
						void* object = nullptr;
						SinglePtr<Registry>::Get()->TryDeserializeClass(_deserializer, object);
							
						(*vec)[t] = static_cast<Polymorph*>(object);
					}
				}
			}

			static const size_t	 m_size = sizeof(rttivector);
			static const NameCRC m_name = StringCRC("kun::reflect::rttivector");
		};

		template<class T> struct TypeTraits< std::unique_ptr<T> >
		{
			static_assert( (bool) std::is_base_of<Polymorph,T>::value, "your type should be a Polymorph" );

			static void Serialize(Serializer& _serializer, const void* _adr, const char*)
			{
				const auto ptr = static_cast<const std::unique_ptr<T>*>(_adr);

				const auto p = ptr->get();
				auto classinfo = SinglePtr<Registry>::Get()->FindMetaClass(typeid(*p));
				assert(classinfo != nullptr);

				_serializer.Run(p, *classinfo);
			}

			static void Deserialize(Deserializer& _deserializer, void* _adr, const char*)
			{
				auto ptr = static_cast<std::unique_ptr<Polymorph>*>(_adr);

				if (ptr->get() != nullptr)
				{
					delete ptr->release();
				}					

				void* object = nullptr;
				SinglePtr<Registry>::Get()->TryDeserializeClass(_deserializer, object);
				assert(object != nullptr);
				Polymorph* typedobject = static_cast<Polymorph*>(object);
				*ptr = std::unique_ptr<Polymorph>(typedobject);
			}

			static const size_t	 m_size = sizeof(std::unique_ptr<Polymorph>);
			static const NameCRC m_name = StringCRC("std::unique_ptr<Polymorph>");
		};

	} // reflect
} // kun
