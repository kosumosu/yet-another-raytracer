#include "MonteCarloPathIntegrator.h"
#include "Raytracer.h"
#include "GeometryObject.h"
#include "LightSource.h"
#include "Material.h"
#include "LightingContext.h"
#include "color_functions.hpp"
#include <iostream>
#include <iomanip>

constexpr space_real BIAS = std::numeric_limits<space_real>::epsilon() * space_real(32768.0);
//constexpr space_real BIAS = space_real(0); // std::numeric_limits<space_real>::epsilon() * space_real(32768.0);


color_rgbx MonteCarloPathIntegrator::EvaluateRadianceByLightsAtVertex(const ray3 & currentRay, const Hit & hit, bool entering, const bsdf_distribution & bsdfDistribution, math::UniformRandomBitGenerator<unsigned> & randomEngine) const
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

					const bool isOccluded = _raytracer->DoesIntersect(ray3(hit.point(), lightSample.getValue().direction), BIAS, lightSample.getValue().distance - BIAS * lightSample.getValue().distance, hit.object(), hit.normal());

					if (!isOccluded)
					{
						const auto geometricTerm = color_real(std::abs(math::dot(lightSample.getValue().direction, hit.normal())));
						const auto radianceByLight =
							lightSample.getValue().evaluate()
							* hit.object()->material()->EvaluateNonDeltaScattering(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), lightSample.getValue().direction, randomEngine)
							* geometricTerm
							* color_real(math::oneOverPi)
							/ color_real(lightSample.getPdf());

						//if (radianceByLight[0] < color_real(0.0) || radianceByLight[1] < color_real(0.0) || radianceByLight[2] < color_real(0.0))
						//	throw std::exception();

						radianceAtCurrentPathVertex += radianceByLight;
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
	bool earlyExit = false;

	Hit hit = _raytracer->TraceRay(ray, bias);

	for (unsigned bounce = 0; bounce < bounceLimit - 1; ++bounce)
	{
		if (hit.has_occurred())
		{
			hit.object()->material()->WithBsdfDistribution(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine,
				            [&](const bsdf_distribution & bsdfDistribution)
				            {
								const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());
					            color_rgbx radianceAtCurrentPathVertex(0);

					            if (bsdfDistribution.has_non_delta_component())
						            radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(currentRay, hit, entering, bsdfDistribution, randomEngine);

					            if (accountForEmission)
						            radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine);

					            integral += radianceAtCurrentPathVertex * throughput;

					            if (!bsdfDistribution.has_non_delta_component() && bsdfDistribution.delta_components() == 0)
					            {
						            earlyExit = true;
									return;
					            }

					            const auto bsdfSample = bsdfDistribution.generate_sample();
					            const auto bsdfDirection = bsdfSample.getValue().outgoingDirection;
					            const auto bsdfColor = bsdfSample.getValue().evaluate();
					            const auto bsdfPdf = color_real(bsdfSample.getPdf());

					            const auto geometricTerm = color_real(std::abs(math::dot(bsdfDirection, hit.normal())));

								const auto vertexThroughput = bsdfSample.getIsDelta()
						                          ? bsdfColor * geometricTerm / bsdfPdf
						                          : bsdfColor * geometricTerm * color_real(math::oneOverPi) / bsdfPdf;

								const auto importance = std::max(color_real(0.75), color::get_importance(vertexThroughput));

								std::uniform_real_distribution<color_real> distr(color_real(0.0), upperRandomBound<color_real>()); // a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
								if (importance < color_real(1.0) && distr(randomEngine) >= importance)
								{
									earlyExit = true;
									return;
								}
								else
								{
									throughput *= vertexThroughput / std::min(color_real(1.0), importance);
								}

								//throughput *= vertexThroughput;

#if false
								if (
									vertexThroughput[0] < color_real(0.0) || vertexThroughput[1] < color_real(0.0) || vertexThroughput[2] < color_real(0.0)
									|| std::isnan(vertexThroughput[0]) || std::isnan(vertexThroughput[1]) || std::isnan(vertexThroughput[2])
									)
								{
									std::cout << typeid(*hit.object()->material()).name() << "\n";
									std::cout << std::setprecision(std::numeric_limits<space_real>::max_digits10 + 1);
									std::cout << vertexThroughput[0] << " " << vertexThroughput[1] << " " << vertexThroughput[2] << "\n";
									std::cout << "hit point " << hit.point()[0] << " " << hit.point()[1] << " " << hit.point()[2] << "\n";
									std::cout << "hit normal " << hit.normal()[0] << " " << hit.normal()[1] << " " << hit.normal()[2] << "\n";
									std::cout << "ray direction " << currentRay.direction()[0] << " " << currentRay.direction()[1] << " " << currentRay.direction()[2] << "\n";
									std::cout << "ray origin " << currentRay.origin()[0] << " " << currentRay.origin()[1] << " " << currentRay.origin()[2] << "\n";
									std::cout << "bsdfPdf " << bsdfPdf << "\n";
									throw std::exception();
								}
#endif
								

					            currentRay = ray3(hit.point(), bsdfDirection);
					            hit = _raytracer->TraceRay(currentRay, BIAS, std::numeric_limits<space_real>::max(), hit.object(), hit.normal());
					            accountForEmission = bsdfSample.getIsDelta();
				            }
			            );
			if (earlyExit)
				break;
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
			hit.object()->material()->WithBsdfDistribution(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine,
				            [&](const bsdf_distribution & bsdfDistribution)
				            {
								const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());
					            color_rgbx radianceAtCurrentPathVertex(0);
					            if (bsdfDistribution.has_non_delta_component())
						            radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(currentRay, hit, entering, bsdfDistribution, randomEngine);

					            if (accountForEmission)
						            radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(*hit.object(), hit.point(), hit.normal(), currentRay.direction(), randomEngine);

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
