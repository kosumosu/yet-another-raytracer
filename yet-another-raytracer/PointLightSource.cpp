#include "PointLightSource.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "LightingContext.h"

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

PointLightSource::PointLightSource(void)
	: m_attenuation(1.0f, 0.0f, 0.0f) {}


PointLightSource::~PointLightSource(void) {}

void PointLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	auto point_to_light = m_position - context.getPoint();

	if (math::dot(point_to_light, context.getNormal()) >= space_real(0.0))
	{
		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;
		if (!rayEvaluator.DoesIntersect(ray3(context.getPoint(), direction), context.getBias(), distance))
		{
			auto attenuated_color = m_color * m_attenuation.Evaluate(distance);
			Flux flux(this, direction, attenuated_color, distance, 1.0);

			job(flux);
		}
	}
}

void PointLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	auto point_to_light = m_position - context.getPoint();

	if (math::dot(point_to_light, context.getNormal()) >= space_real(0.0))
	{
		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;

		job(lighting_functional_distribution(
				1U,
				[&](const lighting_functional_distribution::delta_func & subJob)
				{
					subJob(math::random_sample<const light_sample, space_real>(
						light_sample(
							direction,
							distance,
							[&]()
							{
								return m_color * m_attenuation.Evaluate(distance);
							}
						),
						space_real(1.0),
						true));
				},
				[&]()
				{
					return math::random_sample<const light_sample, space_real>(
						light_sample(
							direction,
							distance,
							[=]()
							{
								return m_color * m_attenuation.Evaluate(distance);
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
