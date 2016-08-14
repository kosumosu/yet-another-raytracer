#include "BlinnMaterial.h"
#include "ShadingContext.h"
#include "LightingContext.h"
#include "color_functions.hpp"

#define ENABLE_IMPORTANCE_SAMPLING true

using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;


color_rgbx BlinnMaterial::Shade(const ShadingContext & context) const
{
	return _emission + BlinnMaterial::GetScattering(context);
}

color_rgbx BlinnMaterial::GetEmission(const ShadingContext & context) const
{
	return _emission;
}

color_rgbx BlinnMaterial::GetScattering(const ShadingContext & context) const
{
	color_rgbx radiance;

	if (_diffuse != color_rgbx() || (_specular != color_rgbx() && _shininess > color_real(0.0)))
	{
		context.lighting_server()->IterateOverFluxes(LightingContext(context.world_space_hit_point(), context.normal(), context.bias(), context.trace_depth(), context.allow_subdivision()), *context.ray_evaluator(), *context.getRandomEngine(),
		                                             [&](const Flux & flux)
		                                             {
			                                             auto differentialCoeff = color_real(std::max(space_real(0.0), math::dot(flux.direction(), context.normal())));

			                                             radiance += (ComputeDiffuseComponent(context, flux) + ComputeSpecularComponent(context, flux)) * differentialCoeff / flux.probabilityDensity();
		                                             });
	}


	if (_specular != color_rgbx())
	{
		const auto reflected_direction = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));
		const auto reflection = context.ray_evaluator()->TraceRay(ray3(context.world_space_hit_point(), reflected_direction), context.trace_depth(), context.bias(), context.allow_subdivision(), true);
		radiance += reflection * _specular;
	}

	return radiance;
}

color_real BlinnMaterial::GetEmissionImportance() const
{
	return color_real(math::pi) * color::get_importance(_emission);
}

color_rgbx BlinnMaterial::ComputeDiffuseComponent(const ShadingContext & context, const Flux & flux) const
{
	//auto diffuse = math::clamp(color_rgbx(context.normal()[0], context.normal()[1], context.normal()[2], color_real(0.0)), color_rgbx(0.0), color_rgbx(1.0));
	return _diffuse * (color_rgbx(1.0) - _specular) * flux.color() * color_real(math::oneOverPi);
}

color_rgbx BlinnMaterial::ComputeSpecularComponent(const ShadingContext & context, const Flux & flux) const
{
	auto half_vector = math::normalize(flux.direction() - context.incident_ray().direction());
	auto intensity = std::pow(color_real(std::max(space_real(0.0), math::dot(half_vector, context.normal()))), _shininess);
	return _specular * flux.color() * intensity;
}

color_real BlinnMaterial::GetReflectionProbability() const
{
	return color::get_importance(_specular);
}

color_rgbx BlinnMaterial::EvaluateDiffuseColor(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	//return math::dot(incidentDirection, normal) < 0 ? color_rgbx(1.0, 0.0, 0.0, 0.0) : color_rgbx(0.0, 1.0, 1.0, 0.0);

	return _diffuse;
}

