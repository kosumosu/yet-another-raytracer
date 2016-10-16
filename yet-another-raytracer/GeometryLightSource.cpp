#include "GeometryLightSource.h"
#include "Material.h"
#include "LightingContext.h"

using lighting_functional_distribution = FunctionalDistribution<const light_sample, const vector3, space_real>;

GeometryLightSource::GeometryLightSource(const ObjectCollection & objects)
{
	std::vector<std::pair<GeometryObject*, color_real>> objectsWithWeights;

	color_real totalPower = color_real(0);
	for (const auto object : objects)
	{
		const auto power = color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetEmissionImportance();
		totalPower += power;
		if (power > color_real(0.0))
		{
			objectsWithWeights.push_back(std::make_pair(object.get(), power));
			_objects.push_back(object.get());
		}
	}

	_totalPower = totalPower;
	_distribution = math::discrete_distribution<GeometryObject*, color_real>(std::begin(objectsWithWeights), std::end(objectsWithWeights));
}

void GeometryLightSource::DoWithDistribution(const LightingContext & context, math::UniformRandomBitGenerator<unsigned> & randomEngine, const distibution_func & job) const
{
	std::uniform_real_distribution<color_real> distr(color_real(0.0), upperRandomBound<color_real>()); // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
	const auto randomFunc = [&]()
	{
		return color_real(distr(randomEngine));
	};
	job(lighting_functional_distribution(
		[&]()
		{
			const auto objectSample = _distribution.GetRandomElement(randomFunc);
			const auto pointSample = objectSample.getValue()->PickRandomPointOnSurface(randomEngine);

			const auto pointToLight = pointSample.getValue().point - context.getPoint();
			const space_real distance = math::length(pointToLight);
			const auto direction = pointToLight / distance;

			const auto geometricFactor = std::max(color_real(0), color_real(-math::dot(pointSample.getValue().normal, direction)));

			const auto pdf = objectSample.getPdf() * pointSample.getPdf() * (distance * distance) / geometricFactor;

			return math::random_sample<const light_sample, space_real>(
				light_sample(
					direction,
					distance,
					[=, &randomEngine]()
					{
						return objectSample.getValue()->material()->EvaluateEmission(*objectSample.getValue(), pointSample.getValue().point, pointSample.getValue().normal, pointSample.getValue().uvs, direction, randomEngine);
					}
				),
				pdf,
				false);
		},
		[&](const vector3 & sample)
		{
			ray3 ray(context.getPoint(), sample);
			space_real pdf = space_real(0);
			for (const auto & object : _objects)
			{
				const auto hit = object->FindHit(ray, context.getBias(), std::numeric_limits<space_real>::max());
				if (hit.has_occurred())
				{
					const auto power = color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetEmissionImportance();
					const auto powerPdf = power / _totalPower;
					const auto surfaceAreaPdf = object->GetOneSidedSurfaceArea();

					const auto geometricFactor = std::max(color_real(0), color_real(-math::dot(hit.normal(), ray.direction())));
					const auto distanceFactor = color_real(hit.distance() * hit.distance());

					pdf += powerPdf * surfaceAreaPdf * distanceFactor / geometricFactor;
				}
			}

			return pdf;
		}
	));
}
