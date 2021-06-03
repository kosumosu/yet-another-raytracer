#include "SkyLightSource.h"
#include "LightingContext.h"

#define ENABLE_COSINE_WEIGHTED_SAMPLING true

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

SkyLightSource::SkyLightSource()
	: m_samples(20)
	, m_color(color_rgbx::fill(0.5)) {}



void SkyLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<random_int_t> & randomEngine, const distibution_func & job) const
{
	job(lighting_functional_distribution(
		[&]()
		{
#if ENABLE_COSINE_WEIGHTED_SAMPLING
			const auto [direction, pdf] = math::cosineWeightedHemiSphericalRand(context.getNormal(), randomEngine);
#else
			auto direction = math::hemiSphericalRand(normal);
			const space_real pdf = space_real(0.5 * math::oneOverPi);
#endif

			return math::random_sample<const light_sample, space_real>(
				light_sample(
					direction,
					std::numeric_limits<space_real>::max(),
					[=]()
					{
						return m_color;
					}
				),
				pdf,
				false);
		},
		[&](const vector3 & sample)
		{
#if ENABLE_COSINE_WEIGHTED_SAMPLING
			return math::dot(sample, context.getNormal()) * space_real(math::oneOverPi);
#else
			return space_real(0.5 * math::oneOverPi);
#endif
		}
	));
}

color_real SkyLightSource::GetApproximateTotalPower() const
{
	throw std::exception{};
}
