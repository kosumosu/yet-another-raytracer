#include "RayEvaluator.h"
#include "Material.h"
#include "ShadingContext.h"
#include <limits>

constexpr space_real BIAS = std::numeric_limits<space_real>::epsilon() * space_real(32768.0);

RayEvaluator::RayEvaluator(const Raytracer * raytracer, const LightingServer * lighting_server, math::UniformRandomBitGenerator<unsigned>* randomEngine)
	: m_raytracer(raytracer)
	, m_lighting_server(lighting_server)
	, m_randomEngine(randomEngine)
{
}

RayEvaluator::~RayEvaluator(void)
{
}

color_rgbx RayEvaluator::TraceRay( const ray3 & ray, unsigned int depth_left, space_real bias, bool allowSubdivision, bool includeEmission) const
{
	if (depth_left < 1)
		return color_rgbx();

	auto hit = m_raytracer->TraceRay(ray, bias);

	if (!hit.has_occurred())
		return m_background_color;
		//return color_rgbx(ray.direction()[1] * 0.5 + 0.5);
	
	ShadingContext context;
	context.trace_depth(depth_left - 1);
	context.world_space_hit_point(hit.point());

	context.normal(hit.normal());
	context.incident_ray(ray);
	context.object(hit.object());
	context.ray_evaluator(this);
	context.setRandomEngine(m_randomEngine);
	context.lighting_server(m_lighting_server);
	context.bias(BIAS);
	context.allow_subdivision(allowSubdivision);

	auto object = hit.object();
	auto material = object->material();

	//auto color = hit.object()->material()->Shade(context);
	auto color = includeEmission ? material->Shade(context) : material->GetScattering(context);
	return color;
}

bool RayEvaluator::DoesIntersect(const ray3 & ray, space_real minDistance, space_real maxDistance) const
{
	return m_raytracer->DoesIntersect(ray, minDistance, maxDistance, nullptr, vector3());
}
