#pragma once

#include "Types.h"
#include "UniformRandomBitGenerator.h"
#include "Distribution.h"
#include <functional>

class GeometryObject;

struct bsdf_sample
{
	bsdf_sample(const vector3 & outgoingDirection, const std::function<color_rgbx()> & evaluate)
		: outgoingDirection(outgoingDirection),
		  evaluate(evaluate) {}

	vector3 outgoingDirection;
	std::function<color_rgbx()> evaluate;
};

using bsdf_distribution = Distribution<const bsdf_sample, const vector3, space_real>;
using bsdf_distribution_func = std::function<void(const bsdf_distribution & distribution)>;

class Material
{
public:

	Material(void) { }

	virtual ~Material(void) { }

	// first integrate over hemisphere, then find average across averywhere
	virtual color_real GetEmissionImportance() const = 0;

	virtual void WithBsdfDistribution(
		const GeometryObject & object,
		const vector3 & hitPoint,
		const vector3 & normal,
		const vector3 & incidentDirection,
		math::UniformRandomBitGenerator<unsigned int> & randomEngine,
		const bsdf_distribution_func & job) const = 0;

	virtual color_rgbx EvaluateEmission(
		const GeometryObject & object,
		const vector3 & hitPoint,
		const vector3 & normal,
		const vector3 & incidentDirection,
		math::UniformRandomBitGenerator<unsigned int> & randomEngine) const = 0;

	virtual color_rgbx EvaluateNonDeltaScattering(
		const GeometryObject & object,
		const vector3 & hitPoint,
		const vector3 & normal,
		const vector3 & incidentDirection,
		const vector3 & outgoingDirection,
		math::UniformRandomBitGenerator<unsigned int> & randomEngine) const = 0;

	virtual Material * Clone() const = 0;
};
