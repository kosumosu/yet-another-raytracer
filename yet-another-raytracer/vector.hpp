#pragma once

#include "helper.hpp"
#include <type_traits>

namespace math
{
	template<typename TSpace, size_t DIMENSIONS, class Enable = void>
	class vector {};

	// Represents vector in space of TSpace^DIMENSIONS.
	template<typename TSpace, size_t DIMENSIONS>
	class vector<TSpace, DIMENSIONS, std::enable_if_t<(DIMENSIONS > 0)>>
	{
	public:

		constexpr static size_t dimensions() { return DIMENSIONS; }

		constexpr vector() :
			m_elements{}
		{
		}

		template <typename TOther, class = std::enable_if_t<std::is_constructible<TSpace, TOther>::value>>
		vector(const vector<TOther, DIMENSIONS> & other)
		{
			//iterate<0, dimensions() - 1>([&](size_t i) { m_elements[i] = TSpace(other[i]); });
			for (size_t i = 0; i < dimensions(); ++i)
			{
				m_elements[i] = TSpace(other[i]);
			}
		}

		template <typename TOther>
		explicit vector(const TOther other[])
		{
			//iterate<0, dimensions() - 1>([&](size_t i) { m_elements[i] = TSpace(other[i]); });
			for (size_t i = 0; i < dimensions(); ++i)
			{
				m_elements[i] = TSpace(other[i]);
			}
		}

		template <typename TValue, class = std::enable_if_t<(DIMENSIONS > 1)>>
		explicit vector(const TValue & value)
		{
			//iterate<0, dimensions() - 1>([&](size_t i) { m_elements[i] = TSpace(value); });

			for (size_t i = 0; i < dimensions(); ++i)
			{
				m_elements[i] = TSpace(value);
			}
		}

		template <typename TFirstValue, typename ... TValues, class = std::enable_if_t<(sizeof...(TValues) == DIMENSIONS - 1) && std::conjunction<std::is_constructible<TSpace, TFirstValue>, std::is_constructible<TSpace, TValues> ...>::value>>
		constexpr explicit vector(const TFirstValue & first_value, const TValues & ... values)
			: m_elements{ TSpace(first_value), TSpace(values) ... }
		{
		}

		template <typename ... TValues, class = std::enable_if_t<(sizeof...(TValues) > 0) && (sizeof ... (TValues) < DIMENSIONS)>>
		explicit vector(const vector<TSpace, DIMENSIONS - sizeof ... (TValues)> & vec, const TValues & ... values)
		{
			iterate<0, DIMENSIONS - sizeof ... (values) - 1>([&](size_t i) { m_elements[i] = vec[i]; });

			for (size_t i = 0; i < dimensions() - sizeof ... (values); ++i)
			{
				m_elements[i] = vec[i];
			}

			setValues(m_elements + DIMENSIONS - sizeof ... (values), values ...);
		}

		constexpr const TSpace & operator[](std::size_t index) const
		{
			return m_elements[index];
		}

		TSpace & operator[](std::size_t index)
		{
			return m_elements[index];
		}

		vector<TSpace, DIMENSIONS - 1> reduce() const
		{
			return vector<TSpace, DIMENSIONS - 1>(m_elements);
		}

	private:
		TSpace m_elements[DIMENSIONS];
	};


}