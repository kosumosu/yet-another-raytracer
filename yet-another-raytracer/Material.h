#pragma once

#include "Types.h"

class ShadingContext;

class Material
{
public:

	Material(void)
	{
	}

	virtual ~Material(void)
	{
	}

	// Returns color as the result of shading
	virtual color_rgbx Shade(const ShadingContext & context) const = 0;

	virtual color_rgbx GetEmission(const ShadingContext & context) const = 0;
	virtual color_rgbx GetScattering(const ShadingContext & context) const = 0;

	// first integrate over hemisphere, then find average across averywhere
	virtual color_real GetAverageEmission() const = 0;

	virtual Material * Clone() const = 0;
};