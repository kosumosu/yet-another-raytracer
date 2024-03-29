#pragma once

#include "random_sample.hpp"
#include <vector>
#include <algorithm>

namespace math
{
	template <class TElement, class TWeight, class TPdf = TWeight>
	class discrete_distribution
	{
	public:
		discrete_distribution()
			: _weightSum(0)
		{
		}

		template <
			class TIterator,
			class = std::enable_if_t<std::is_assignable<std::pair<TElement, TWeight>, typename TIterator::value_type>::value>
		>
		discrete_distribution(TIterator begin, TIterator end)
		{
			const auto size = std::distance(begin, end);

			_cdf.resize(size);
			TWeight accumulator = begin->second;
			*std::begin(_cdf) = accumulator;

			std::transform(std::next(begin), end, std::next(std::begin(_cdf)), [&](const std::pair<TElement, TWeight> & element)
			{
				return accumulator += element.second;
			});

			_elements.resize(size);
			std::transform(begin, end, std::begin(_elements), [&](const std::pair<TElement, TWeight> & element)
			{
				return element.first;
			});

			_weightSum = accumulator;
		}

		template <typename TGenerator>
		[[nodiscard]] random_sample<TElement, TPdf> GetRandomElement(TGenerator& rng) const
		{
			TWeight randomValue = rng() * _weightSum;
			auto weightIterator = std::lower_bound(std::begin(_cdf), std::end(_cdf), randomValue);
			auto iteratorDistance = std::distance(std::begin(_cdf), weightIterator);

			auto weight = weightIterator != std::begin(_cdf)
				? *weightIterator - *std::prev(weightIterator)
				: *weightIterator;

			return random_sample<TElement, TPdf>(*std::next(std::begin(_elements), iteratorDistance), weight / _weightSum, false);
		}

	private:
		TWeight _weightSum;
		std::vector<TWeight> _cdf; // cumulative distribution function
		std::vector<TElement> _elements;
	};
}