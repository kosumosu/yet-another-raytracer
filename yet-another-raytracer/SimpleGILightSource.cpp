#include "SimpleGILightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

SimpleGILightSource::SimpleGILightSource(bool includeEmission, size_t sampleCount)
	: _samples(sampleCount)
	, _includeEmission(includeEmission)
{
	
}

SimpleGILightSource::~SimpleGILightSource(void)
{
}

void SimpleGILightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	if (context.getDepthLeft() >= 1)
	{
		size_t actualSamples = context.getAllowSubdivision() ? _samples : 1;
		color_real sampleWeight = color_real(1.0 / actualSamples);


		for (size_t i = 0; i < actualSamples; i++)
		{
#if true
			auto direction = math::cosineWeightedHemiSphericalRand(context.getNormal(), randomEngine);
			const color_real pdf = color_real(math::dot(direction, context.getNormal()) * space_real(math::oneOverPi));
#else
			auto direction = math::hemiSphericalRand(normal);
			const color_real pdf = color_real(0.5 * math::oneOverPi);
#endif

			auto color = rayEvaluator.TraceRay(ray3(context.getPoint(), direction), context.getDepthLeft(), context.getBias(), false, _includeEmission);
			if (color != color_rgbx())
			{
				Flux flux(this, direction, color * sampleWeight, std::numeric_limits<space_real>::max(), pdf);
				job(flux);
			}
		}
	}
}
