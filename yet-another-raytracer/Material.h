#pragma once

#include "UniformRandomBitGenerator.h"
#include "Distribution.h"
#include "Types.h"
#include <functional>
#include <utility>

class GeometryObject;

struct bsdf_sample
{
	bsdf_sample(vector3 outgoingDirection, std::function<color_rgbx()> evaluate)
		: outgoingDirection(std::move(outgoingDirection))
		, evaluate(std::move(evaluate))
	{
	}

	vector3 outgoingDirection;
	std::function<color_rgbx()> evaluate;
};

using bsdf_distribution = Distribution<const bsdf_sample, const vector3, space_real>;
using bsdf_distribution_func = std::function<void(const bsdf_distribution& distribution)>;

class Material
{
public:
	virtual ~Material() = default;

	// first integrate over hemisphere, then find average across averywhere
	[[nodiscard]] virtual color_real GetEmissionImportance() const = 0;

	virtual void WithBsdfDistribution(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		math::UniformRandomBitGenerator<unsigned int>& randomEngine,
		const bsdf_distribution_func& job) const = 0;

	virtual color_rgbx EvaluateEmission(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		math::UniformRandomBitGenerator<unsigned int>& randomEngine) const = 0;

	virtual color_rgbx EvaluateNonDeltaScattering(
		const GeometryObject& object,
		const vector3& hitPoint,
		const vector3& normal,
		const uvs_t& uvs,
		const vector3& incidentDirection,
		const vector3& outgoingDirection,
		math::UniformRandomBitGenerator<unsigned int>& randomEngine) const = 0;
};
