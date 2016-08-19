#pragma once

#include <algorithm>

#define ENABLE_TEMPLATE_LOOP false

namespace math
{
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