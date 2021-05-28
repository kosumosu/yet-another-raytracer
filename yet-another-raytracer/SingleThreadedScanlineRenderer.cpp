#include "SingleThreadedScanlineRenderer.h"

#include "Film.h"
#include "KDTreeAccelerator.h"
#include "MonteCarloPathIntegrator.h"
#include "NullAccelerator.h"
#include "Raytracer.h"
#include "Scene.h"
#include "StdHigheResolutionClockStopwatch.h"
#include "Types.h"

#include <utility>

SingleThreadedScanlineRenderer::SingleThreadedScanlineRenderer(
	initialization_finished_callback initializationFinishedCallback,
	rendering_finished_callback renderingFinishedCallback,
	progress_callback progressCallback)
	: progressCallback_(std::move(progressCallback))
	, initializationFinishedCallback_(std::move(initializationFinishedCallback))
	, renderingFinishedCallback_(std::move(renderingFinishedCallback))
{
}

void SingleThreadedScanlineRenderer::ProcessPixel(Film& film, const Scene& scene, const RayIntegrator& rayIntegrator, unsigned int x, unsigned int y) const
{
	const unsigned seed = x | (y << 16);
	math::StdUniformRandomBitGenerator<unsigned int, std::mt19937> pixelPersonalRandomEngine(std::mt19937{seed});

	const bool doJitter = scene.getSamplesPerPixel() > 1;
	const color_real sampleWeight = color_real(1.0) / color_real(scene.getSamplesPerPixel());
	color_rgbx averageColor = color_rgbx::zero();
	const vector2 pixelLeftBottomCoord(x, y);
	const vector2 sizeNormalizationFactor(1.0 / film.width(), 1.0 / film.height());

	for (std::size_t i = 0; i < scene.getSamplesPerPixel(); i++)
	{
		const auto shiftInsidePixel = doJitter ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0), pixelPersonalRandomEngine) : vector2(0.5, 0.5);
		const auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

		const auto ray = scene.camera()->GetViewRay(jitteredCoord * sizeNormalizationFactor, space_real(film.width()) / space_real(film.height()));
		averageColor += rayIntegrator.EvaluateRay(ray, scene.max_trace_depth(), space_real(0.0), pixelPersonalRandomEngine) * sampleWeight;
	}

	film.setPixel({x, y}, averageColor);
}

void SingleThreadedScanlineRenderer::Render(Film& film, const Scene& scene) const
{
	//NullAccelerator accelerator(scene.objects());
	const KDTreeAccelerator accelerator(scene.objects());
	Raytracer raytracer(std::unique_ptr<Marcher>(accelerator.CreateMarcher()));

	std::vector<const LightSource*> lights(scene.lights().size());
	std::transform(std::begin(scene.lights()), std::end(scene.lights()), std::begin(lights), [](const auto& lightPtr) { return lightPtr.get(); });
	const MonteCarloPathIntegrator integrator{&raytracer, std::move(lights), [&](const ray3& ray) { return scene.getEnvironmentColor(); }};

	const bool isCropped = scene.getCropWidth() > 0 && scene.getCropHeight() > 0;

	const unsigned int startX = isCropped ? scene.getCropX() : 0U;
	const unsigned int startY = isCropped ? scene.getCropY() : 0U;
	const unsigned int endX = isCropped ? std::min(film.width(), scene.getCropX() + scene.getCropWidth()) : film.width();
	const unsigned int endY = isCropped ? std::min(film.height(), scene.getCropY() + scene.getCropHeight()) : film.height();
	const float oneOverTotalPixels = 1.0f / (float(endX - startX) * float(endY - startY)); // for timer

	initializationFinishedCallback_();

	StdHigheResolutionClockStopwatch timer;


	timer.Restart();
	for (unsigned int y = startY; y < endY; ++y)
	{
		for (unsigned int x = startX; x < endX; ++x)
		{
			ProcessPixel(film, scene, integrator, x, y);

			if (timer.Sample() > 2.0f)
			{
				timer.Restart();
				progressCallback_(float((y - startY) * (endX - startX) + x - startX) * oneOverTotalPixels);
			}
		}
	}
	renderingFinishedCallback_();
}
