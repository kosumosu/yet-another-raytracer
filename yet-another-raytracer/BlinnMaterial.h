#pragma once

#include "Material.h"
#include "Types.h"
#include <exception>
#include "Flux.h"

class __declspec(align(MM_ALIGNMENT)) BlinnMaterial : public Material
{
public:
	

	BlinnMaterial(void)
		: m_diffuse(color4())
		, m_specular(color4())
		, m_emission(color4())
		, m_shininess(1.0f)
	{
	}

	BlinnMaterial(const color4 & emission, const color4 & diffuse, const color4 & specular, color_real shininess)
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



	const color4 & emission() const { return m_emission; }
	void emission(const color4 & value) { m_emission = value; }

	const color4 & diffuse() const { return m_diffuse; }
	void diffuse(const color4 & value) { m_diffuse = value; }

	const color4 & specular() const { return m_specular; }
	void specular(const color4 & value) { m_specular = value; }

	color_real shininess() const { return m_shininess; }
	void shininess(color_real value) { m_shininess = value; }


	virtual color4 Shade( const ShadingContext & context ) const;

	virtual Material * Clone() const;

private:
	color4 m_emission;
	color4 m_diffuse;
	color4 m_specular;
	color_real m_shininess;

	color4 ComputeDiffuseComponent( const ShadingContext & context, const Flux & flux ) const;
	color4 ComputeSpecularComponent( const ShadingContext & context, const Flux & flux ) const;

};

