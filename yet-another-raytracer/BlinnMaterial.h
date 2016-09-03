#pragma once

#include "Material.h"
#include "Types.h"

class BlinnMaterial : public Material
{
public:
	

	BlinnMaterial(void)
		: _emission(color_rgbx::zero())
		, _diffuse(color_rgbx::zero())
		, _specular(color_rgbx::zero())
		, _shininess(1.0f)
		, _translucency(color_rgbx::zero()) { }

	BlinnMaterial(const color_rgbx & emission, const color_rgbx & diffuse, const color_rgbx & specular, color_real shininess, const color_rgbx & translucency)
		: _emission(emission)
		, _diffuse(diffuse)
		, _specular(specular)
		, _shininess(shininess)
		, _translucency(translucency)
	{
		
	}

	BlinnMaterial(const BlinnMaterial & original)
		: _emission(original._emission)
		, _diffuse(original._diffuse)
		, _specular(original._specular)
		, _shininess(original._shininess)
		, _translucency(original._translucency)
	{

	}

	~BlinnMaterial(void)
	{
	}



	const color_rgbx & emission() const { return _emission; }
	void emission(const color_rgbx & value) { _emission = value; }

	const color_rgbx & diffuse() const { return _diffuse; }
	void diffuse(const color_rgbx & value) { _diffuse = value; }

	const color_rgbx & specular() const { return _specular; }
	void specular(const color_rgbx & value) { _specular = value; }

	color_real shininess() const { return _shininess; }
	void shininess(color_real value) { _shininess = value; }

	color_rgbx translucency() const { return _translucency; }
	void translucency(const color_rgbx & value) { _translucency = value; }

	color_real GetEmissionImportance() const override;

	Material * Clone() const override;

	void WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const override;
	color_rgbx EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;
	color_rgbx EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const override;

private:
	color_rgbx _emission;
	color_rgbx _diffuse;
	color_rgbx _specular;
	color_real _shininess;
	color_rgbx _translucency;

	color_real GetReflectionProbability() const;

	color_rgbx EvaluateDiffuseColor(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const;
};

