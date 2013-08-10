#include "SkyLightSource.h"

#include <glm/gtc/random.hpp>

SkyLightSource::SkyLightSource(void)
	: m_samples(20)
	, m_color(color_real(0.5))
	, m_color_per_sample(m_color / color_real(m_samples))
{
}


SkyLightSource::~SkyLightSource(void)
{
}

FluxCollection * SkyLightSource::GetFluxes( const vector3 & point, const vector3 & normal ) const
{
	FluxCollection * fluxes = new FluxCollection();
	fluxes->reserve(m_samples);

	for (unsigned int i = 0; i < m_samples; i++)
	{
		auto direction = math::hemiSphericalRand(normal);
		if (math::dot(normal, direction) > space_real(0.0))
		{
			Flux flux(this, direction, m_color_per_sample, std::numeric_limits<space_real>::max());
			fluxes->push_back(flux);
		}
	}
	return fluxes;
}
