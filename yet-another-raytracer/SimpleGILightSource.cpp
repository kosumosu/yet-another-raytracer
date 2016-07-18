#include "SimpleGILightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"

SimpleGILightSource::SimpleGILightSource(void)
	: m_samples(20)
{
}


SimpleGILightSource::~SimpleGILightSource(void)
{
}

void SimpleGILightSource::IterateOverFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned depthLeft, space_real bias, bool allowSubdivision, const flux_func & job) const
{
	if (depthLeft >= 1)
	{
		unsigned int actualSamples = allowSubdivision ? m_samples : 1;
		color_real sampleWeight = color_real(1.0 / actualSamples);


		for (unsigned int i = 0; i < actualSamples; i++)
		{
#if true
			auto direction = math::cosineWeightedHemiSphericalRand(normal);
			const color_real pdf = color_real(math::dot(direction, normal) * space_real(math::oneOverPi));
#else
			auto direction = math::hemiSphericalRand(normal);
			const color_real pdf = color_real(0.5 * math::oneOverPi);
#endif

			auto color = rayEvaluator.TraceRay(Ray(point, direction), depthLeft, bias, false);
			if (color != color_rgbx())
			{
				Flux flux(this, direction, color * sampleWeight, std::numeric_limits<space_real>::max(), pdf);
				job(flux);
			}
		}
	}
}
