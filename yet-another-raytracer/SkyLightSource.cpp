#include "SkyLightSource.h"
#include "LightingContext.h"

#define ENABLE_IMPORTANCE_SAMPLING true

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

SkyLightSource::SkyLightSource()
	: m_samples(20)
	, m_color(color_rgbx::fill(0.5)) {}



void SkyLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	job(lighting_functional_distribution(
		[&]()
		{
#if ENABLE_IMPORTANCE_SAMPLING
			auto direction = math::cosineWeightedHemiSphericalRand(context.getNormal(), randomEngine);
			const space_real pdf = math::dot(direction, context.getNormal()) * space_real(math::oneOverPi);
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
#if ENABLE_IMPORTANCE_SAMPLING
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
