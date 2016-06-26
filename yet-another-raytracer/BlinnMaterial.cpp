#include "BlinnMaterial.h"
#include "ShadingContext.h"

color_rgbx BlinnMaterial::Shade( const ShadingContext & context ) const
{
	color_rgbx m_illumination = m_emission;

	if (m_diffuse != color_rgbx() || (m_specular != color_rgbx() && m_shininess > color_real(0.0)))
	{
		FluxCollection * fluxes(context.lighting_server()->GetFluxesAtPoint(context.world_space_hit_point(), context.normal(), context.bias(), context.trace_depth(), *context.ray_evaluator(), context.allow_subdivision()));

		for (auto & flux : *fluxes)
		{
			auto differentialCoeff = color_real(std::max(space_real(0.0), math::dot(flux.direction(), context.normal())));

			m_illumination += (ComputeDiffuseComponent(context, flux) + ComputeSpecularComponent(context, flux)) * differentialCoeff / flux.probabilityDensity();
		}

		delete fluxes;
	}


	if (m_specular != color_rgbx())
	{
		auto reflected_direction = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));
		auto reflection = context.ray_evaluator()->TraceRay(Ray(context.world_space_hit_point(), reflected_direction), context.trace_depth(), context.bias(), context.allow_subdivision());
		m_illumination += reflection * m_specular;
	}

	return m_illumination;
}

color_rgbx BlinnMaterial::ComputeDiffuseComponent( const ShadingContext & context, const Flux & flux) const
{
	//auto diffuse = math::clamp(color_rgbx(context.normal()[0], context.normal()[1], context.normal()[2], color_real(0.0)), color_rgbx(0.0), color_rgbx(1.0));
	return m_diffuse * (color_rgbx(1.0) - m_specular) * flux.color() * color_real(math::oneOverPi);
}

color_rgbx BlinnMaterial::ComputeSpecularComponent( const ShadingContext & context, const Flux & flux ) const
{
	auto half_vector = math::normalize(flux.direction() - context.incident_ray().direction());
	auto intensity = std::pow(color_real(std::max(space_real(0.0), math::dot(half_vector, context.normal()))), m_shininess);
	return m_specular * flux.color() * intensity;
}

Material * BlinnMaterial::Clone() const
{
	return new BlinnMaterial(*this);
}
