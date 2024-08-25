#pragma once

#include "Material.h"
#include "Types.h"

#include <utility>

class Texture;

class BlinnMaterial final : public Material
{
	color_rgbx emission_;
	const Texture* diffuseMap_;
	color_rgbx diffuse_;
	color_rgbx specular_;
	color_real shininess_;
	color_rgbx translucency_;

public:
	BlinnMaterial()
		: emission_(color_rgbx::zero())
		, diffuseMap_(nullptr)
		, diffuse_(color_rgbx::zero())
		, specular_(color_rgbx::zero())
		, shininess_(1.0f)
		, translucency_(color_rgbx::zero())
	{
	}


	BlinnMaterial(color_rgbx emission, const Texture* diffuseMap, color_rgbx diffuse, color_rgbx specular, color_real shininess, color_rgbx translucency)
		: emission_(std::move(emission))
		, diffuseMap_(diffuseMap)
		, diffuse_(std::move(diffuse))
		, specular_(std::move(specular))
		, shininess_(shininess)
		, translucency_(std::move(translucency))
	{
	}


	const color_rgbx& emission() const { return emission_; }
	void emission(const color_rgbx& value) { emission_ = value; }

	const color_rgbx& diffuse() const { return diffuse_; }
	void diffuse(const color_rgbx& value) { diffuse_ = value; }

	const color_rgbx& specular() const { return specular_; }
	void specular(const color_rgbx& value) { specular_ = value; }

	color_real shininess() const { return shininess_; }
	void shininess(color_real value) { shininess_ = value; }

	color_rgbx translucency() const { return translucency_; }
	void translucency(const color_rgbx& value) { translucency_ = value; }

	color_real GetEmissionImportance() const override;

	void WithBsdfDistribution(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler,
		const bsdf_distribution_func& job) const override;

	color_rgbx EvaluateEmission(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const override;

	color_rgbx EvaluateNonDeltaScattering(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const vector3& outgoingDirection,
		const math::Sampler<space_real>& sampler) const override;

private:
	color_real GetReflectionProbability() const;

	color_rgbx EvaluateDiffuseColor(
		const objects::GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const vector2& uv,
		const vector3& incidentDirection,
		const math::Sampler<space_real>& sampler) const;
};
