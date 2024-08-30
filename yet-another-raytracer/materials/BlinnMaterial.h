#pragma once

#include "Material.h"
#include "Types.h"

#include <utility>

class Texture;

namespace materials {

class BlinnMaterial final : public Material
{
	color_rgb emission_;
	const Texture* diffuseMap_;
	color_rgb diffuse_;
	color_rgb specular_;
	color_real shininess_;
	color_rgb translucency_;

public:
	BlinnMaterial()
		: emission_(color_rgb::zero())
		, diffuseMap_(nullptr)
		, diffuse_(color_rgb::zero())
		, specular_(color_rgb::zero())
		, shininess_(1.0f)
		, translucency_(color_rgb::zero())
	{
	}


	BlinnMaterial(color_rgb emission, const Texture* diffuseMap, color_rgb diffuse, color_rgb specular, color_real shininess, color_rgb translucency)
		: emission_(std::move(emission))
		, diffuseMap_(diffuseMap)
		, diffuse_(std::move(diffuse))
		, specular_(std::move(specular))
		, shininess_(shininess)
		, translucency_(std::move(translucency))
	{
	}


	const color_rgb& emission() const { return emission_; }
	void emission(const color_rgb& value) { emission_ = value; }

	const color_rgb& diffuse() const { return diffuse_; }
	void diffuse(const color_rgb& value) { diffuse_ = value; }

	const color_rgb& specular() const { return specular_; }
	void specular(const color_rgb& value) { specular_ = value; }

	color_real shininess() const { return shininess_; }
	void shininess(color_real value) { shininess_ = value; }

	color_rgb translucency() const { return translucency_; }
	void translucency(const color_rgb& value) { translucency_ = value; }

	color_real GetEmissionImportance() const override;

	void WithBsdfDistribution(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler,
		const bsdf_distribution_func& job) const override;

	color_rgb EvaluateEmission(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const override;

	color_rgb EvaluateNonDeltaScattering(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const vector3& outgoingDirection,
		const math::Sampler<space_real>& sampler) const override;

private:
	color_real GetReflectionProbability() const;

	color_rgb EvaluateDiffuseColor(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const vector2& uv,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const;
};

}