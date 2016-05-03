#pragma once

#include "Material.h"
#include "Types.h"
#include "Flux.h"

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

	virtual color_rgbx Shade(const ShadingContext & context) const;

	virtual Material * Clone() const;

private:
	space_real m_iorInside;
	space_real m_iorOutside;
};

