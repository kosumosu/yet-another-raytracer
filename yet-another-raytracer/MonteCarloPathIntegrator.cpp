#include "MonteCarloPathIntegrator.h"

#include "GeometryObject.h"
#include "LightingContext.h"
#include "LightSource.h"
#include "Material.h"
#include "Raytracer.h"

#include "color_functions.hpp"
#include "discrete_distribution.hpp"

#include <cassert>
#include <iomanip>
#include <random>
#include <utility>

constexpr space_real BIAS = std::numeric_limits<space_real>::epsilon() * space_real(32768.0);
//constexpr space_real BIAS = space_real(0); // std::numeric_limits<space_real>::epsilon() * space_real(32768.0);


MonteCarloPathIntegrator::MonteCarloPathIntegrator(const Raytracer* raytracer, const std::vector<const LightSource*>& lights, infinity_func infinityEvaluator)
	: raytracer_(raytracer)
	, infinityEvaluator_(std::move(infinityEvaluator))
{
	std::vector<std::pair<const LightSource*, color_real>> lightsWithWeights;
	lightsWithWeights.reserve(lights.size());

	color_real totalPower = {0};
	for (const auto& light : lights)
	{
		const auto power = light->GetApproximateTotalPower();
		totalPower += power;
		if (power > color_real(0.0))
		{
			lightsWithWeights.emplace_back(light, power);
		}
	}

	oneOverTotalPower_ = color_real(1.0) / totalPower;

	lightDistribution_ = math::discrete_distribution<const LightSource*, color_real>{std::begin(lightsWithWeights), std::end(lightsWithWeights)};
	// TODO
}


color_rgbx MonteCarloPathIntegrator::EvaluateRadianceByLightsAtVertex(
	const ray3& currentRay,
	const Hit& hit,
	bool entering,
	const bsdf_distribution& bsdfDistribution,
	math::Sampler<space_real>& sampler) const
{
	color_rgbx radianceAtCurrentPathVertex{color_rgbx::zero()};
	const LightingContext context{ hit.object(), hit.point(), hit.normal(), BIAS, 1, false };
	
	// a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
	const auto randomFunc = [&]()
	{
		return color_real(sampler.Get1D());
	};
	const auto sampledLight = lightDistribution_.GetRandomElement(randomFunc);
	const auto light = sampledLight.getValue();
	light->DoWithDistribution(
		context,
		sampler,
		[&](const light_distribution& lightDistribution)
		{
			if (lightDistribution.delta_components() != 0 || lightDistribution.has_non_delta_component())
			{
				const auto optionalLightSample = lightDistribution.generate_sample();

				if (optionalLightSample.getValue()) {
					const auto& lightSample = *optionalLightSample.getValue();
					
					const bool isOccluded = raytracer_->DoesIntersect(
						ray3(hit.point(), lightSample.direction),
						BIAS,
						lightSample.distance - BIAS * lightSample.distance,
						hit.object(),
						hit.normal());

					if (!isOccluded)
					{
						const auto geometricTerm = color_real(std::abs(math::dot(lightSample.direction, hit.normal())));
						const auto radianceByLight = lightSample.evaluate() * hit.object()->material()->EvaluateNonDeltaScattering(
							*hit.object(),
							hit.point(),
							hit.normal(),
							hit.uvs(),
							currentRay.direction(),
							lightSample.direction,
							sampler) * geometricTerm * color_real(math::oneOverPi) / color_real(optionalLightSample.getPdf());

						//if (radianceByLight[0] < color_real(0.0) || radianceByLight[1] < color_real(0.0) || radianceByLight[2] < color_real(0.0))
						//	throw std::exception();

						radianceAtCurrentPathVertex = radianceByLight / sampledLight.getPdf();

						assert(math::max_element(radianceAtCurrentPathVertex) < 20000.0f);
					}
				}
			}
		});

	return radianceAtCurrentPathVertex;
}


