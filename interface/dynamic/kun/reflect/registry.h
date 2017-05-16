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

#include <set>
#include <typeinfo>
#include <memory>

#include "interface\dynamic\kun\singleptr.h"
#include "interface\dynamic\kun\reflect\id.h"

namespace kun
{
	namespace reflect
	{
		class MetaClassInfo;
		class Stream;
		class Deserializer;

		class Registry
		{
			static ModuleID	ms_moduleID;

			virtual void				 Register		(const MetaClassInfo& _classInfo, ModuleID _moduleID) = 0;

		public:
			struct Descriptor
			{
				NameCRC					m_name;
				const MetaClassInfo*	m_info;
				ModuleID				m_moduleID;

				bool operator < (const Descriptor& _o) const { return m_name < _o.m_name; }
			};

			typedef std::set<Descriptor>::const_iterator const_iterator;

			virtual	const_iterator		 begin()															const = 0;
			virtual	const_iterator		 end()																const = 0;

			virtual void				 Unregister			(ModuleID _moduleID)								  = 0;
			virtual const MetaClassInfo* FindMetaClass		(NameCRC _name)									const = 0;
			virtual const MetaClassInfo* FindMetaClass		(const std::type_info& _typeid)					const = 0;
			virtual const MetaClassInfo* TryDeserializeClass(Deserializer& _deserializer, void*& _object)	const = 0;
			virtual void				 Dump				()												const = 0;		
			virtual     				 ~Registry			() {}

			static void					 InitModuleID		(ModuleID _moduleID) { ms_moduleID = _moduleID; }

			// Registration helper
			template<class T> static void Register()
			{
				SinglePtr<Registry>::Get()->Register( kref::MetaClass<T>::m_info, ms_moduleID );
			}

			static void	Unregister () 
			{ 
				SinglePtr<Registry>::Get()->Unregister(ms_moduleID);
			}
		};

	} // reflect::
} // kun::

namespace kref = kun::reflect;
