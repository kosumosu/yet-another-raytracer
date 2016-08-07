#pragma once

#include "Material.h"
#include "Types.h"
#include "Flux.h"

class BlinnMaterial : public Material
{
public:
	

	BlinnMaterial(void)
		: m_emission(color_rgbx())
		, m_diffuse(color_rgbx())
		, m_specular(color_rgbx())
		, m_shininess(1.0f)
	{
	}

	BlinnMaterial(const color_rgbx & emission, const color_rgbx & diffuse, const color_rgbx & specular, color_real shininess)
		: m_emission(emission)
		, m_diffuse(diffuse)
		, m_specular(specular)
		, m_shininess(shininess)
	{
		
	}

	BlinnMaterial(const BlinnMaterial & original)
		: m_emission(original.emission())
		, m_diffuse(original.diffuse())
		, m_specular(original.specular())
		, m_shininess(original.shininess())
	{

	}

	~BlinnMaterial(void)
	{
	}



	const color_rgbx & emission() const { return m_emission; }
	void emission(const color_rgbx & value) { m_emission = value; }

	const color_rgbx & diffuse() const { return m_diffuse; }
	void diffuse(const color_rgbx & value) { m_diffuse = value; }

	const color_rgbx & specular() const { return m_specular; }
	void specular(const color_rgbx & value) { m_specular = value; }

	color_real shininess() const { return m_shininess; }
	void shininess(color_real value) { m_shininess = value; }


	color_rgbx Shade( const ShadingContext & context ) const override;
	color_rgbx GetEmission(const ShadingContext & context) const override;
	color_rgbx GetScattering(const ShadingContext & context) const override;
	color_real GetEmissionImportance() const override;

	Material * Clone() const override;

	void WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const override;
	color_rgbx EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
	color_rgbx EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;

private:
	color_rgbx m_emission;
	color_rgbx m_diffuse;
	color_rgbx m_specular;
	color_real m_shininess;

	color_rgbx ComputeDiffuseComponent(const ShadingContext & context, const Flux & flux) const;
	color_rgbx ComputeSpecularComponent(const ShadingContext & context, const Flux & flux) const;
	color_real GetReflectionProbability() const;

	color_rgbx EvaluateDiffuseColor(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const;
};

