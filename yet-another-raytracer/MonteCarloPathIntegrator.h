#pragma once

#include "RayIntegrator.h"

#include <functional>
#include "Material.h"
#include "base_matrix4x4.hpp"

class Hit;
class GeometryObject;
class LightSource;
class Raytracer;

class MonteCarloPathIntegrator : public RayIntegrator
{
public:
	using infinity_func = std::function<color_rgbx(const ray3 & ray)>;

	MonteCarloPathIntegrator(const Raytracer * raytracer, const std::vector<const LightSource*> lights, const infinity_func & infinityEvaluator)
		: _raytracer(raytracer),
		  _lights(lights),
		  _infinityEvaluator(infinityEvaluator) {}

	color_rgbx EvaluateRay(const ray3 & ray, unsigned bounceLimit, space_real bias, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;

private:
	const Raytracer * _raytracer;
	const std::vector<const LightSource*> _lights;
	const infinity_func _infinityEvaluator;

	color_rgbx MonteCarloPathIntegrator::EvaluateRadianceByLightsAtVertex(const ray3 & currentRay, const Hit & hit, const bsdf_distribution & bsdfDistribution, math::UniformRandomBitGenerator<unsigned> & randomEngine) const;
};
