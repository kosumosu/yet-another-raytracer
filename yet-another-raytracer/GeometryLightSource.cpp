#include "GeometryLightSource.h"
#include "Material.h"
#include "LightingContext.h"
#include "RayEvaluator.h"
#include "Flux.h"
#include "ShadingContext.h"

GeometryLightSource::GeometryLightSource(const ObjectCollection & objects, size_t sampleCount)
	: _sampleCount(sampleCount)
{
	std::vector<std::pair<GeometryObject*, color_real>> objectsWithWeights;

	for (const auto object : objects)
	{
		const auto power = color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetAverageEmission();
		if (power > color_real(0.0))
			objectsWithWeights.push_back(std::make_pair(object.get(), power));
	}

	_distribution = math::discrete_distribution<GeometryObject*, color_real>(std::begin(objectsWithWeights), std::end(objectsWithWeights));
}

void GeometryLightSource::IterateOverFluxes(const LightingContext & context, const RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, const flux_func & job) const
{
	size_t actualSampleCount = context.getAllowSubdivision() ? _sampleCount : 1;
	color_real sampleWeight = color_real(1.0 / actualSampleCount);

	std::uniform_real_distribution<space_real> distr;
	for (size_t i = 0; i < actualSampleCount; ++i)
	{
		const auto objectSample = _distribution.GetRandomElement([&]() { return color_real(distr(randomEngine)); });
		const auto pointSample =  objectSample.getValue()->PickRandomPointOnSurface(randomEngine);

		const auto pointToLight = pointSample.getValue() - context.getPoint();
		const auto direction = math::normalize(pointToLight);

		const auto ray = ray3(context.getPoint(), direction);
		const auto hit = rayEvaluator.raytracer()->TraceRay(ray, context.getBias(), math::length(pointToLight) * space_real(1.001));

		if (hit.has_occurred() && hit.object() == objectSample.getValue())
		{
			ShadingContext subContext;
			subContext.normal(hit.normal());
			subContext.object(hit.object());
			subContext.incident_ray(ray);

			const auto geometricFactor = std::max(color_real(0), color_real(-math::dot(hit.normal(), ray.direction())));
			const auto irradiance = objectSample.getValue()->material()->GetEmission(subContext) / color_real(hit.distance() * hit.distance()) * geometricFactor;

			Flux flux(this, direction, irradiance, std::numeric_limits<space_real>::max(), objectSample.getPdf() * color_real(pointSample.getPdf() * actualSampleCount));
			job(flux);
		}
	}
}