void BlinnMaterial::WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, incidentDirection, randomEngine);
	bsdf_functional_distribution::generate_sample_func generateDiffuseFuncImpl = [&]()
		{
			const bool isEntering = math::is_obtuse_angle(incidentDirection, normal);
			std::uniform_real_distribution<color_real> distr(color_real(0.0), upperRandomBound<color_real>()); // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
			const auto translucenceProbability = color::get_importance(_translucency);
			const bool isTranslucent = translucenceProbability > color_real(0.0) && translucenceProbability >= distr(randomEngine);

			const bool flipNormal = isEntering == isTranslucent;

#if ENABLE_IMPORTANCE_SAMPLING
			const auto direction = flipNormal ? math::cosineWeightedHemiSphericalRand(-normal, randomEngine) : math::cosineWeightedHemiSphericalRand(normal, randomEngine);
			const space_real pdf = std::abs(math::dot(direction, normal)) * space_real(math::oneOverPi);
#else
			const auto direction = math::hemiSphericalRand(-normal, randomEngine);
			const space_real pdf = space_real(0.5 * math::oneOverPi);
#endif

			if (isTranslucent)
			{
				return math::random_sample<const bsdf_sample, space_real>(
					bsdf_sample(direction,
						[=]()
						{
							return _translucency * (color_rgbx(1.0) - _specular);
						}),
						pdf * (1.0 - GetReflectionProbability()) * translucenceProbability,
						false
						);
			}
			else
			{
				return math::random_sample<const bsdf_sample, space_real>(
					bsdf_sample(direction,
						[=]()
						{
							return diffuseColor * (color_rgbx(1.0) - _specular) * (color_rgbx(1.0) - _translucency);
						}),
						pdf * (1.0 - GetReflectionProbability()) * (color_real(1.0) - translucenceProbability),
						false
						);
			}
		};

	bsdf_functional_distribution::generate_sample_func generateDiffuseFunc =
		diffuseColor == color_rgbx() || _specular == color_rgbx(1.0)
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: generateDiffuseFuncImpl;

	bsdf_functional_distribution::evaluate_pdf_func evaluateDiffusePdfFunc =
		diffuseColor == color_rgbx() || _specular == color_rgbx(1.0)
			? bsdf_functional_distribution::evaluate_pdf_func(nullptr)
			: [&](const vector3 & direction)
			{
				const auto translucenceProbability = color::get_importance(_translucency);
#if ENABLE_IMPORTANCE_SAMPLING
				return math::is_obtuse_angle(direction, normal)
					? color_real(math::dot(direction, normal) * space_real(math::oneOverPi)) * (1.0 - GetReflectionProbability()) * (color_real(1.0) - translucenceProbability)
					: color_real(math::dot(direction, -normal) * space_real(math::oneOverPi)) * (1.0 - GetReflectionProbability()) * translucenceProbability;
#else
				return math::is_obtuse_angle(direction, normal)
					? color_real(0.5 * math::oneOverPi) * (1.0 - GetReflectionProbability()) * (color_real(1.0) - translucenceProbability)
					: color_real(0.5 * math::oneOverPi) * (1.0 - GetReflectionProbability()) * translucenceProbability;
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
								return _specular / color_real(std::abs(cosTheta));
				            }),
				space_real(GetReflectionProbability()),
				true
			);
		};

	bsdf_functional_distribution::generate_sample_func generateReflectionFunc =
		_specular == color_rgbx()
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: generateReflectionFuncImpl;

	bsdf_functional_distribution::iterate_over_delta_func iterateReflectionFunc =
		_specular == color_rgbx()
			? bsdf_functional_distribution::iterate_over_delta_func(nullptr)
			: [&](const bsdf_functional_distribution::delta_func & subJob)
			{
				subJob(generateReflectionFuncImpl());
			};

	bsdf_functional_distribution::generate_sample_func generateSampleFunc =
		_specular == color_rgbx() && diffuseColor == color_rgbx()
			? bsdf_functional_distribution::generate_sample_func(nullptr)
			: [&]()
			{
				const color_real reflectionPobability = GetReflectionProbability();
				std::uniform_real_distribution<color_real> distr(color_real(0.0), upperRandomBound<color_real>()); // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
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
	return _emission;
}

color_rgbx BlinnMaterial::EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, incidentDirection, randomEngine);
	return math::is_obtuse_angle(incidentDirection, normal)
		? diffuseColor * (color_rgbx(1.0) - _specular) * (color_rgbx(1.0) - _translucency)
		: (color_rgbx(1.0) - _specular) * _translucency;
}

Material * BlinnMaterial::Clone() const
{
	return new BlinnMaterial(*this);
}
