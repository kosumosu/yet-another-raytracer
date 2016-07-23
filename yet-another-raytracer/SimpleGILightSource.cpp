#include "SimpleGILightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

SimpleGILightSource::SimpleGILightSource(void)
	: m_samples(20)
{
}


SimpleGILightSource::~SimpleGILightSource(void)
{
}

void SimpleGILightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	if (context.getDepthLeft() >= 1)
	{
		unsigned int actualSamples = context.getAllowSubdivision() ? m_samples : 1;
		color_real sampleWeight = color_real(1.0 / actualSamples);


		for (unsigned int i = 0; i < actualSamples; i++)
		{
#if true
			auto direction = math::cosineWeightedHemiSphericalRand(context.getNormal(), randomEngine);
			const color_real pdf = color_real(math::dot(direction, context.getNormal()) * space_real(math::oneOverPi));
#else
			auto direction = math::hemiSphericalRand(normal);
			const color_real pdf = color_real(0.5 * math::oneOverPi);
#endif

			auto color = rayEvaluator.TraceRay(Ray(context.getPoint(), direction), context.getDepthLeft(), context.getBias(), false);
			if (color != color_rgbx())
			{
				Flux flux(this, direction, color * sampleWeight, std::numeric_limits<space_real>::max(), pdf);
				job(flux);
			}
		}
	}
}
