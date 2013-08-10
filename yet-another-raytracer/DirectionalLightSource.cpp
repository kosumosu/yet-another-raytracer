#include "DirectionalLightSource.h"

#include <limits>

DirectionalLightSource::DirectionalLightSource(void)
	: m_direction(vector3())
	, m_color(color4())
{
}


DirectionalLightSource::~DirectionalLightSource(void)
{
}


FluxCollection * DirectionalLightSource::GetFluxes( const vector3 & point, const vector3 & normal ) const
{
	FluxCollection * collection = new FluxCollection();

	if (math::dot(m_direction, normal) >= 0.0f )
	{
		Flux flux(this, m_direction, m_color, std::numeric_limits<space_real>::max());
		collection->push_back(flux);
	}

	return collection;
}
