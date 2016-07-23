#pragma once
#include <type_traits>

namespace math
{
	template <typename TValue, typename TPdf>
	class random_sample
	{
	public:
		constexpr random_sample(const TValue & value, const TPdf & pdf)
			: value(value)
			, pdf(pdf)
		{
		}

		constexpr random_sample(TValue && value, TPdf && pdf)
			: value(std::move(value))
			, pdf(std::move(pdf))
		{
		}


		TValue & getValue()
		{
			return value;
		}

		constexpr const TValue & getValue() const
		{
			return value;
		}

		TPdf & getPdf()
		{
			return pdf;
		}

		constexpr const TPdf & getPdf() const
		{
			return pdf;
		}

	private:
		TValue value;
		TPdf pdf;
	};
}
