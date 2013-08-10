#include "RayEvaluator.h"
#include "Material.h"
#include "ShadingContext.h"
#include <limits>

#define BIAS std::numeric_limits<space_real>::epsilon() * (space_real)32768.0

RayEvaluator::RayEvaluator(const Raytracer * raytracer, const LightingServer * lighting_server)
	: m_raytracer(raytracer)
	, m_lighting_server(lighting_server)
{
}


RayEvaluator::~RayEvaluator(void)
{
}

color4 RayEvaluator::TraceRay( const Ray & ray, unsigned int depth_left, space_real bias ) const
{
	if (depth_left < 1)
		return m_background_color;

	auto hit = m_raytracer->TraceRay(ray, bias);

	if (!hit.has_occurred())
		return m_background_color;
	
	ShadingContext context;
	context.trace_depth(depth_left - 1);
	context.hit_point(hit.point());

	context.normal(hit.normal());
	context.incident_ray(ray);
	context.object(hit.object());
	context.raytracer(m_raytracer);
	context.ray_evaluator(this);
	context.lighting_server(m_lighting_server);
	context.bias(BIAS);

	auto object = hit.object();
	auto material = object->material();

	//auto color = hit.object()->material()->Shade(context);
	auto color = material->Shade(context);
	return color;
}
