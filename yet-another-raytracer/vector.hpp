#pragma once

#include "helper.hpp"
#include <type_traits>
#include <utility>
#include <array>

namespace math
{
	template<typename TSpace, size_t DIMENSIONS, class Enable = void/*, typename TIndexSeq = my_make_index_sequence<DIMENSIONS>*/>
	class vector {};

	// Represents vector in space of TSpace^DIMENSIONS.
	template<typename TSpace, size_t DIMENSIONS/*, std::size_t... Indices2*/>
	class vector<TSpace, DIMENSIONS, std::enable_if_t<(DIMENSIONS > 0)>/*, std::index_sequence<Indices2...>*/>
	{
	public:

		using _Myt = vector<TSpace, DIMENSIONS, void>;

		constexpr static _Myt fill(const TSpace & value) { return vector<TSpace, DIMENSIONS>(value, std::make_index_sequence<DIMENSIONS>()); }
		constexpr static _Myt zero() { return fill(0); }

		constexpr static size_t dimensions() { return DIMENSIONS; }

		constexpr vector(const _Myt & other) //noexcept(noexcept(vector(other, std::make_index_sequence<DIMENSIONS>())))
			: vector(other, std::make_index_sequence<DIMENSIONS>())
		{
		}

		template <typename TOther, class = std::enable_if_t<std::is_constructible<TSpace, TOther>::value>>
		constexpr vector(const vector<TOther, DIMENSIONS> & other) //noexcept(noexcept(vector(other, std::make_index_sequence<DIMENSIONS>())))
			: vector(other, std::make_index_sequence<DIMENSIONS>())
		{
		}

		_Myt & operator=(const _Myt & other)
		{
			if (this == &other)
				return *this;

			m_elements = other.m_elements;

			return *this;
		}

		constexpr vector(const _Myt && other) //noexcept(noexcept(vector(std::move(other), std::make_index_sequence<DIMENSIONS>())))
			: vector(std::move(other), std::make_index_sequence<DIMENSIONS>())
		{
		}

		_Myt & operator=(_Myt && other) noexcept(noexcept(std::swap(m_elements, other.m_elements)))
		{
			if (this == &other)
				return *this;

			std::swap(m_elements, other.m_elements);

			return *this;
		}

		template <typename TOther>
		constexpr explicit vector(const TOther (&other)[DIMENSIONS]) //noexcept(noexcept(vector(other, std::make_index_sequence<DIMENSIONS>())))
			: vector(other, std::make_index_sequence<DIMENSIONS>())
		{
		}

		//template <typename TValue, class = std::enable_if_t<(DIMENSIONS > 1)>>
		//constexpr explicit vector(const TValue & value)
		//	: vector(value, std::make_index_sequence<DIMENSIONS>())
		//{

		//}

		template <typename TFirstValue, typename ... TValues,
			class = std::enable_if_t<
				(sizeof...(TValues) == DIMENSIONS - 1)
				&& std::conjunction<std::is_constructible<TSpace, TFirstValue>, std::is_constructible<TSpace, TValues> ...>::value
			>
		>
		constexpr vector(TFirstValue && first_value, TValues && ... values)
			: m_elements{ { TSpace(std::forward<TFirstValue>(first_value)), TSpace(std::forward<TValues>(values)) ... } }
		{
		}

		template <typename ... TValues, class = std::enable_if_t<(sizeof...(TValues) > 0) && (sizeof...(TValues) < DIMENSIONS)>>
		constexpr explicit vector(const vector<TSpace, DIMENSIONS - sizeof... (TValues)> & vec, const TValues &... values)
			: vector(std::make_index_sequence<DIMENSIONS - sizeof... (TValues)>(), vec, values...)
		{
		}

		constexpr const TSpace & operator[](std::size_t index) const
		{
			return m_elements[index];
		}

		TSpace & operator[](std::size_t index)
		{
			return m_elements[index];
		}

		constexpr vector<TSpace, DIMENSIONS - 1> reduce() const
		{
			return reduce_impl(std::make_index_sequence<DIMENSIONS - 1>());
		}

	private:
		std::array<TSpace, DIMENSIONS> m_elements;

		template <typename TOther, std::size_t... Indices>
		constexpr vector(const vector<TOther, DIMENSIONS> & other, std::index_sequence<Indices...>)
			: m_elements{ { TSpace(other[Indices])... } }
		{
		}

		template <std::size_t... Indices>
		constexpr vector(const _Myt && other, std::index_sequence<Indices...>) //noexcept(std::is_nothrow_move_constructible<TSpace>::value)
			: m_elements{ {std::move(other[Indices])... } }
		{
		}

		template <typename TOther, std::size_t... Indices>
		constexpr explicit vector(const TOther (&other)[DIMENSIONS], std::index_sequence<Indices...>)
			: m_elements{ {TSpace(other[Indices])...} }
		{
		}

		template <typename TValue, std::size_t... Indices>
		constexpr explicit vector(const TValue & value, std::index_sequence<Indices...>)
			: m_elements{ {TSpace((static_cast<void>(Indices), value))... } } // trick to repeat same value. static_cast<void>(Indices) is for safety
			//: m_elements{ TSpace(value)... } // trick to repeat same value. static_cast<void>(Indices) is for safety
		{
		}

		template <typename ... TValues, std::size_t... VecIndices>
		constexpr explicit vector(std::index_sequence<VecIndices...>, const vector<TSpace, DIMENSIONS - sizeof...(TValues)> & vec, const TValues &... values)
			: m_elements{ { TSpace(vec[VecIndices])..., TSpace(values)...} }
		{
		}


		template <std::size_t... ReducedIndices>
		constexpr vector<TSpace, DIMENSIONS - 1> reduce_impl(std::index_sequence<ReducedIndices...>) const
		{
			return vector<TSpace, DIMENSIONS - 1>(m_elements[ReducedIndices] ...);
		}
	};


}
