#include "PointLightSource.h"
#include "LightingContext.h"
#include "color_functions.hpp"

using lighting_functional_distribution = FunctionalDistribution<std::optional<light_sample>, vector3, space_real>;

PointLightSource::PointLightSource()
	: m_position(vector3::zero())
	, m_color(color_rgbx::zero())
	, m_attenuation(1.0f, 0.0f, 0.0f) {}


void PointLightSource::DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const
{
	const auto point_to_light = m_position - context.getPoint();

	if (math::dot(point_to_light, context.getNormal()) >= space_real(0.0))
	{
		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;

		job(lighting_functional_distribution(
				1U,
				[&]()
				{
					return math::random_sample<std::optional<light_sample>, space_real>(
						light_sample(
							direction,
							distance,
							[=, this]()
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

color_real PointLightSource::GetApproximateTotalPower() const
{
	return color::get_importance(m_color) * color_real(4.0 * math::pi);
}
