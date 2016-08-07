#include "DirectionalLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"

#include <limits>
#include "LightingContext.h"

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

DirectionalLightSource::DirectionalLightSource(void)
	: m_direction(vector3())
	  , m_color(color_rgbx()) {}


DirectionalLightSource::~DirectionalLightSource(void) {}

void DirectionalLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	if (math::dot(m_direction, context.getNormal()) >= 0.0f && !rayEvaluator.DoesIntersect(ray3(context.getPoint(), m_direction), context.getBias(), std::numeric_limits<space_real>::max()))
	{
		Flux flux(this, m_direction, m_color, std::numeric_limits<space_real>::max(), 1.0);
		job(flux);
	}
}

void DirectionalLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	if (math::dot(m_direction, context.getNormal()) >= 0.0f)
	{
		job(lighting_functional_distribution(
				1U,
				[&](const lighting_functional_distribution::delta_func & subJob)
				{
					subJob(math::random_sample<const light_sample, space_real>(
						light_sample(
							m_direction,
							std::numeric_limits<space_real>::max(),
							[&]()
							{
								return m_color;
							}
						),
						space_real(1.0),
						true));
				},
				[&]()
				{
					return math::random_sample<const light_sample, space_real>(
						light_sample(
							m_direction,
							std::numeric_limits<space_real>::max(),
							[=]()
							{
								return m_color;
							}
						),
						space_real(1.0),
						true);
				})
		);
	}
	else
	{
		job(lighting_functional_distribution());
	}
}
