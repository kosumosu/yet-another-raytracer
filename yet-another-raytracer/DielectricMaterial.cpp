#include "DielectricMaterial.h"

#include <cassert>
#include <iomanip>
#include <optional>


using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;


namespace
{
	bool refract(const vector3& incidentDirection, const vector3& normal, space_real ior, vector3& out_refractedDirection, space_real& refractedCosTheta)
	{
		const auto fixedNormal = math::dot(incidentDirection, normal) < 0 ? normal : -normal;

		const auto binormal = math::cross(fixedNormal, incidentDirection); // sine scaled
		const auto tangent = math::cross(binormal, fixedNormal); // sine scaled

		const auto refractedTangent = tangent * ior;
		const auto refractedSinSquaredTheta = math::length2(refractedTangent);

		if (refractedSinSquaredTheta > space_real(1.0))
		{
			return false; // TIR occured
		}

		refractedCosTheta = std::sqrt(space_real(1.0) - refractedSinSquaredTheta);

		out_refractedDirection = refractedTangent - fixedNormal * refractedCosTheta;
		return true;
	}

	[[maybe_unused]] bool refractPbrt(const vector3& incidentDirection, const vector3& normal, space_real ior, vector3& out_refractedDirection, space_real& refractedCosTheta)
	{
		const auto fixedNormal = math::dot(incidentDirection, normal) < 0 ? normal : -normal;

		const auto cosThetaI = -math::dot(fixedNormal, incidentDirection);
		const auto sin2ThetaI = std::max(space_real(0), space_real(1.0) - cosThetaI * cosThetaI);
		const auto sin2ThetaT = ior * ior * sin2ThetaI;

		// Handle total internal reflection for transmission
		if (sin2ThetaT >= 1)
			return false;

		const auto cosThetaT = std::sqrt(1 - sin2ThetaT);
		refractedCosTheta = cosThetaT;
		out_refractedDirection = ior * incidentDirection + (ior * cosThetaI - cosThetaT) * fixedNormal;
		return true;
	}


	[[maybe_unused]] color_real FrDielectric(space_real cosThetaI, space_real etaI, space_real etaT)
	{
		cosThetaI = std::abs(cosThetaI);

		// Compute _cosThetaT_ using Snell's law
		const auto sinThetaI = std::sqrt(std::max(space_real(0.0), space_real(1.0) - cosThetaI * cosThetaI));
		const auto sinThetaT = etaI / etaT * sinThetaI;

		// Handle total internal reflection
		if (sinThetaT >= 1)
			return 1;

		const auto cosThetaT = std::sqrt(std::max(space_real(0.0), space_real(1.0) - sinThetaT * sinThetaT));
		const auto Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
		const auto Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
		return color_real((Rparl * Rparl + Rperp * Rperp) / space_real(2.0));
	}

	class Fresnel
	{
		const vector3 incidentDirection_;
		const vector3 normal_;
		const vector3 fixedNormal_;
		const space_real cosThetaI_;
		const space_real absCosThetaI_;
		const space_real ior_;

		color_real reflectance_;
		space_real cosThetaT_;
	public:
		struct Reflection
		{
			space_real reflectedRayToNormalCos;
			vector3 reflectedDirection;
		};

		struct Refraction
		{
			space_real refractedRayToNormalCos;
			vector3 refractedDirection;
		};


