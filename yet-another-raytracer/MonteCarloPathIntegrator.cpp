#include "MonteCarloPathIntegrator.h"
#include "Raytracer.h"
#include "GeometryObject.h"
#include "LightSource.h"
#include "Material.h"
#include "LightingContext.h"

constexpr space_real BIAS = std::numeric_limits<space_real>::epsilon() * space_real(32768.0);

color_rgbx MonteCarloPathIntegrator::EvaluateRadianceByLightsAtVertex(const ray3 & currentRay, const Hit & hit, const bsdf_distribution & bsdfDistribution, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	color_rgbx radianceAtCurrentPathVertex(0);
	const LightingContext context(hit.point(), hit.normal(), BIAS, 1, false);
	for (const auto & light : _lights)
	{
		light->DoWithDistribution(context, randomEngine,
			[&](const light_distribution & lightDistribution)
			{
				if (lightDistribution.delta_components() != 0 || lightDistribution.has_non_delta_component())
				{
					const auto lightSample = lightDistribution.generate_sample();

					const bool isOccluded = _raytracer->DoesIntersect(ray3(hit.point(), lightSample.getValue().direction), BIAS, lightSample.getValue().distance - BIAS * lightSample.getValue().distance);

					if (!isOccluded)
					{
						const auto geometricTerm = color_real(std::max(space_real(0.0), math::dot(lightSample.getValue().direction, hit.normal())));
						radianceAtCurrentPathVertex +=
							lightSample.getValue().evaluate()
							* hit.object()->material()->EvaluateNonDeltaScattering(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), lightSample.getValue().direction, randomEngine)
							* geometricTerm
							* color_real(math::oneOverPi)
							/ color_real(lightSample.getPdf());
					}
				}
			});
	}

	return radianceAtCurrentPathVertex;
}

color_rgbx MonteCarloPathIntegrator::EvaluateRay(const ray3 & ray, unsigned bounceLimit, space_real bias, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
{
	color_rgbx integral(0);
	color_rgbx throughput(1);

	ray3 currentRay = ray;
	bool accountForEmission = true;
	space_real currentBias = bias;
	bool earlyExit = false;

	Hit hit = _raytracer->TraceRay(ray, bias);

	if (hit.has_occurred())


		for (unsigned bounce = 0; bounce < bounceLimit - 1; ++bounce)
		{
			if (hit.has_occurred())
			{
				hit.object()->material()->WithBsdfDistribution(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine,
					            [&](const bsdf_distribution & bsdfDistribution)
					            {
						            color_rgbx radianceAtCurrentPathVertex(0);

						            if (bsdfDistribution.has_non_delta_component())
							            radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(currentRay, hit, bsdfDistribution, randomEngine);

						            if (accountForEmission)
							            radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine);

						            integral += radianceAtCurrentPathVertex * throughput;

						            const auto bsdfSample = bsdfDistribution.generate_sample();
						            const auto bsdfDirection = bsdfSample.getValue().outgoingDirection;
						            const auto bsdfColor = bsdfSample.getValue().evaluate();
						            const auto bsdfPdf = color_real(bsdfSample.getPdf());

						            const auto geometricTerm = color_real(std::max(space_real(0.0), math::dot(bsdfDirection, hit.normal())));
						            throughput *= bsdfColor * geometricTerm * color_real(math::oneOverPi) / bsdfPdf;
						            currentRay = ray3(hit.point(), bsdfDirection);
						            hit = _raytracer->TraceRay(currentRay, BIAS);
						            accountForEmission = !bsdfDistribution.has_non_delta_component();
					            }
				            );
			}
			else
			{
				integral += _infinityEvaluator(currentRay) * throughput;
				earlyExit = true;
				break;
			}
		}

	// last bounce is a special case
	if (bounceLimit > 0 && !earlyExit)
	{
		if (hit.has_occurred())
		{
			if (bounceLimit == 1)
			{
				// account for lighting for the first ray
				integral += hit.object()->material()->EvaluateEmission(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine);
			}

			hit.object()->material()->WithBsdfDistribution(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine,
				            [&](const bsdf_distribution & bsdfDistribution)
				            {
					            const auto radianceAtCurrentPathVertex = EvaluateRadianceByLightsAtVertex(currentRay, hit, bsdfDistribution, randomEngine);

					            integral += radianceAtCurrentPathVertex * throughput;
				            }
			            );
		}
		else
		{
			integral += _infinityEvaluator(currentRay) * throughput;
		}
	}


	return integral;
}
