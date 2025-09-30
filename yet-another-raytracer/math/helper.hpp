#pragma once

#include <algorithm>
#include <optional>
#include <cmath>

#define ENABLE_TEMPLATE_LOOP false

namespace math
{
	namespace details
	{
		template <std::size_t N, std::size_t... Indices>
		struct my_make_index_sequence_struct {
			using type = typename my_make_index_sequence_struct<N - 1, N - 1, Indices...>::type;
		};

		template <std::size_t... Indices>
		struct my_make_index_sequence_struct<0, Indices...> {
			using type = std::index_sequence<Indices...>;
		};
	}

	// helper analog to std::make_index_sequence<N>. Used to workaround a bug with __make_integer_seq
	template <std::size_t N>
	using my_make_index_sequence = typename details::my_make_index_sequence_struct<N>::type;

	// helper to unfold index pack into repeating T pack
	template<std::size_t, typename T>
	using get_type = T;

#if ENABLE_TEMPLATE_LOOP
	namespace details
	{
		template<int FROM, int TO, typename T>
		std::enable_if_t<FROM == TO>
			iterate_sfinae(const T & func)
		{
			func(FROM);
		}

		template<int FROM, int TO, typename T>
		std::enable_if_t<(FROM > TO)>
			iterate_sfinae(const T & func)
		{
		}

		template<int FROM, int TO, typename T>
		std::enable_if_t<(FROM < TO)>
			iterate_sfinae(const T & func)
		{
			func(FROM);
			iterate_sfinae<FROM + 1, TO>(func);
		}
	}

	// Implements static loop with templates. It was faster at some moment.
	template<int FIRST, int LAST, typename T>
	void iterate(const T & func)
	{
		details::iterate_sfinae<FIRST, LAST, T>(func);
	}
#else
	template<int FIRST, int LAST, typename T>
	constexpr void iterate(const T & func)
	{
		for (int i = FIRST; i <= LAST; ++i)
		{
			func(i);
		}
	}
#endif

	template <typename TArray, typename TFirst, typename ... TRest>
	void setValues(TArray array[], const TFirst & first, const TRest & ... rest)
	{
		array[0] = TArray(first);
		setValues(array + 1, rest ...);
	}

	template <typename TArray, typename TFirst>
	void setValues(TArray array[], const TFirst & first)
	{
		array[0] = TArray(first);
	}


	template <typename T>
	T saturate(const T & value, const T & min_value, const T & max_value)
	{
		return std::min(max_value, std::max(min_value, value));
	}

	template<typename T>
	bool is_same_sign(const T & a, const T & b)
	{
		static_assert(std::is_floating_point<T>::value, "Argument must be floating point!");
		return std::signbit(a) == std::signbit(b);
	}

	template <typename T>
	T sign(T val)
	{
		static_assert(std::is_floating_point<T>::value, "Argument must be floating point!");
		return std::copysign(T(1.0), val);
		//return val >= 0 ? T(1) : T(-1);
	}

	template <class T>
	T square(const T& a)
	{
		return a * a;
	}

	template <class T>
	T safeSqrt(const T& a)
	{
		return std::sqrt(std::max(T(0), a));
	}

	/*
	  diff_of_products() computes a*b-c*d with a maximum error <= 1.5 ulp

	  Claude-Pierre Jeannerod, Nicolas Louvet, and Jean-Michel Muller,
	  "Further Analysis of Kahan's Algorithm for the Accurate Computation
	  of 2x2 Determinants". Mathematics of Computation, Vol. 82, No. 284,
	  Oct. 2013, pp. 2245-2264
	*/
	template<class T>
	T diffOfProducts(T a, T b, T c, T d) {
		auto w = d * c;
		auto e = std::fma(-d, c, w);
		auto f = std::fma(a, b, -w);
		return f + e;
	}

	template<class T>
	std::optional<std::pair<T, T>> solveQuadratic(const T &a, const T &b, const T &c) {
		const auto discriminant = diffOfProducts(b, b, T(4.0) * a, c);
		if (discriminant < 0) {
			return std::nullopt;
		}
		const auto q = -T(0.5) * (b + std::copysign(std::sqrt(discriminant), b));

		return std::make_pair(q / a, c / q);
	}
}