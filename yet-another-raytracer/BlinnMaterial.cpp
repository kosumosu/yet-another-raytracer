#include "BlinnMaterial.h"
#include "ShadingContext.h"
#include "LightingContext.h"
#include "color_functions.hpp"

#define ENABLE_IMPORTANCE_SAMPLING true

using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;


color_rgbx BlinnMaterial::Shade(const ShadingContext & context) const
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
		const auto reflected_direction = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));
		const auto reflection = context.ray_evaluator()->TraceRay(ray3(context.world_space_hit_point(), reflected_direction), context.trace_depth(), context.bias(), context.allow_subdivision(), true);
		radiance += reflection * m_specular;
	}

	return radiance;
}

color_real BlinnMaterial::GetEmissionImportance() const
{
	return color_real(math::pi) * color::get_importance(m_emission);
}

color_rgbx BlinnMaterial::ComputeDiffuseComponent(const ShadingContext & context, const Flux & flux) const
{
	//auto diffuse = math::clamp(color_rgbx(context.normal()[0], context.normal()[1], context.normal()[2], color_real(0.0)), color_rgbx(0.0), color_rgbx(1.0));
	return m_diffuse * (color_rgbx(1.0) - m_specular) * flux.color() * color_real(math::oneOverPi);
}

color_rgbx BlinnMaterial::ComputeSpecularComponent(const ShadingContext & context, const Flux & flux) const
{
	auto half_vector = math::normalize(flux.direction() - context.incident_ray().direction());
	auto intensity = std::pow(color_real(std::max(space_real(0.0), math::dot(half_vector, context.normal()))), m_shininess);
	return m_specular * flux.color() * intensity;
}

color_real BlinnMaterial::GetReflectionProbability() const
{
	return color::get_importance(m_specular);
}

color_rgbx BlinnMaterial::EvaluateDiffuseColor(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	//return math::dot(incidentDirection, normal) < 0 ? color_rgbx(1.0, 0.0, 0.0, 0.0) : color_rgbx(0.0, 1.0, 1.0, 0.0);

	return m_diffuse;
}

void BlinnMaterial::WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, incidentDirection, randomEngine);
	bsdf_functional_distribution::generate_sample_func generateDiffuseFuncImpl = [&]()
		{
#if ENABLE_IMPORTANCE_SAMPLING
			auto direction = math::cosineWeightedHemiSphericalRand(normal, randomEngine);
			const space_real pdf = math::dot(direction, normal) * space_real(math::oneOverPi);
#else
			auto direction = math::hemiSphericalRand(normal, randomEngine);
			const space_real pdf = space_real(0.5 * math::oneOverPi);
#endif
			return math::random_sample<const bsdf_sample, space_real>(
				bsdf_sample(direction,
				            [=]()
				            {
					            return diffuseColor * (color_rgbx(1.0) - m_specular);
				            }),
				pdf * (1.0 - GetReflectionProbability()),
				false
			);
		};

	bsdf_functional_distribution::generate_sample_func generateDiffuseFunc =
		diffuseColor == color_rgbx() || m_specular == color_rgbx(1.0)
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: generateDiffuseFuncImpl;

	bsdf_functional_distribution::evaluate_pdf_func evaluateDiffusePdfFunc =
		diffuseColor == color_rgbx() || m_specular == color_rgbx(1.0)
			? bsdf_functional_distribution::evaluate_pdf_func(nullptr)
			: [&](const vector3 & direction)
			{
#if ENABLE_IMPORTANCE_SAMPLING
				return color_real(math::dot(direction, normal) * space_real(math::oneOverPi)) * (1.0 - GetReflectionProbability());
#else
				return color_real(0.5 * math::oneOverPi) * (1.0 - GetReflectionProbability());
#endif
			};

	bsdf_functional_distribution::generate_sample_func generateReflectionFuncImpl = [&]()
		{
			const auto cosTheta = math::dot(incidentDirection, normal);
			const auto reflected_direction = incidentDirection - normal * (space_real(2.0) * cosTheta);
			return math::random_sample<const bsdf_sample, space_real>(
				bsdf_sample(reflected_direction,
				            [=]()
				            {
					            return m_specular / color_real(-cosTheta);
				            }),
				space_real(GetReflectionProbability()),
				true
			);
		};

	bsdf_functional_distribution::generate_sample_func generateReflectionFunc =
		m_specular == color_rgbx()
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: generateReflectionFuncImpl;

	bsdf_functional_distribution::iterate_over_delta_func iterateReflectionFunc =
		m_specular == color_rgbx()
			? bsdf_functional_distribution::iterate_over_delta_func(nullptr)
			: [&](const bsdf_functional_distribution::delta_func & subJob)
			{
				subJob(generateReflectionFuncImpl());
			};

	bsdf_functional_distribution::generate_sample_func generateSampleFunc =
		m_specular == color_rgbx() && diffuseColor == color_rgbx()
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: [&]()
			{
				const color_real reflectionPobability = GetReflectionProbability();
				std::uniform_real_distribution<space_real> distr;
				if (distr(randomEngine) < reflectionPobability)
					return generateReflectionFuncImpl();
				else
					return generateDiffuseFuncImpl();
			};


	job(bsdf_functional_distribution(
		iterateReflectionFunc == nullptr ? 0U : 1U,
		iterateReflectionFunc,
		generateReflectionFunc,
		generateDiffuseFunc,
		evaluateDiffusePdfFunc,
		generateSampleFunc));
}

color_rgbx BlinnMaterial::EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	return m_emission;
}

color_rgbx BlinnMaterial::EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, incidentDirection, randomEngine);
	return diffuseColor * (color_rgbx(1.0) - m_specular);
}

Material * BlinnMaterial::Clone() const
{
	return new BlinnMaterial(*this);
}
