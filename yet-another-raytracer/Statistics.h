#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>

namespace statistics::detail
{
	template <class TDerived>
	class Statistics
	{
	public:
		void registerPath(bool carriesLight)
		{
			if (carriesLight)
			{
				derived().registerLightCarryingPathImpl();
			}
			else
			{
				derived().registerNonLightCarryingPathImpl();
			}
		}

		void registerBundle(bool carriesLight)
		{
			if (carriesLight)
			{
				derived().registerLightCarryingBundleImpl();
			}
			else
			{
				derived().registerNonLightCarryingBundleImpl();
			}
		}

		void mergeIn(const TDerived& stats)
		{
			derived().mergeInImpl(stats);
		}

		void printResult(std::ostream& stream) const
		{
			derived().printResultImpl(stream);
		}

	private:
		[[nodiscard]] TDerived& derived()
		{
			return *static_cast<TDerived*>(this);
		}

		[[nodiscard]] const TDerived& derived() const
		{
			return *static_cast<const TDerived*>(this);
		}
	};

	class NopStatistics final : public Statistics<NopStatistics>
	{
		friend class Statistics<NopStatistics>;
	protected:
		void registerLightCarryingPathImpl()
		{
		}

		void registerNonLightCarryingPathImpl()
		{
		}

		void registerLightCarryingBundleImpl()
		{
		}

		void registerNonLightCarryingBundleImpl()
		{
		}

		void mergeInImpl(const NopStatistics& stats)
		{
		}

		void printResultImpl(std::ostream& /*stream*/) const
		{
		}
	};

	class ActualStatistics final : public Statistics<ActualStatistics>
	{
		friend class Statistics<ActualStatistics>;
		std::size_t lightCarryingPaths_ = 0;
		std::size_t nonLightCarryingPaths_ = 0;
		std::size_t lightCarryingBundles_ = 0;
		std::size_t nonLightCarryingBundles_ = 0;
	protected:
		void registerLightCarryingPathImpl()
		{
			++lightCarryingPaths_;
		}

		void registerNonLightCarryingPathImpl()
		{
			++nonLightCarryingPaths_;
		}

		void registerLightCarryingBundleImpl()
		{
			++lightCarryingBundles_;
		}

		void registerNonLightCarryingBundleImpl()
		{
			++nonLightCarryingBundles_;
		}

		void mergeInImpl(const ActualStatistics& stats)
		{
			lightCarryingPaths_ += stats.lightCarryingPaths_;
			nonLightCarryingPaths_ += stats.nonLightCarryingPaths_;
			lightCarryingBundles_ += stats.lightCarryingBundles_;
			nonLightCarryingBundles_ += stats.nonLightCarryingBundles_;
		}

		void printResultImpl(std::ostream& stream) const
		{
			stream << "Stats:" << std::endl;
			stream << "Total paths: " << std::setprecision(3) << double(lightCarryingPaths_ + nonLightCarryingPaths_) / 1'000'000'000.0 << " bln" << std::endl;
			stream << "Total bundles: " << std::setprecision(3) << double(lightCarryingBundles_ + nonLightCarryingBundles_) / 1'000'000'000.0 << " bln" <<
				std::endl;
			stream << "Light carrying paths: " << std::setprecision(3) << double(lightCarryingPaths_) / double(lightCarryingPaths_ + nonLightCarryingPaths_) *
				100.0 << "%" << std::endl;
			stream << "Light carrying bundles: " << std::setprecision(3) << double(lightCarryingBundles_) / double(
				lightCarryingBundles_ + nonLightCarryingBundles_) * 100.0 << "%" << std::endl;
		}
	};
}

namespace statistics
{
#if true
	using Stats = detail::ActualStatistics;
#else
	using Stats = detail::NullStatistics;
#endif
}
