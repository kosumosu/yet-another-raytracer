#include "PointLightSource.h"
#include "RayEvaluator.h"


PointLightSource::PointLightSource(void)
	: m_attenuation(1.0f, 0.0f, 0.0f)
{
}


PointLightSource::~PointLightSource(void)
{
}

FluxCollection * PointLightSource::GetFluxes( const vector3 & point, const vector3 & normal, const RayEvaluator & rayEvaluator, unsigned int depthLeft, space_real bias, bool allowSubdivision) const
{
	FluxCollection * collection = new FluxCollection();
	//collection->reserve(1);

	auto point_to_light = m_position - point;

	if (math::dot(point_to_light, normal) >= space_real(0.0) )
	{
		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;
		if (!rayEvaluator.DoesIntersect(Ray(point, direction), bias, distance))
		{
			auto attenuated_color = m_color * m_attenuation.Evaluate(distance);
			Flux flux(this, direction, attenuated_color, distance, 1.0);

			collection->push_back(flux);
		}
	}

	return collection;
}
