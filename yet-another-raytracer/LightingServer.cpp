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

void LightingServer::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned> & randomEngine, const flux_func & job) const
{
	for (const auto & light : *m_lights)
	{
		light->IterateOverFluxes(context, rayEvaluator, randomEngine, job);
	}
}
