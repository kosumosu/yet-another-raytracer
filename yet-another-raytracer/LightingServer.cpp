#include "LightingServer.h"
#include "RayEvaluator.h"


LightingServer::LightingServer()
	: m_shadows_enabled(false)
	, m_lights(nullptr)
{

}


LightingServer::~LightingServer(void)
{
}

void LightingServer::IterateOverFluxes(const vector3 & point, const vector3 & normal, space_real bias, unsigned depthLeft, const RayEvaluator & rayEvaluator, bool allowSubdivision, const flux_func & job) const
{
	for (const auto & light : *m_lights)
	{
		light->IterateOverFluxes(point, normal, rayEvaluator, depthLeft, bias, allowSubdivision, job);
	}
}
