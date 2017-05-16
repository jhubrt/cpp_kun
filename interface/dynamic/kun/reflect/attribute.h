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

#include <tuple>
#include <array>

namespace kun
{
    namespace reflect
    {
		struct Attribute
		{
			virtual ~Attribute() {}	// to be able to use rtti
		};

		struct AttributesPack
		{
			typedef const Attribute** const_iterator;

			virtual const size_t			size()		  const = 0;
			virtual const Attribute*		operator[] (size_t) = 0;
			virtual const Attribute* const* begin()		  const = 0;
			virtual const Attribute* const* end()		  const = 0;

			virtual ~AttributesPack() {}
		};

		template<typename... Params> class AttributesStorage : public AttributesPack
		{
			typedef const Attribute* ConstAttrib;
			typedef std::array<ConstAttrib, sizeof...(Params)>	Array;

			std::tuple<Params...>						m_attributes;
			std::array<ConstAttrib, sizeof...(Params)>	m_access;

			template<int index> constexpr void assign(Array& _array, FalseType) const
			{
				static_assert(index == -1, ""); // stop recurse
			}

			template<int index> constexpr void assign(Array& _array, TrueType) const
			{
				static_assert(index >= 0, "");
				_array[index] = &std::get<index>(m_attributes);
				assign<index - 1>(_array, typename Test<index != 0>::type ());
			}

		public:
			constexpr AttributesStorage(Params const&... args)
				: m_attributes(args...)
			{
				assign<sizeof...(Params)-1>(m_access, TrueType());
			}

			AttributesStorage(const AttributesStorage& _o)
				: m_attributes(_o.m_attributes)
			{
				// pkoi ca passe pas par là ?
				assign<sizeof...(Params)-1>(m_access, TrueType());
			}
			
			AttributesStorage(AttributesStorage && _o)
				: m_attributes(std::move(_o.m_attributes))
			{
				assign<sizeof...(Params)-1>(m_access, TrueType());
			}

			virtual const size_t			size()		const { return sizeof...(Params); }
			virtual const Attribute*		operator[] (size_t _index) { return m_access[_index]; }
			virtual const Attribute* const*	begin()		const { return &m_access[0]; }
			virtual const Attribute* const*	end()		const { return 1 + &m_access[sizeof...(Params) - 1]; }
		};


		template<class... Types> static auto StoreAttributes(Types const&... args)
		{
			return AttributesStorage<Types...>(args...);
		}

    } // reflect::
} // kun::
