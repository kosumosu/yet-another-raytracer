#pragma once

#include "GeometryObject.h"
#include "ray.hpp"
#include "RayEvaluator.h"
#include "LightingServer.h"
#include "Types.h"
#include <memory>

class RayEvaluator;

class ShadingContext
{
public:
	

	ShadingContext(void)
	{
	}

	const GeometryObject * object() const { return m_object; }
	void object(const GeometryObject * val) { m_object = val; }

	const vector3 & world_space_hit_point() const { return m_hit_point; }
	void world_space_hit_point(const vector3 & val) { m_hit_point = val; }

	const vector3 & normal() const { return m_normal; }
	void normal(const vector3 & val) { m_normal = val; }

	const math::ray<space_real, 3> & incident_ray() const { return m_incident_ray; }
	void incident_ray(const math::ray<space_real, 3> & val) { m_incident_ray = val; }

	const RayEvaluator * ray_evaluator() const { return m_ray_evaluator; }
	void ray_evaluator(const RayEvaluator * val) { m_ray_evaluator = val; }

	const LightingServer * lighting_server() const { return m_lighting_server; }
	void lighting_server(const LightingServer * val) { m_lighting_server = val; }


	math::UniformRandomBitGenerator<unsigned> * getRandomEngine() const { return m_randomEngine; }
	void setRandomEngine(math::UniformRandomBitGenerator<unsigned> * const randomEngine) { m_randomEngine = randomEngine; }

	unsigned int trace_depth() const { return m_trace_depth; }
	void trace_depth(unsigned int val) { m_trace_depth = val; }

	space_real bias() const { return m_bias; }
	void bias(space_real val) { m_bias = val; }

	bool allow_subdivision() const { return m_allowSubdivision; }
	void allow_subdivision(bool value) { m_allowSubdivision = value; }

	/*bool getIncludeEmission() const { return m_includeEmission; }
	void setMIncludeEmission(const bool includeEmission) { m_includeEmission = includeEmission; }*/

private:
	const GeometryObject * m_object;
	vector3 m_hit_point;
	space_real m_bias;
	vector3 m_normal;
	math::ray<space_real, 3> m_incident_ray;

	const RayEvaluator * m_ray_evaluator;
	const LightingServer * m_lighting_server;
	math::UniformRandomBitGenerator<unsigned> * m_randomEngine;

	unsigned int m_trace_depth;
	bool m_allowSubdivision;
	//bool m_includeEmission;
};

