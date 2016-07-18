#include "DirectionalLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"

#include <limits>

DirectionalLightSource::DirectionalLightSource(void)
	: m_direction(vector3())
	, m_color(color_rgbx())
{
}


DirectionalLightSource::~DirectionalLightSource(void)
{
}

void DirectionalLightSource::IterateOverFluxes(const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned depthLeft, space_real bias, bool allowSubdivision, const flux_func & job) const
{
	if (math::dot(m_direction, normal) >= 0.0f && !rayEvaluator.DoesIntersect(Ray(point, m_direction), bias, std::numeric_limits<space_real>::max()))
	{
		Flux flux(this, m_direction, m_color, std::numeric_limits<space_real>::max(), 1.0);
		job(flux);
	}
}
