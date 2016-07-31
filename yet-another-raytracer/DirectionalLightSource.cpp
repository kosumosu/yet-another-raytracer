#include "DirectionalLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"

#include <limits>
#include "LightingContext.h"

DirectionalLightSource::DirectionalLightSource(void)
	: m_direction(vector3())
	, m_color(color_rgbx())
{
}


DirectionalLightSource::~DirectionalLightSource(void)
{
}

void DirectionalLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	if (math::dot(m_direction, context.getNormal()) >= 0.0f && !rayEvaluator.DoesIntersect(ray3(context.getPoint(), m_direction), context.getBias(), std::numeric_limits<space_real>::max()))
	{
		Flux flux(this, m_direction, m_color, std::numeric_limits<space_real>::max(), 1.0);
		job(flux);
	}
}
