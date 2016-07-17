#include "SimpleGILightSource.h"
#include "RayEvaluator.h"

#include <iostream>

SimpleGILightSource::SimpleGILightSource(void)
	: m_samples(20)
{
}


SimpleGILightSource::~SimpleGILightSource(void)
{
}

FluxCollection * SimpleGILightSource::GetFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const
{
	FluxCollection * fluxes = new FluxCollection();

	if (depthLeft >= 1)
	{
		unsigned int actualSamples = allowSubdivision ? m_samples : 1;
		color_real sampleWeight = color_real(1.0 / actualSamples);

		fluxes->reserve(actualSamples);

		for (unsigned int i = 0; i < actualSamples; i++)
		{
#if false
			auto direction = math::cosineWeightedHemiSphericalRand(normal);
			const color_real pdf = math::dot(direction, normal) * math::oneOverPi;
#else
			auto direction = math::hemiSphericalRand(normal);
			const color_real pdf = color_real(0.5 * math::oneOverPi);
#endif
			 
			auto color = rayEvaluator.TraceRay(Ray(point, direction), depthLeft, bias, false);
			if (color != color_rgbx())
			{
				Flux flux(this, direction, color * sampleWeight, std::numeric_limits<space_real>::max(), pdf);
				fluxes->push_back(flux);
			}
		}
	}
	return fluxes;
}
