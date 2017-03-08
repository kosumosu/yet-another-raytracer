#include "DirectionalLightSource.h"
#include <limits>
#include "LightingContext.h"

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

DirectionalLightSource::DirectionalLightSource()
	: m_direction(vector3::zero())
	, m_color(color_rgbx::zero()) {}


DirectionalLightSource::~DirectionalLightSource(void) {}

void DirectionalLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	if (true || math::dot(m_direction, context.getNormal()) >= 0.0f)
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
