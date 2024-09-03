#include "DirectionalLightSource.h"
#include "LightingContext.h"
#include "Scene.h"
#include "color/color_functions.hpp"
#include <limits>
#include <utility>

namespace lights
{
	using lighting_functional_distribution = FunctionalDistribution<std::optional<light_sample>, vector3, space_real>;


	DirectionalLightSource::DirectionalLightSource(vector3 direction, color_rgb color, const Scene & scene)
		: _direction(std::move(direction))
		, _color(std::move(color))
	{
		bounding_box3 scene_box;
		for (const auto & object : scene.objects())
		{
			scene_box.Include(object->bounding_box());
		}

		const auto BoundingSphereRadiusSqr = math::length2(scene_box.max_corner() - scene_box.min_corner());
		_power = color::get_importance(_color) * color_real(BoundingSphereRadiusSqr * math::pi);
	}

	DirectionalLightSource::DirectionalLightSource(vector3 direction, color_rgb color)
		: _direction(std::move(direction))
		, _color(std::move(color))
		, _power(color::get_importance(_color))
	{
	}

	void DirectionalLightSource::DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const
	{
		DoWithDistribution(context.getPoint(), sampler, job);
	}

	void DirectionalLightSource::DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
		const distibution_func& job) const
	{
		job(lighting_functional_distribution(
				1U,
				false,
				[&]()
				{
					return math::random_sample<std::optional<light_sample>, space_real>(
						light_sample(
							_direction,
							std::numeric_limits<space_real>::max(),
							[this]()
							{
								return _color;
							}
						),
						space_real(1.0),
						true);
				})
		);
	}

	color_real DirectionalLightSource::GetApproximateTotalPower() const
	{
		return _power;
	}
}