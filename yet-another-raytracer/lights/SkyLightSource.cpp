#include "SkyLightSource.h"
#include "LightingContext.h"

#define ENABLE_IMPORTANCE_SAMPLING true

namespace lights
{
	using lighting_functional_distribution = FunctionalDistribution<std::optional<light_sample>, vector3, space_real>;

	SkyLightSource::SkyLightSource()
		: m_samples(20)
		, m_color(color_rgb::fill(0.5)) {}



	void SkyLightSource::DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const noexcept
	{
		// 	auto evaluatePdf = [&](const vector3& sample)
		// 	{
		// #if ENABLE_IMPORTANCE_SAMPLING
		// 		return math::dot(sample, context.getNormal()) * space_real(math::oneOverPi);
		// #else
		// 		return space_real(0.5 * math::oneOverPi);
		// #endif
		// 	};

		job(lighting_functional_distribution(
			0U,
			true,
			[&]
			{
	#if ENABLE_IMPORTANCE_SAMPLING
				const auto [direction, pdf] = math::cosineWeightedHemiSphericalRand(context.getNormal(), sampler);
	#else
				auto direction = math::hemiSphericalRand(normal);
				const space_real pdf = space_real(0.5 * math::oneOverPi);
	#endif

				return math::random_sample<std::optional<light_sample>, space_real>(
					light_sample(
						direction,
						std::numeric_limits<space_real>::max(),
						[=, this]()
						{
							return m_color;
						}
					),
					pdf,
					false);
			}
		));
	}

	void SkyLightSource::DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
	                                        const distibution_func& job) const noexcept
	{
		job(lighting_functional_distribution(
			0U,
			true,
			[&]
			{
#if ENABLE_IMPORTANCE_SAMPLING
				const auto direction = math::sphericalRand<space_real>(sampler);
				constexpr auto pdf = space_real(0.25) * std::numbers::inv_pi_v<space_real>;
#else
			auto direction = math::hemiSphericalRand(normal);
			const space_real pdf = space_real(0.5 * math::oneOverPi);
#endif

				return math::random_sample<std::optional<light_sample>, space_real>(
					light_sample(
						direction,
						std::numeric_limits<space_real>::max(),
						[=, this]()
						{
							return m_color;
						}
					),
					pdf,
					false);
			}
		));
	}

	color_real SkyLightSource::GetApproximateTotalPower() const noexcept
	{
		throw std::exception{};
	}
}