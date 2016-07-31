#include "BlinnMaterial.h"
#include "ShadingContext.h"
#include "LightingContext.h"

color_rgbx BlinnMaterial::Shade( const ShadingContext & context ) const
{
	return m_emission + BlinnMaterial::GetScattering(context);
}

color_rgbx BlinnMaterial::GetEmission(const ShadingContext & context) const
{
	return m_emission;
}

color_rgbx BlinnMaterial::GetScattering(const ShadingContext & context) const
{
	color_rgbx radiance;

	if (m_diffuse != color_rgbx() || (m_specular != color_rgbx() && m_shininess > color_real(0.0)))
	{
		context.lighting_server()->IterateOverFluxes(LightingContext(context.world_space_hit_point(), context.normal(), context.bias(), context.trace_depth(), context.allow_subdivision()), *context.ray_evaluator(), *context.getRandomEngine(),
			[&](const Flux & flux)
		{
			auto differentialCoeff = color_real(std::max(space_real(0.0), math::dot(flux.direction(), context.normal())));

			radiance += (ComputeDiffuseComponent(context, flux) + ComputeSpecularComponent(context, flux)) * differentialCoeff / flux.probabilityDensity();
		});
	}


	if (m_specular != color_rgbx())
	{
		auto reflected_direction = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));
		auto reflection = context.ray_evaluator()->TraceRay(ray3(context.world_space_hit_point(), reflected_direction), context.trace_depth(), context.bias(), context.allow_subdivision(), true);
		radiance += reflection * m_specular;
	}

	return radiance;
}

color_real BlinnMaterial::GetAverageEmission() const
{
	return color_real(math::pi) * (m_emission[0] + m_emission[1] + m_emission[2]) / color_real(3.0);
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
