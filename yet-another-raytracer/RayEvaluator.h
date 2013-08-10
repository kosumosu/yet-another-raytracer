#pragma once

#include "Raytracer.h"
#include "LightingServer.h"
#include "Ray.h"
#include "Types.h"

class __declspec(align(MM_ALIGNMENT)) RayEvaluator
{
public:
	
	RayEvaluator(const Raytracer * raytracer, const LightingServer * lighting_server);
	~RayEvaluator(void);

	// Returns color of traced ray
	color4 TraceRay(const Ray & ray, unsigned int depth_left, space_real bias) const;

	color4 background_color() const { return m_background_color; }
	void background_color(const color4 & val) { m_background_color = val; }

	const Raytracer * raytracer() const { return m_raytracer; }

	const LightingServer * lighting_server() const { return m_lighting_server; }

private:
	color4 m_background_color;
	const Raytracer * m_raytracer;
	const LightingServer * m_lighting_server;
};

