#include "BlinnMaterial.h"
#include "Texture.h"
#include "color_functions.hpp"

#define ENABLE_IMPORTANCE_SAMPLING true

using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;

color_real BlinnMaterial::GetEmissionImportance() const
{
	return color_real(math::pi) * color::get_importance(emission_);
}

color_real BlinnMaterial::GetReflectionProbability() const
{
	return color::get_importance(specular_);
}

color_rgbx BlinnMaterial::EvaluateDiffuseColor(
	const GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const vector2& uv,
	const vector3& incidentDirection,
	const math::Sampler<space_real>& sampler) const
{
	//return math::dot(incidentDirection, normal) < 0 ? color_rgbx(1.0, 0.0, 0.0, 0.0) : color_rgbx(0.0, 1.0, 1.0, 0.0);

	if (diffuseMap_ == nullptr)
	{
		return diffuse_;
	}
	else
	{
		return diffuseMap_->Sample(TextureCoords{hitPoint, normal, {uv}});
	}
}

void BlinnMaterial::WithBsdfDistribution(
	const GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const math::Sampler<space_real>& sampler,
	const bsdf_distribution_func& job) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, uvs[0], incidentDirection, sampler);
	bsdf_functional_distribution::generate_sample_func generateDiffuseFuncImpl = [&]()
	{
		const bool isEntering = math::is_obtuse_angle(incidentDirection, normal);
		// a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
		const auto translucenceProbability = color::get_importance(translucency_);
		const bool isTranslucent = translucenceProbability > color_real(0.0) && translucenceProbability >= sampler.Get1D();

		const bool flipNormal = isEntering == isTranslucent;

#if ENABLE_IMPORTANCE_SAMPLING
		const auto [direction, pdf] = flipNormal
			? math::cosineWeightedHemiSphericalRand(-normal, sampler)
			: math::cosineWeightedHemiSphericalRand(normal, sampler);
#else
			const auto direction = math::hemiSphericalRand(-normal, sampler);
			const space_real pdf = space_real(0.5 * math::oneOverPi);
#endif

		if (isTranslucent)
		{
			return math::random_sample<const bsdf_sample, space_real>(
				bsdf_sample(
					direction,
					[=, this]()
					{
						return translucency_ * (color_rgbx::fill(1.0) - specular_);
					}),
				pdf * (color_real(1.0) - GetReflectionProbability()) * translucenceProbability,
				false);
		}
		else
		{
			return math::random_sample<const bsdf_sample, space_real>(
				bsdf_sample(
					direction,
					[=, this]()
					{
						return diffuseColor * (color_rgbx::fill(1.0) - specular_) * (color_rgbx::fill(1.0) - translucency_);
					}),
				pdf * (color_real(1.0) - GetReflectionProbability()) * (color_real(1.0) - translucenceProbability),
				false);
		}
	};

	bsdf_functional_distribution::generate_sample_func generateDiffuseFunc = diffuseColor == color_rgbx::zero() || specular_ == color_rgbx::fill(1.0)
		? bsdf_functional_distribution::generate_sample_func(nullptr)
		: generateDiffuseFuncImpl;

	bsdf_functional_distribution::evaluate_pdf_func evaluateDiffusePdfFunc = diffuseColor == color_rgbx::zero() || specular_ == color_rgbx::fill(1.0)
		? bsdf_functional_distribution::evaluate_pdf_func(nullptr)
		: [&](const vector3& direction)
		{
			const auto translucenceProbability = color::get_importance(translucency_);
#if ENABLE_IMPORTANCE_SAMPLING
			return math::is_obtuse_angle(direction, normal)
				? color_real(math::dot(direction, normal) * space_real(math::oneOverPi)) * (color_real(1.0) - GetReflectionProbability()) * (color_real(1.0) -
					translucenceProbability)
				: color_real(math::dot(direction, -normal) * space_real(math::oneOverPi)) * (color_real(1.0) - GetReflectionProbability()) *
				translucenceProbability;
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
			bsdf_sample(
				reflected_direction,
				[=, this]()
				{
					return specular_ / color_real(std::abs(cosTheta));
				}),
			space_real(GetReflectionProbability()),
			true);
	};

	bsdf_functional_distribution::generate_sample_func generateReflectionFunc = specular_ == color_rgbx::zero()
		? bsdf_functional_distribution::generate_sample_func(nullptr)
		: generateReflectionFuncImpl;

	bsdf_functional_distribution::generate_sample_func generateSampleFunc = specular_ == color_rgbx::zero() && diffuseColor == color_rgbx::zero()
		? bsdf_functional_distribution::generate_sample_func(nullptr)
		: [&]()
		{
			const color_real reflectionProbability = GetReflectionProbability();
			// a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
			if (sampler.Get1D() < reflectionProbability)
				return generateReflectionFuncImpl();
			else
				return generateDiffuseFuncImpl();
		};


	job(
		bsdf_functional_distribution(
			specular_ == color_rgbx::zero() ? 0U : 1U,
			std::move(generateReflectionFunc),
			std::move(generateDiffuseFunc),
			std::move(evaluateDiffusePdfFunc),
			std::move(generateSampleFunc)));
}

color_rgbx BlinnMaterial::EvaluateEmission(
	const GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const math::Sampler<space_real>& sampler) const
{
	if (diffuseMap_ == nullptr)
	{
		return emission_;
	}
	else
	{
		return emission_ * diffuseMap_->Sample(TextureCoords{hitPoint, normal, uvs});
	}
}

color_rgbx BlinnMaterial::EvaluateNonDeltaScattering(
	const GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const vector3& outgoingDirection,
	const math::Sampler<space_real>& sampler) const
{
	const auto diffuseColor = EvaluateDiffuseColor(object, hitPoint, normal, uvs[0], incidentDirection, sampler);
	return math::is_obtuse_angle(incidentDirection, normal)
		? diffuseColor * (color_rgbx::fill(1.0) - specular_) * (color_rgbx::fill(1.0) - translucency_)
		: (color_rgbx::fill(1.0) - specular_) * translucency_;
}
