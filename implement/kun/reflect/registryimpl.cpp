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
#include "implement\kun\reflect\registryimpl.h"


namespace kun
{
	template<> reflect::Registry* SinglePtr<reflect::Registry>::ms_instance = nullptr;

	namespace reflect
	{	
		ModuleID Registry::ms_moduleID = 0;

		RegistryImpl::RegistryImpl(ModuleID _moduleID)
		{
			InitModuleID(_moduleID);
		}

		void RegistryImpl::Register(const MetaClassInfo& _classinfo, ModuleID _moduleID)
		{
			_classinfo.m_fields.checkNoCRCDuplicate();

			Descriptor sorter;
						
			sorter.m_name	  = _classinfo.m_name;
			sorter.m_info	  = &_classinfo;
			sorter.m_moduleID = _moduleID;

			assert(m_classes.find(sorter) == m_classes.end());

			auto inserted = m_classes.insert(sorter);
	
			const Descriptor* newdescriptor = &(*inserted.first);

			m_typeidaccess.insert( std::pair<const size_t,const Descriptor*>(_classinfo.m_typeid.hash_code(), newdescriptor) );
		}

		const MetaClassInfo* RegistryImpl::FindMetaClass(NameCRC _name) const
		{
			Descriptor sorter = { _name, nullptr };

			auto result = m_classes.find(sorter);

			if (result != m_classes.end() )
			{
				return result->m_info;
			}

			return nullptr;
		}

		const MetaClassInfo* RegistryImpl::FindMetaClass(const std::type_info& _typeid) const
		{
			auto it = m_typeidaccess.find(_typeid.hash_code());

			if (it != m_typeidaccess.end())
			{
				return it->second->m_info;
			}

			return nullptr;
		}

		Registry::const_iterator RegistryImpl::begin() const
		{
			return m_classes.cbegin();
		}

		Registry::const_iterator RegistryImpl::end() const
		{
			return m_classes.cend();
		}

		void RegistryImpl::Unregister(ModuleID _moduleID)
		{
			for (auto it = m_classes.begin(); it != m_classes.end(); )
			{
				auto itnext = it;
				itnext++;

				if (it->m_moduleID == _moduleID)
				{
					m_classes.erase(it);
				}
				it = itnext;
			}
		}

		static void RegistryImpl_printAttributes(const AttributesPack* _attributes)
		{
			if (_attributes != nullptr)
			{
				printf (" { attributes: ");

				for (const Attribute* const* it = _attributes->begin(); it != _attributes->end(); it++)
				{
					const Attribute* attr = *it;

					printf("(%s %p) ", typeid(*attr).name(), attr);
				}

				printf ("}");
			}
		}

		void RegistryImpl::Dump() const
		{
			printf ("\n-----------------------------------------------------------\n");
			printf ("Dump kun::reflect::Registry\n");
			printf ("-----------------------------------------------------------\n\n");

			for (const auto& it : m_classes)
			{
				printf("%s: id=0x%-8lx", it.m_info->m_debugname, it.m_info->m_name);
				RegistryImpl_printAttributes(it.m_info->m_attributes);
				printf("\n");

				if (it.m_info->m_bases.begin() != it.m_info->m_bases.end())
				{
					printf("    base classes:\n");

					for (const auto& itb : it.m_info->m_bases)
					{
						printf("        %s (0x%lx, offset = %lld)\n", itb.base->m_debugname, itb.base->m_name, itb.offset);
					}

					printf("\n");
				}

				if (it.m_info->m_fields.begin() != it.m_info->m_fields.end())
				{
					printf("    fields:\n");
					
					for (const auto& it2 : it.m_info->m_fields)
					{
						printf("        %-16s: id=0x%-8lx size=%lld", it2.debugname, it2.name, it2.size);
						RegistryImpl_printAttributes(it2.attributes);
						printf("\n");
					}

					printf("\n");
				}
				
				if (it.m_info->m_methods.begin() != it.m_info->m_methods.end())
				{
					printf("    methods:\n");

					for (const auto& it2 : it.m_info->m_methods)
					{
						printf("        %-16s: id=0x%-8lx funcptr=%p %s", it2.debugname, it2.name, it2.funcptr, (it2.flags & Method::Flags::STATIC) ? "static" : "method");
						RegistryImpl_printAttributes(it2.attributes);
						printf("\n");
					}

					printf("\n");
				}

				printf("\n");
			}
		}

		const MetaClassInfo* RegistryImpl::TryDeserializeClass(Deserializer& _deserializer, void*& _object) const
		{
			Stream::Section objectsection;
			NameCRC classname;
			Stream& stream = _deserializer.m_stream;


			stream.ReadSection(objectsection);
			stream.Read(classname, "classname");

			auto metaclass = FindMetaClass(classname);

			if (metaclass != nullptr)
			{
				_object = metaclass->m_factory();
				_deserializer.m_deserializeClassBodyFunc(_deserializer, _object, *metaclass);
			}
			else
			{
				_object = nullptr;
				stream.m_file.SeekRel(objectsection.m_size - sizeof(classname));
			}

			return metaclass;
		}


	} // reflect::
} // kun::
