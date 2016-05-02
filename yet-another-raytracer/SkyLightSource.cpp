#include "SkyLightSource.h"
#include "RayEvaluator.h"

SkyLightSource::SkyLightSource(void)
	: m_samples(20)
	, m_color(color_real(0.5))
{
}


SkyLightSource::~SkyLightSource(void)
{
}

FluxCollection * SkyLightSource::GetFluxes( const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const
{
	FluxCollection * fluxes = new FluxCollection();

	unsigned int actualSamples = allowSubdivision ? m_samples : 1;
	color_real sampleWeight = color_real(1.0 / actualSamples);

	fluxes->reserve(actualSamples);

	for (unsigned int i = 0; i < actualSamples; i++)
	{
		auto direction = math::hemiSphericalRand(normal);
		if (!rayEvaluator.DoesIntersect(Ray(point, direction), bias, std::numeric_limits<space_real>::max()))
		{
			Flux flux(this, direction, m_color * sampleWeight, std::numeric_limits<space_real>::max(), 0.5);
			fluxes->push_back(flux);
		}
	}
	return fluxes;
}
