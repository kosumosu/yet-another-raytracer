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
	color_real GetAverageEmission() const override;

	Material * Clone() const override;

private:
	color_rgbx m_emission;
	color_rgbx m_diffuse;
	color_rgbx m_specular;
	color_real m_shininess;

	color_rgbx ComputeDiffuseComponent(const ShadingContext & context, const Flux & flux) const;
	color_rgbx ComputeSpecularComponent(const ShadingContext & context, const Flux & flux) const;

};

