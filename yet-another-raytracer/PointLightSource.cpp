#include "PointLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

PointLightSource::PointLightSource(void)
	: m_attenuation(1.0f, 0.0f, 0.0f)
{
}


PointLightSource::~PointLightSource(void)
{
}

void PointLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	auto point_to_light = m_position - context.getPoint();

	if (math::dot(point_to_light, context.getNormal()) >= space_real(0.0))
	{
		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;
		if (!rayEvaluator.DoesIntersect(Ray(context.getPoint(), direction), context.getBias(), distance))
		{
			auto attenuated_color = m_color * m_attenuation.Evaluate(distance);
			Flux flux(this, direction, attenuated_color, distance, 1.0);

			job(flux);
		}
	}

}