color_rgbx MonteCarloPathIntegrator::EvaluateRay(
	const ray3& ray,
	unsigned bounceLimit,
	space_real bias,
	math::Sampler<space_real>& sampler) const
{
	color_rgbx integral(color_rgbx::zero());
	color_rgbx throughput(color_rgbx::fill(1));

	ray3 currentRay = ray;
	bool accountForEmission = true;
	bool earlyExit = false;

	Hit hit = raytracer_->TraceRay(ray, bias);

	for (unsigned bounce = 0; bounce < bounceLimit - 1; ++bounce)
	{
		if (!hit.has_occurred())
		{
			const auto infinityPayload = infinityEvaluator_(currentRay) * throughput;
			assert(!math::anyNan(infinityPayload));
			integral += infinityPayload;
			stats_.registerPath(!math::isZero(infinityPayload), bounce);
			earlyExit = true;
			break;
		}
		
		hit.object()->material()->WithBsdfDistribution(
			*hit.object(),
			hit.point(),
			hit.normal(),
			hit.uvs(),
			currentRay.direction(),
			sampler,
			[&](const bsdf_distribution& bsdfDistribution)
			{
				const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());
				color_rgbx radianceAtCurrentPathVertex(color_rgbx::zero());

				if (bsdfDistribution.has_non_delta_component())
					radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(currentRay, hit, entering, bsdfDistribution, sampler);

				if (accountForEmission)
					radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(
						*hit.object(),
						hit.point(),
						hit.normal(),
						hit.uvs(),
						currentRay.direction(),
						sampler);

				const auto samplePayload = radianceAtCurrentPathVertex * throughput;
				assert(!math::anyNan(samplePayload));

				integral += samplePayload;
				stats_.registerPath(!math::isZero(samplePayload), bounce + 1);
				
				assert(math::max_element(integral) < 20000.0f);

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
				
				// a workaround since uniform_random_generator occasionally generates 1.0f when it should not.
				if (importance < color_real(1.0) && sampler.Get1D() >= importance || importance == color_real(0.0))
				{
					earlyExit = true;
					return;
				}
				else
				{
					throughput *= vertexThroughput / std::min(color_real(1.0), importance);
					assert(!math::anyNan(throughput));
					assert(math::max_element(throughput) < 100000.0f);
				}
				
#if false
				if (
					vertexThroughput[0] < color_real(0.0) || vertexThroughput[1] < color_real(0.0) || vertexThroughput[2] < color_real(0.0)
					|| std::isnan(vertexThroughput[0]) || std::isnan(vertexThroughput[1]) || std::isnan(vertexThroughput[2])
					)
				{
					std::wcout << typeid(*hit.object()->material()).name() << "\n";
					std::wcout << std::setprecision(std::numeric_limits<space_real>::max_digits10 + 1);
					std::wcout << vertexThroughput[0] << " " << vertexThroughput[1] << " " << vertexThroughput[2] << "\n";
					std::wcout << "hit point " << hit.point()[0] << " " << hit.point()[1] << " " << hit.point()[2] << "\n";
					std::wcout << "hit normal " << hit.normal()[0] << " " << hit.normal()[1] << " " << hit.normal()[2] << "\n";
					std::wcout << "ray direction " << currentRay.direction()[0] << " " << currentRay.direction()[1] << " " << currentRay.direction()[2] << "\n";
					std::wcout << "ray origin " << currentRay.origin()[0] << " " << currentRay.origin()[1] << " " << currentRay.origin()[2] << "\n";
					std::wcout << "bsdfPdf " << bsdfPdf << "\n";
					throw std::exception();
				}
#endif


				currentRay = ray3(hit.point(), bsdfDirection);
				hit = raytracer_->TraceRay(currentRay, BIAS, std::numeric_limits<space_real>::max(), hit.object(), hit.normal());
				accountForEmission = bsdfSample.getIsDelta();
			});
		if (earlyExit)
			break;
	}

	// last bounce is a special case
	if (bounceLimit > 0 && !earlyExit)
	{
		if (!hit.has_occurred())
		{
			const auto infinityPayload = infinityEvaluator_(currentRay) * throughput;
			assert(!math::anyNan(infinityPayload));
			integral += infinityPayload;
			stats_.registerPath(!math::isZero(infinityPayload), bounceLimit);
		}
		else
		{
			hit.object()->material()->WithBsdfDistribution(
				*hit.object(),
				hit.point(),
				hit.normal(),
				hit.uvs(),
				currentRay.direction(),
				sampler,
				[&](const bsdf_distribution& bsdfDistribution)
				{
					const bool entering = math::is_obtuse_angle(currentRay.direction(), hit.normal());
					color_rgbx radianceAtCurrentPathVertex(color_rgbx::zero());
					if (bsdfDistribution.has_non_delta_component())
						radianceAtCurrentPathVertex += EvaluateRadianceByLightsAtVertex(currentRay, hit, entering, bsdfDistribution, sampler);

					if (accountForEmission)
						radianceAtCurrentPathVertex += hit.object()->material()->EvaluateEmission(
							*hit.object(),
							hit.point(),
							hit.normal(),
							hit.uvs(),
							currentRay.direction(),
							sampler);

					const auto samplePayload = radianceAtCurrentPathVertex * throughput;
					assert(!math::anyNan(samplePayload));
					integral += samplePayload;
					stats_.registerPath(!math::isZero(samplePayload), bounceLimit + 1);
				});
		}
	}

	assert(math::max_element(integral) < 20000.0f);

	stats_.registerBundle(!math::isZero(integral));

	return integral;
}
