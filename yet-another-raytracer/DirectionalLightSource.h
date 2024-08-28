#pragma once

#include "LightSource.h"
#include "Types.h"

class Scene;

class DirectionalLightSource : public LightSource
{
public:
	DirectionalLightSource(const vector3 & direction, const color_rgb & color, const Scene & scene);

	void DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	vector3 _direction;
	color_rgb _color;
	color_real _power;
};

