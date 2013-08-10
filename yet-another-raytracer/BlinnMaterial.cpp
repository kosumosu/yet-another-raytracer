#include "BlinnMaterial.h"
#include "ShadingContext.h"

color4 BlinnMaterial::Shade( const ShadingContext & context ) const
{
	color4 m_illumination = m_emission;

	FluxCollection * fluxes(context.lighting_server()->GetFluxesAtPoint(context.hit_point(), context.normal(), context.bias()));

	for (auto & flux : *fluxes)
	{
		m_illumination += ComputeDiffuseComponent(context, flux) + ComputeSpecularComponent(context, flux);
	}

	delete fluxes;

	auto reflected_direction = context.incident_ray().direction() - context.normal() * (color_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));

	if (m_specular != color4())
	{
		auto reflection = context.ray_evaluator()->TraceRay(Ray(context.hit_point(), reflected_direction), context.trace_depth(), context.bias());
		m_illumination += reflection * m_specular;
	}

	return m_illumination;
}

color4 BlinnMaterial::ComputeDiffuseComponent( const ShadingContext & context, const Flux & flux ) const
{
	auto intensity = (color_real)std::max(space_real(0.0), math::dot(flux.direction(), context.normal()));
	return m_diffuse * flux.color() * intensity;
}

color4 BlinnMaterial::ComputeSpecularComponent( const ShadingContext & context, const Flux & flux ) const
{
	auto half_vector = math::normalize(flux.direction() - context.incident_ray().direction());
	auto intensity = std::powf((color_real)std::max(space_real(0.0), math::dot(half_vector, context.normal())), m_shininess);
	return m_specular * flux.color() * intensity;
}

Material * BlinnMaterial::Clone() const
{
	return new BlinnMaterial(*this);
}
