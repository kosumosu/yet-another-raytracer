#pragma once

#include "Material.h"
#include "Types.h"

class DielectricMaterial : public Material
{
public:

	DielectricMaterial(void)
		: _iorInside(1.5)
		, _iorOutside(1.0)
	{
	}

	explicit DielectricMaterial(space_real iorInside, space_real iorOutside, const color_rgbx & surfaceTransparency)
		: _iorInside(iorInside)
		, _iorOutside(iorOutside)
		, _surfaceTransparency(surfaceTransparency)
	{

	}


	virtual ~DielectricMaterial(void)
	{
	}

	void setIorInside(space_real ior) { _iorInside = ior; }
	void setIorOutside(space_real ior) { _iorOutside = ior; }
	void setSurfaceTransparency(const color_rgbx & value) { _surfaceTransparency = value; }

	color_rgbx Shade(const ShadingContext & context) const override;
	color_rgbx GetEmission(const ShadingContext & context) const override;
	color_rgbx GetScattering(const ShadingContext & context) const override;
	color_real GetEmissionImportance() const override;

	Material * Clone() const override;

	void WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const override;
	color_rgbx EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
	color_rgbx EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
private:
	space_real _iorInside;
	space_real _iorOutside;
	color_rgbx _surfaceTransparency;
};

