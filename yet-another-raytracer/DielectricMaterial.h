#pragma once

#include "Material.h"
#include "Types.h"

class DielectricMaterial : public Material
{
public:

	DielectricMaterial(void)
		: m_iorInside(1.5)
		, m_iorOutside(1.0)
	{
	}

	explicit DielectricMaterial(space_real iorInside, space_real iorOutside)
		: m_iorInside(iorInside)
		, m_iorOutside(iorOutside)
	{

	}


	virtual ~DielectricMaterial(void)
	{
	}

	void setIorInside(space_real ior) { m_iorInside = ior; }
	void setIorOutside(space_real ior) { m_iorOutside = ior; }

	color_rgbx Shade(const ShadingContext & context) const override;
	color_rgbx GetEmission(const ShadingContext & context) const override;
	color_rgbx GetScattering(const ShadingContext & context) const override;
	color_real GetEmissionImportance() const override;

	Material * Clone() const override;

	void WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const override;
	color_rgbx EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
	color_rgbx EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
private:
	space_real m_iorInside;
	space_real m_iorOutside;
};

