#pragma once

#include <algorithm>

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
	template<size_t, typename T>
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
	void iterate(const T & func)
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

	template <typename TInt, typename TFloat>
	TInt fast_floor(const TFloat & float_value)
	{
		static_assert(std::is_floating_point<TFloat>::value, "Argument must be floating point!");
		static_assert(std::is_integral<TInt>::value && !std::is_same<TInt, bool>::value, "Return value must be of integral type but not bool.");
		return float_value >= TFloat(0) ? static_cast<TInt>(float_value) : static_cast<TInt>(float_value) - TInt(1);
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
}