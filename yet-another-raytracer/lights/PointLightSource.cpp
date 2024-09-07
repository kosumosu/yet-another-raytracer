#include "PointLightSource.h"
#include "LightingContext.h"
#include "color/color_functions.hpp"

namespace lights
{
	using lighting_functional_distribution = FunctionalDistribution<std::optional<light_sample>, vector3, space_real>;

	PointLightSource::PointLightSource()
		: m_position(vector3::zero())
		, m_color(color_rgb::zero())
		, m_attenuation(1.0f, 0.0f, 0.0f) {}


	void PointLightSource::DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const noexcept
	{
		DoWithDistribution(context.getPoint(), sampler, job);
	}

	void PointLightSource::DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
		const distibution_func& job) const noexcept
	{
		const auto point_to_light = m_position - point;

		auto distance = math::length(point_to_light);
		auto direction = point_to_light / distance;

		job(lighting_functional_distribution(
				1U,
				false,
				[&]
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

	color_real PointLightSource::GetApproximateTotalPower() const noexcept
	{
		return color::get_importance(m_color) * color_real(4.0 * math::pi);
	}
}