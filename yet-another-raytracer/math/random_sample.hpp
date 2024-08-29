#pragma once
#include <type_traits>

namespace math
{
	template <typename TValue, typename TPdf>
	class random_sample
	{
	public:
		constexpr random_sample(const TValue & value, const TPdf & pdf, bool isDelta)
			: _value(value)
			, _pdf(pdf)
			, _isDelta(isDelta)
		{
		}

		constexpr random_sample(TValue && value, TPdf && pdf, bool isDelta)
			: _value(std::move(value))
			, _pdf(std::move(pdf))
			, _isDelta(isDelta)
		{
		}

		template <typename TOtherValue, typename TOtherPdf, class = std::enable_if_t<std::is_constructible<TOtherValue, TValue>::value && std::is_constructible<TPdf, TOtherPdf>::value>>
		//template <typename TOtherValue, typename TOtherPdf>
		constexpr random_sample(const random_sample<TOtherValue, TOtherPdf> & other)
			: _value(other._pdf)
			, _pdf(other._pdf)
			, _isDelta(other._isDelta)
		{
		}

		template <typename TOtherValue, typename TOtherPdf, class = std::enable_if_t<std::is_constructible<TOtherValue, TValue>::value && std::is_constructible<TPdf, TOtherPdf>::value>>
		//template <typename TOtherValue, typename TOtherPdf>
		constexpr random_sample(random_sample<TOtherValue, TOtherPdf> && other)
			: _value(std::move(other._value))
			, _pdf(std::move(other._pdf))
			, _isDelta(other._isDelta)
		{
		}

		TValue & getValue()
		{
			return _value;
		}

		constexpr const TValue & getValue() const
		{
			return _value;
		}

		TPdf & getPdf()
		{
			return _pdf;
		}

		constexpr const TPdf & getPdf() const
		{
			return _pdf;
		}

		constexpr bool getIsDelta() const
		{
			return _isDelta;
		}

	private:
		TValue _value;
		TPdf _pdf;
		bool _isDelta;
	};
}
