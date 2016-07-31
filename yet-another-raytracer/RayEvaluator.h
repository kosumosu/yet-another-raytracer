#pragma once

#include "Raytracer.h"
#include "LightingServer.h"
#include "ray.hpp"
#include "Types.h"

class RayEvaluator
{
public:
	
	RayEvaluator(const Raytracer * raytracer, const LightingServer * lighting_server, math::UniformRandomBitGenerator<unsigned> * randomEngine);
	~RayEvaluator(void);

	// Returns color of traced ray
	color_rgbx TraceRay(const ray3 & ray, unsigned int depth_left, space_real bias, bool allowSubdivision, bool includeEmission) const;
	bool DoesIntersect(const ray3 & ray, space_real minDistance, space_real maxDistance) const;

	color_rgbx background_color() const { return m_background_color; }
	void background_color(const color_rgbx & val) { m_background_color = val; }

	const Raytracer * raytracer() const { return m_raytracer; }

	const LightingServer * lighting_server() const { return m_lighting_server; }

private:
	color_rgbx m_background_color;
	const Raytracer * m_raytracer;
	const LightingServer * m_lighting_server;
	math::UniformRandomBitGenerator<unsigned> * m_randomEngine;
};

