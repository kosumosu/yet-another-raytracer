#pragma once

#include "Flux.h"
#include "LightSource.h"
#include "Raytracer.h"
#include "Types.h"
#include <vector>
#include <memory>

class __declspec(align(MM_ALIGNMENT)) LightingServer
{
public:

	LightingServer(const Raytracer * raytracer);

	~LightingServer(void);

	bool shadows_enabled() const { return m_shadows_enabled; }
	void shadows_enabled(bool val) { m_shadows_enabled = val; }

	const color4 & ambient_light() const { return m_ambient_light; }
	void ambient_light(const color4 & val) { m_ambient_light = val; }

	const LightSourceCollection * lights() const { return m_lights; }
	void lights(const LightSourceCollection * val) { m_lights = val; }

	const Raytracer * raytracer() const { return m_raytracer; }
	void raytracer(const Raytracer * val) { m_raytracer = val; }

	// Get all incident illumination fluxes that affect given point. Shadowed fluxes are filtered out. Either culled are.
	// Caller must destroy the result.
	FluxCollection * GetFluxesAtPoint( const vector3 & point, const vector3 & normal, space_real bias ) const;

private:
	bool m_shadows_enabled;

	color4 m_ambient_light;
	const LightSourceCollection * m_lights;

	const Raytracer * m_raytracer;

};

