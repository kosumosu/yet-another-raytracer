#include "SkyLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

#define ENABLE_IMPORTANCE_SAMPLING true

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

SkyLightSource::SkyLightSource(void)
	: m_samples(20)
	  , m_color(color_real(0.5)) {}


SkyLightSource::~SkyLightSource(void) {}

void SkyLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	unsigned int actualSamples = context.getAllowSubdivision() ? m_samples : 1;
	color_real sampleWeight = color_real(1.0 / actualSamples);

	for (unsigned int i = 0; i < actualSamples; i++)
	{
#if ENABLE_IMPORTANCE_SAMPLING
		auto direction = math::cosineWeightedHemiSphericalRand(context.getNormal(), randomEngine);
		const color_real pdf = color_real(math::dot(direction, context.getNormal()) * space_real(math::oneOverPi));
#else
		auto direction = math::hemiSphericalRand(normal);
		const color_real pdf = color_real(0.5 * math::oneOverPi);
#endif
		if (!rayEvaluator.DoesIntersect(ray3(context.getPoint(), direction), context.getBias(), std::numeric_limits<space_real>::max()))
		{
			Flux flux(this, direction, m_color * sampleWeight, std::numeric_limits<space_real>::max(), 0.5 * math::oneOverPi);
			job(flux);
		}
	}
}

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
					[&]()
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
