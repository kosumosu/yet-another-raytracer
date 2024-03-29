#pragma once

#include "RayIntegrator.h"

#include "discrete_distribution.hpp"
#include "Material.h"
#include "Statistics.h"

#include <functional>

class Hit;
class GeometryObject;
class LightSource;
class Raytracer;

class MonteCarloPathIntegrator : public RayIntegrator
{
public:
	using infinity_func = std::function<color_rgbx(const ray3& ray)>;
private:
	const Raytracer* raytracer_;
	const infinity_func infinityEvaluator_;

	mutable statistics::Stats stats_;

	math::discrete_distribution<const LightSource*, color_real> lightDistribution_;
	color_real oneOverTotalPower_;

public:
	MonteCarloPathIntegrator(const Raytracer* raytracer, const std::vector<const LightSource*>& lights, infinity_func infinityEvaluator);

	color_rgbx EvaluateRay(const ray3& ray, unsigned bounceLimit, space_real bias, math::Sampler<space_real>& sampler) const override;

	const statistics::Stats& getStats() const
	{
		return stats_;
	}

private:
	color_rgbx EvaluateRadianceByLightsAtVertex(
		const ray3& currentRay,
		const Hit& hit,
		bool entering,
		const bsdf_distribution& bsdfDistribution,
		math::Sampler<space_real>& sampler) const;
};
