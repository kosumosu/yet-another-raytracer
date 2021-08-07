#pragma once

#include "Material.h"
#include "Types.h"

class DielectricMaterial final : public Material
{
	space_real _iorInside;
	space_real _iorOutside;
	color_rgbx surfaceTransparency_;

public:
	DielectricMaterial()
		: _iorInside(1.5)
		, _iorOutside(1.0)
		, surfaceTransparency_(color_rgbx::fill(1))
	{
	}

	explicit DielectricMaterial(space_real iorInside, space_real iorOutside, const color_rgbx& surfaceTransparency)
		: _iorInside(iorInside)
		, _iorOutside(iorOutside)
		, surfaceTransparency_(surfaceTransparency)
	{
	}


	virtual ~DielectricMaterial(void)
	{
	}

	void setIorInside(space_real ior) { _iorInside = ior; }
	void setIorOutside(space_real ior) { _iorOutside = ior; }
	void setSurfaceTransparency(const color_rgbx& value) { surfaceTransparency_ = value; }

	color_real GetEmissionImportance() const override;

	void WithBsdfDistribution(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler,
		const bsdf_distribution_func& job) const override;
	
	color_rgbx EvaluateEmission(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const override;
	
	color_rgbx EvaluateNonDeltaScattering(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const vector3& outgoingDirection,
		const math::Sampler<space_real>& sampler) const override;
};
