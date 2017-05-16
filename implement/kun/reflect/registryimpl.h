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
#include <map>

#include "interface\dynamic\kun\reflect\registry.h"

namespace kun
{
	namespace reflect
	{
		class RegistryImpl : public Registry
		{
			std::set<Descriptor>				m_classes;
			std::map<size_t,const Descriptor*>	m_typeidaccess;

			virtual void				 Register(const MetaClassInfo& _classInfo, ModuleID _moduleID)	final;

		public:

			RegistryImpl(ModuleID _moduleID);

			virtual void				 Unregister			(ModuleID _moduleID)							final;
			virtual const MetaClassInfo* FindMetaClass		(NameCRC _name)									const final;
			virtual const MetaClassInfo* FindMetaClass		(const std::type_info& _typeid)					const final;
			virtual const MetaClassInfo* TryDeserializeClass(Deserializer& _deserializer, void*& _object)	const final;
			virtual void				 Dump				()												const final;
			virtual	const_iterator		 begin				()												const final;
			virtual	const_iterator		 end				()												const final;
		};

	} // reflect::
} // kun::

namespace kref = kun::reflect;