		Fresnel(const vector3& incidentDirection, const vector3& normal, space_real etaI, space_real etaT)
			: incidentDirection_{incidentDirection}
			, normal_{normal}
			, fixedNormal_{math::dot(incidentDirection, normal) < 0 ? normal : -normal}
			, cosThetaI_{math::dot(incidentDirection, normal)}
			, absCosThetaI_{std::abs(cosThetaI_)}
			, ior_{etaI / etaT}
		{
			const auto sin2ThetaI = std::max(space_real(0), space_real(1.0) - absCosThetaI_ * absCosThetaI_);
			const auto sin2ThetaT = ior_ * ior_ * sin2ThetaI;

			if (sin2ThetaT >= space_real(1.0))
			{
				reflectance_ = color_real(1.0);
			}
			else
			{
				cosThetaT_ = std::sqrt(std::max(space_real(0.0), space_real(1.0) - sin2ThetaT));
				const auto parallelPolarizationAmplitude = ((etaT * absCosThetaI_) - (etaI * cosThetaT_)) / ((etaT * absCosThetaI_) + (etaI * cosThetaT_));
				const auto perpendicularPolarizationAmplitude = ((etaI * absCosThetaI_) - (etaT * cosThetaT_)) / ((etaI * absCosThetaI_) + (etaT * cosThetaT_));

				reflectance_ = color_real(
					(parallelPolarizationAmplitude * parallelPolarizationAmplitude + perpendicularPolarizationAmplitude * perpendicularPolarizationAmplitude) /
					space_real(2.0));
			}
		}

		[[nodiscard]] color_real reflectance() const
		{
			return reflectance_;
		}

		[[nodiscard]] Reflection evaluateReflection() const
		{
			return {absCosThetaI_, incidentDirection_ - normal_ * (space_real(2.0) * cosThetaI_)};
		}

		[[nodiscard]] Refraction evaluateRefraction() const
		{
			return {cosThetaT_, ior_ * incidentDirection_ + (ior_ * absCosThetaI_ - cosThetaT_) * fixedNormal_};
		}
	};
}

color_real DielectricMaterial::GetEmissionImportance() const
{
	return color_real(0.0);
}

void DielectricMaterial::WithBsdfDistribution(
	const objects::GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const math::Sampler<space_real>& sampler,
	const bsdf_distribution_func& job) const
{
	job(
		bsdf_functional_distribution(
			2U,
			// generate delta sample
			[&]()
			{
				const auto cosTheta = -math::dot(incidentDirection, normal);
				const bool entering = cosTheta > space_real(0.0);

				const space_real iorFrom = entering ? _iorOutside : _iorInside;
				const space_real iorTo = entering ? _iorInside : _iorOutside;

				const Fresnel fresnel{incidentDirection, normal, iorFrom, iorTo};

				const auto reflectance = fresnel.reflectance();
				
				// a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
				const auto randomNumber = sampler.Get1D();
				const bool doReflectance = randomNumber < reflectance;

				if (doReflectance)
				{
					const auto reflection = fresnel.evaluateReflection();

					return math::random_sample<const bsdf_sample, space_real>(
						bsdf_sample(
							reflection.reflectedDirection,
							[=]()
							{
								return color_rgbx::fill(reflectance / std::abs(color_real(reflection.reflectedRayToNormalCos)));
							}),
						space_real(reflectance),
						true);
				}
				else
				{
					const auto transmission = color_real(1.0) - reflectance;
					const auto refraction = fresnel.evaluateRefraction();

					assert(math::length2(refraction.refractedDirection) <= space_real(1.00001));

					return math::random_sample<const bsdf_sample, space_real>(
						bsdf_sample(
							refraction.refractedDirection,
							[=, this]()
							{
								return surfaceTransparency_ * color_real(
									transmission * color_real((iorFrom * iorFrom) / (iorTo * iorTo)) / std::abs(
										color_real(refraction.refractedRayToNormalCos)));
							}),
						space_real(transmission),
						true);
				}
			}));
}

color_rgbx DielectricMaterial::EvaluateEmission(
	const objects::GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const math::Sampler<space_real>& sampler) const
{
	return color_rgbx::zero();
}

color_rgbx DielectricMaterial::EvaluateNonDeltaScattering(
	const objects::GeometryObject& object,
	const vector3& hitPoint,
	const vector3& normal,
	const uvs_t& uvs,
	const vector3& incidentDirection,
	const vector3& outgoingDirection,
	const math::Sampler<space_real>& sampler) const
{
	return color_rgbx::zero();
}
