#include "DielectricMaterial.h"
#include "ShadingContext.h"

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
	auto cosTheta = -math::dot(context.incident_ray().direction(), context.normal());
	bool entering = cosTheta > space_real(0.0);

	space_real iorIn = entering ? m_iorOutside : m_iorInside;
	space_real iorOut = entering ? m_iorInside : m_iorOutside;

	auto reflectedDirection = context.incident_ray().direction() - context.normal() * (space_real(2.0) * math::dot(context.incident_ray().direction(), context.normal()));
	
	auto reflectance =  FrDielectric(cosTheta, iorIn, iorOut);
	
	color_rgbx totalColor;
	totalColor += reflectance * context.ray_evaluator()->TraceRay(Ray(context.world_space_hit_point(), reflectedDirection), context.trace_depth(), context.bias(), context.allow_subdivision());

	auto transmission = color_real(1.0) - reflectance;

	if (transmission > color_real(0.0))
	{
		vector3 refractedDirection;
		if (refract(context.incident_ray().direction(), context.normal(), iorIn / iorOut, refractedDirection))
		{
			totalColor +=
				transmission
				* color_real((iorIn * iorIn) / (iorOut * iorOut))
				* context.ray_evaluator()->TraceRay(Ray(context.world_space_hit_point(), refractedDirection), context.trace_depth(), context.bias(), context.allow_subdivision());
		}
	}

	return totalColor;
}

Material * DielectricMaterial::Clone() const
{
	return new DielectricMaterial(m_iorInside, m_iorOutside);
}
