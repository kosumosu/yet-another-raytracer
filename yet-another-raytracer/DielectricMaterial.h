#pragma once

#include "Material.h"

#include <utility>
#include "Types.h"

class DielectricMaterial final : public Material
{
	space_real _iorInside;
	space_real _iorOutside;
	color_rgb surfaceTransparency_;

public:
	DielectricMaterial()
		: _iorInside(1.5)
		, _iorOutside(1.0)
		, surfaceTransparency_(color_rgb::fill(1))
	{
	}

	explicit DielectricMaterial(space_real iorInside, space_real iorOutside, color_rgb surfaceTransparency)
		: _iorInside(iorInside)
		, _iorOutside(iorOutside)
		, surfaceTransparency_(std::move(surfaceTransparency))
	{
	}

	void setIorInside(space_real ior) { _iorInside = ior; }
	void setIorOutside(space_real ior) { _iorOutside = ior; }
	void setSurfaceTransparency(const color_rgb& value) { surfaceTransparency_ = value; }

	color_real GetEmissionImportance() const override;

	void WithBsdfDistribution(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler,
		const bsdf_distribution_func& job) const override;
	
	color_rgb EvaluateEmission(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const override;
	
	color_rgb EvaluateNonDeltaScattering(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const vector3& outgoingDirection,
		const math::Sampler<space_real>& sampler) const override;
};
