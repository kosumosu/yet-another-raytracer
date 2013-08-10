#include "LightingServer.h"


LightingServer::LightingServer( const Raytracer * raytracer )
	: m_raytracer(raytracer)
	, m_shadows_enabled(true)
{

}


LightingServer::~LightingServer(void)
{
}

FluxCollection * LightingServer::GetFluxesAtPoint( const vector3 & point, const vector3 & normal, space_real bias ) const
{
	FluxCollection * filtered_fluxes = new FluxCollection(); // to reduce allocations 

	for (auto & light : *m_lights)
	{
		FluxCollection * fluxes(light->GetFluxes(point, normal));

		if (m_shadows_enabled)
		{
			for (auto & flux : *fluxes)
			{
				bool hit = m_raytracer->DoesIntersect(Ray(point, flux.direction()), bias, flux.distance());
				if (!hit)
				{
					filtered_fluxes->push_back(flux);
				}
			}
		}
		else
		{
			filtered_fluxes->insert(filtered_fluxes->end(), fluxes->begin(), fluxes->end());
		}

		delete fluxes;
	}

	return filtered_fluxes;
}
