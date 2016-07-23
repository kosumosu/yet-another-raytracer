#include "SkyLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

SkyLightSource::SkyLightSource(void)
	: m_samples(20)
	, m_color(color_real(0.5))
{
}


SkyLightSource::~SkyLightSource(void)
{
}

void SkyLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	unsigned int actualSamples = context.getAllowSubdivision() ? m_samples : 1;
	color_real sampleWeight = color_real(1.0 / actualSamples);

	for (unsigned int i = 0; i < actualSamples; i++)
	{
		auto direction = math::hemiSphericalRand(context.getNormal(), randomEngine);
		if (!rayEvaluator.DoesIntersect(Ray(context.getPoint(), direction), context.getBias(), std::numeric_limits<space_real>::max()))
		{
			Flux flux(this, direction, m_color * sampleWeight, std::numeric_limits<space_real>::max(), 0.5);
			job(flux);
		}
	}
}
