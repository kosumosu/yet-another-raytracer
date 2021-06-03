#pragma once

#include "LightSource.h"
#include "Types.h"

class Scene;

class DirectionalLightSource : public LightSource
{
public:
	DirectionalLightSource(const vector3 & direction, const color_rgbx & color, const Scene & scene);

	void DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<random_int_t> & randomEngine, const distibution_func & job) const override;

	color_real GetApproximateTotalPower() const override;
private:
	vector3 _direction;
	color_rgbx _color;
	color_real _power;
};

