#pragma once

#include "LightSource.h"
#include "Types.h"

class Scene;

namespace lights
{
	class DirectionalLightSource : public LightSource
	{
	public:
		DirectionalLightSource(vector3 direction, color_rgb color, const Scene & scene);
		DirectionalLightSource(vector3 direction, color_rgb color);

		void DoWithDistribution(const LightingContext & context, math::Sampler<space_real> & sampler, const distibution_func & job) const noexcept override;

		void DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
			const distibution_func& job) const noexcept override;

		color_real GetApproximateTotalPower() const noexcept override;


		[[nodiscard]] space_real EvaluatePdfExperimental(const ray3& ray) const noexcept override
		{
			return space_real(0);
		}

	private:
		vector3 _direction;
		color_rgb _color;
		color_real _power;
	};
}