#include "GeometryLightSource.h"
#include "materials/Material.h"
#include "LightingContext.h"

namespace lights
{
    using lighting_functional_distribution = FunctionalDistribution<std::optional<light_sample>, vector3, space_real>;

    GeometryLightSource::GeometryLightSource(const ObjectCollection& objects)
    {
        std::vector<std::pair<const objects::GeometryObject*, color_real>> objectsWithWeights;

        color_real totalPower = {0};
        for (const auto& object : objects)
        {
            const auto power = color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetEmissionImportance();
            totalPower += power;
            if (power > color_real(0.0))
            {
                objectsWithWeights.emplace_back(object.get(), power);
                _objects.push_back(object.get());
            }
        }

        _totalPower = totalPower;
        _distribution = math::discrete_distribution<const objects::GeometryObject*, color_real>(
            std::begin(objectsWithWeights), std::end(objectsWithWeights));
    }

    void GeometryLightSource::DoWithDistribution(const LightingContext& context, math::Sampler<space_real>& sampler,
                                                 const distibution_func& job) const noexcept
    {
        DoWithDistributionImpl(context.getPoint(), context.object(), sampler, job);
    }

    void GeometryLightSource::DoWithDistribution(const vector3& point, math::Sampler<space_real>& sampler,
                                                 const distibution_func& job) const noexcept
    {
        DoWithDistributionImpl(point, nullptr, sampler, job);
    }

    void GeometryLightSource::DoWithDistributionImpl(const vector3& point,
                                                     const objects::GeometryObject* illuminated_object,
                                                     math::Sampler<space_real>& sampler,
                                                     const distibution_func& job) const noexcept
    {
        const auto randomFunc = [&]()
        {
            return color_real(sampler.Get1D());
        };

        // auto evaluatePdf = [&](const vector3 & sample)
        // {
        // 	ray3 ray(context.getPoint(), sample);
        // 	space_real pdf = {0};
        // 	for (const auto & object : _objects)
        // 	{
        // 		const auto hit = object->FindHit(ray, context.getBias(), std::numeric_limits<space_real>::max());
        // 		if (hit.has_occurred())
        // 		{
        // 			const auto power = color_real(object->GetOneSidedSurfaceArea()) * object->material()->GetEmissionImportance();
        // 			const auto powerPdf = power / _totalPower;
        // 			const auto surfaceAreaPdf = object->GetOneSidedSurfaceArea();
        //
        // 			const auto geometricFactor = std::max(color_real(0), color_real(-math::dot(hit.normal(), ray.direction())));
        // 			const auto distanceFactor = color_real(hit.distance() * hit.distance());
        //
        // 			pdf += powerPdf * surfaceAreaPdf * distanceFactor / geometricFactor;
        // 		}
        // 	}
        //
        // 	return pdf;
        // };

        job(lighting_functional_distribution(
            0U,
            true,
            [&]()
            {
                const auto objectSample = _distribution.GetRandomElement(randomFunc);

                const auto optionalPointSample = (objectSample.getValue() == illuminated_object)
                                                     ? objectSample.getValue()->PickRandomPointOnSurfaceForLighting(
                                                         point, sampler)
                                                     : std::make_optional(
                                                         objectSample.getValue()->PickRandomPointOnSurface(sampler));

                if (optionalPointSample)
                {
                    const auto& pointSample = *optionalPointSample;

                    const auto pointToLight = pointSample.getValue().point - point;
                    const space_real distance = math::length(pointToLight);
                    const auto direction = pointToLight / distance;

                    const auto geometricFactor = std::max(color_real(0),
                                                          color_real(-math::dot(pointSample.getValue().normal, direction)));

                    if (geometricFactor > space_real(0.0))
                    {
                        const auto pdf = objectSample.getPdf() * pointSample.getPdf() * (distance * distance) /
                            geometricFactor;

                        assert(std::isfinite(pdf));
                        assert(pdf > 1e-5f);

                        return math::random_sample<std::optional<light_sample>, space_real>(
                            light_sample(
                                direction,
                                distance,
                                [=, &sampler]()
                                {
                                    return objectSample.getValue()->material()->EvaluateEmission(
                                        *objectSample.getValue(), pointSample.getValue().point,
                                        pointSample.getValue().normal, pointSample.getValue().uvs, direction, sampler);
                                }
                            ),
                            pdf,
                            false);
                    }
                }

                return math::random_sample<std::optional<light_sample>, space_real>(
                    std::nullopt,
                    objectSample.getPdf(),
                    false);
            }
        ));
    }

    color_real GeometryLightSource::GetApproximateTotalPower() const noexcept
    {
        return _totalPower;
    }
}