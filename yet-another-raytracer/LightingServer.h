#pragma once

#include "LightSource.h"
#include "LightingCommon.h"
#include "Types.h"

class Raytracer;
class RayEvaluator;

class LightingServer
{
public:

	LightingServer();

	~LightingServer(void);

	bool shadows_enabled() const { return m_shadows_enabled; }
	void shadows_enabled(bool val) { m_shadows_enabled = val; }

	const color_rgbx & ambient_light() const { return m_ambient_light; }
	void ambient_light(const color_rgbx & val) { m_ambient_light = val; }

	const LightSourceCollection * lights() const { return m_lights; }
	void lights(const LightSourceCollection * val) { m_lights = val; }

	void IterateOverFluxes(const vector3 & point, const vector3 & normal, space_real bias, unsigned int depthLeft, const RayEvaluator& rayEvaluator, bool allowSubdivision, const flux_func & job) const;

private:
	bool m_shadows_enabled;

	color_rgbx m_ambient_light;
	const LightSourceCollection * m_lights;
};

