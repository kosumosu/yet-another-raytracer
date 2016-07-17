#include "LightingServer.h"
#include "RayEvaluator.h"
#include "Raytracer.h"


LightingServer::LightingServer()
	: m_shadows_enabled(false)
	, m_lights(nullptr)
{

}


LightingServer::~LightingServer(void)
{
}

FluxCollection * LightingServer::GetFluxesAtPoint(const vector3 & point, const vector3 & normal, space_real bias, unsigned int depthLeft, const RayEvaluator& rayEvaluator, bool allowSubdivision) const
{
	FluxCollection * filtered_fluxes = new FluxCollection(); // to reduce allocations

	for (auto & light : *m_lights)
	{
		FluxCollection * fluxes(light->GetFluxes(point, normal, rayEvaluator, depthLeft, bias, allowSubdivision));


		filtered_fluxes->insert(filtered_fluxes->end(), fluxes->begin(), fluxes->end());

		delete fluxes;
	}

	return filtered_fluxes;
}
