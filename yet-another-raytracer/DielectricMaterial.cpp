#include "DielectricMaterial.h"
#include "ShadingContext.h"


using bsdf_functional_distribution = FunctionalDistribution<const bsdf_sample, const vector3, space_real>;


inline bool refract(const vector3 & incidentDirection, const vector3 & normal, space_real ior, vector3 & out_refractedDirection)
{
	// borrowed from pbrt

	//auto cosThetaI = -math::dot(normal, incidentDirection);
	//auto sin2ThetaI = std::max(space_real(0), space_real(1.0) - cosThetaI * cosThetaI);
	//auto sin2ThetaT = ior * ior * sin2ThetaI;

	//// Handle total internal reflection for transmission
	//if (sin2ThetaT >= 1)
	//	return false;

	//auto cosThetaT = std::sqrt(1 - sin2ThetaT);
	//out_refractedDirection = ior * incidentDirection + (ior * cosThetaI - cosThetaT) * normal;
	//return true;

	//out_refractedDirection = incidentDirection;
	//return true;

	auto fixedNormal = math::dot(incidentDirection, normal) < 0 ? normal : -normal;

	auto binormal = math::cross(fixedNormal, incidentDirection); // sine scaled
	auto tangent = math::cross(binormal, fixedNormal); // sine scaled

	auto refractedTangent = tangent * ior;
	auto refractedSinSquaredTheta = math::length2(refractedTangent);

	if (refractedSinSquaredTheta > space_real(1.0))
		return false; // TIR occured

	auto refractedCosTheta = std::sqrt(space_real(1.0) - refractedSinSquaredTheta);

	out_refractedDirection = refractedTangent - fixedNormal * refractedCosTheta;
	return true;
}

color_real FrDielectric(space_real cosThetaI, space_real etaI, space_real etaT)
{
	cosThetaI = std::abs(cosThetaI);

	// Compute _cosThetaT_ using Snell's law
	auto sinThetaI = std::sqrt(std::max(space_real(0.0), space_real(1.0) - cosThetaI * cosThetaI));
	auto sinThetaT = etaI / etaT * sinThetaI;

	// Handle total internal reflection
	if (sinThetaT >= 1)
		return 1;

	auto cosThetaT = std::sqrt(std::max(space_real(0.0), space_real(1.0) - sinThetaT * sinThetaT));
	auto Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	auto Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
		((etaI * cosThetaI) + (etaT * cosThetaT));
	return color_real((Rparl * Rparl + Rperp * Rperp) / space_real(2.0));
}

color_rgbx DielectricMaterial::Shade(const ShadingContext & context) const
{
	return DielectricMaterial::GetScattering(context);
}

color_rgbx DielectricMaterial::GetEmission(const ShadingContext & context) const
{
	return color_rgbx();
}

color_rgbx DielectricMaterial::GetScattering(const ShadingContext & context) const
{
	const auto cosTheta = -math::dot(context.incident_ray().direction(), context.normal());
	const bool entering = cosTheta > space_real(0.0);

	const space_real iorIn = entering ? m_iorOutside : m_iorInside;
	const space_real iorOut = entering ? m_iorInside : m_iorOutside;

	const auto reflectedDirection = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));

	const auto reflectance = FrDielectric(cosTheta, iorIn, iorOut);
	const auto transmission = color_real(1.0) - reflectance;

	color_rgbx totalColor;
	totalColor += reflectance * context.ray_evaluator()->TraceRay(ray3(context.world_space_hit_point(), reflectedDirection), context.trace_depth(), context.bias(), context.allow_subdivision(), true);


	if (transmission > color_real(0.0))
	{
		vector3 refractedDirection;
		if (refract(context.incident_ray().direction(), context.normal(), iorIn / iorOut, refractedDirection))
		{
			totalColor +=
				transmission
				* color_real((iorIn * iorIn) / (iorOut * iorOut))
				* context.ray_evaluator()->TraceRay(ray3(context.world_space_hit_point(), refractedDirection), context.trace_depth(), context.bias(), context.allow_subdivision(), true);
		}
	}

	return totalColor;
}

color_real DielectricMaterial::GetEmissionImportance() const
{
	return color_real(0.0);
}

Material * DielectricMaterial::Clone() const
{
	return new DielectricMaterial(m_iorInside, m_iorOutside);
}

void DielectricMaterial::WithBsdfDistribution(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine, const bsdf_distribution_func & job) const
{
	job(bsdf_functional_distribution(
		2U,
		[&](const bsdf_functional_distribution::delta_func & subJob)
		{
			const auto cosTheta = -math::dot(incidentDirection, normal);
			const bool entering = cosTheta > space_real(0.0);

			const space_real iorIn = entering ? m_iorOutside : m_iorInside;
			const space_real iorOut = entering ? m_iorInside : m_iorOutside;

			const auto reflectedDirection = incidentDirection - normal * (space_real(2.0) * math::dot(incidentDirection, normal));

			const auto reflectance = FrDielectric(cosTheta, iorIn, iorOut);
			const auto transmission = color_real(1.0) - reflectance;

			subJob(math::random_sample<const bsdf_sample, space_real>(
				bsdf_sample(
					reflectedDirection,
					[&]()
					{
						return color_rgbx(reflectance);
					}
				),
				space_real(1.0),
				true));

			if (transmission > color_real(0.0))
			{
				vector3 refractedDirection;
				if (refract(incidentDirection, normal, iorIn / iorOut, refractedDirection))
				{
					subJob(math::random_sample<const bsdf_sample, space_real>(
						bsdf_sample(
							refractedDirection,
							[&]()
							{
								return color_rgbx(transmission * color_real((iorIn * iorIn) / (iorOut * iorOut)));
							}
						),
						space_real(1.0),
						true));
				}
			}
		},
		[&]()
		{
			const auto cosTheta = -math::dot(incidentDirection, normal);
			const bool entering = cosTheta > space_real(0.0);

			const space_real iorIn = entering ? m_iorOutside : m_iorInside;
			const space_real iorOut = entering ? m_iorInside : m_iorOutside;

			const auto reflectedDirection = incidentDirection - normal * (space_real(2.0) * math::dot(incidentDirection, normal));

			const auto reflectance = FrDielectric(cosTheta, iorIn, iorOut);
			const auto transmission = color_real(1.0) - reflectance;

			std::uniform_real_distribution<color_real> distr;
			const bool doReflectance = distr(randomEngine) <= reflectance;

			if (doReflectance)
			{
				return math::random_sample<const bsdf_sample, space_real>(
					bsdf_sample(
						reflectedDirection,
						[&]()
						{
							return color_rgbx(reflectance);
						}),
					space_real(reflectance),
					true
				);
			}
			else
			{
				vector3 refractedDirection;
				refract(incidentDirection, normal, iorIn / iorOut, refractedDirection);

				return math::random_sample<const bsdf_sample, space_real>(
					bsdf_sample(
						refractedDirection,
						[&]()
						{
							return color_rgbx(transmission * color_real((iorIn * iorIn) / (iorOut * iorOut)));
						}),
					space_real(transmission),
					true
				);
			}
		}));
}

color_rgbx DielectricMaterial::EvaluateEmission(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	return color_rgbx();
}

color_rgbx DielectricMaterial::EvaluateNonDeltaScattering(const GeometryObject & object, const vector3 & hitPoint, const vector3 & normal, const vector3 & incidentDirection, const vector3 & outgoingDirection, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	return color_rgbx();
}
