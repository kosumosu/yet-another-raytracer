#include "DirectionalLightSource.h"
#include "LightingContext.h"
#include "Scene.h"
#include "color_functions.hpp"
#include <limits>

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;


DirectionalLightSource::DirectionalLightSource(const vector3 & direction, const color_rgbx & color, const Scene & scene) 
	: _direction(direction)
	, _color(color)
{
	bounding_box3 scene_box;
	for (const auto & object : scene.objects())
	{
		scene_box.Include(object->bounding_box());
	}

	const auto BoundingSphereRadiusSqr = math::length2(scene_box.max_corner() - scene_box.min_corner());
	_power = color::get_importance(_color) * BoundingSphereRadiusSqr * math::pi;
}

void DirectionalLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	if (true || math::dot(_direction, context.getNormal()) >= 0.0f)
	{
		job(lighting_functional_distribution(
				1U,
				[&](const lighting_functional_distribution::delta_func & subJob)
				{
					subJob(math::random_sample<const light_sample, space_real>(
						light_sample(
							_direction,
							std::numeric_limits<space_real>::max(),
							[&]()
							{
								return _color;
							}
						),
						space_real(1.0),
						true));
				},
				[&]()
				{
					return math::random_sample<const light_sample, space_real>(
						light_sample(
							_direction,
							std::numeric_limits<space_real>::max(),
							[=]()
							{
								return _color;
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

color_real DirectionalLightSource::GetApproximateTotalPower() const
{
	return _power;
}
