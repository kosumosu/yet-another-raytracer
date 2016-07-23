#pragma once

#include <type_traits>

namespace utility
{
	template <typename TValue, typename TIter, class = typename std::enable_if<std::is_assignable<TValue, typename TIter::value_type>::value>::type>
	class transform_iterator
	{
	public:
		static constexpr transform_iterator end() { return transform_iterator(); }

		explicit transform_iterator(TIter underlyingIter)
			: _underlyingIter(underlyingIter) {
		}

	private:
		constexpr transform_iterator()
		{
		}

		TIter _underlyingIter;
	};

}