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

#include "interface\dynamic\kun\reflect\metaclass.h"
#include "implement\kun\object\identifiablesetimpl.h"


namespace kun
{
	template<> object::IdentifiableSet* SinglePtr<object::IdentifiableSet>::ms_instance = nullptr;

	namespace object
	{		
		void IdentificableSetImpl::AddIdentifiable(u64 _id, Identifiable* _object)
		{
			assert(m_identifiables.find(_id) == m_identifiables.end());
			m_identifiables.insert(std::make_pair(_id, _object));
		}

		void IdentificableSetImpl::RemoveIdentifiable(u64 _id)
		{
			if (_id != 0)
			{
				m_identifiables.erase(_id);
			}
		}

		Identifiable* IdentificableSetImpl::RetrieveIdentifiable (u64 _id)
		{
			auto it = m_identifiables.find(_id);

			if (it != m_identifiables.end())
			{
				return it->second;
			}
			else
			{
				return nullptr;
			}
		}

		void IdentificableSetImpl::Resolve(void* _adr, const reflect::MetaClassInfo& _metaclass)
		{			
			constexpr auto nameCRC = reflect::TypeTraits<IdentifiablePtr<Identifiable>>::m_name;

			for (const auto& b : _metaclass.m_bases)
			{
				Resolve( b.translate(_adr), *(b.base) );
			}

			for (const auto& f : _metaclass.m_fields)
			{
				if (f.nameoftype == nameCRC)
				{
					auto iptr = static_cast<IdentifiablePtr<Identifiable>*>(f.getfield(_adr));	// not very elegant => need a derivation to access a common part...
			
					auto identifiable = IdentificableSetImpl::RetrieveIdentifiable(iptr->GetId());
					assert(identifiable != nullptr);
					iptr->Assign(identifiable);
				}
			}
		}
	} // object

} // kun

namespace kobj = kun::object;

