#include "Renderer.h"

#include "Film.h"
#include "Scene.h"
#include "NullAccelerator.h"
#include "KDTreeAccelerator.h"
#include "Raytracer.h"
#include "StdHigheResolutionClockStopwatch.h"
#include "Types.h"
#include "MonteCarloPathIntegrator.h"


Renderer::Renderer(const initialization_finished_callback & initializationFinishedCallback, const rendering_finished_callback & renderingFinishedCallback, const progress_callback & progressCallback)
	: m_progressCallback(progressCallback)
	, m_initializationFinishedCallback(initializationFinishedCallback)
	, m_renderingFinishedCallback(renderingFinishedCallback)
{
}

void Renderer::ProcessPixel(Film & film, const Scene & scene, const RayIntegrator & rayIntegrator, math::UniformRandomBitGenerator<unsigned> & randomEngine, unsigned x, unsigned y) const
{
	const unsigned seed = x | (y << 16);
	math::StdUniformRandomBitGenerator<unsigned int, std::mt19937> pixelPersonalRandomEngine(std::mt19937{ seed });

	bool doJitter = scene.getSamplesPerPixel() > 1;
	color_real sampleWeight = color_real(1.0) / color_real(scene.getSamplesPerPixel());
	color_rgbx averageColor = color_rgbx::zero();
	vector2 pixelLeftBottomCoord(x, y);
	vector2 sizeNormalizationFactor(1.0 / film.width(), 1.0 / film.height());

	for (std::size_t i = 0; i < scene.getSamplesPerPixel(); i++)
	{
		auto shiftInsidePixel = doJitter ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0), pixelPersonalRandomEngine) : vector2(0.5, 0.5);
		auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

		auto ray = scene.camera()->GetViewRay(jitteredCoord * sizeNormalizationFactor, space_real(film.width()) / space_real(film.height()));
		averageColor += rayIntegrator.EvaluateRay(ray, scene.max_trace_depth(), space_real(0.0), pixelPersonalRandomEngine) * sampleWeight;
	}

	film.setPixel(x, y, averageColor);
}

void Renderer::Render(Film & film, const Scene & scene) const
{
	math::StdUniformRandomBitGenerator<unsigned int, std::mt19937> randomEngine{ std::mt19937{} };


	PrepareObjects(scene.objects());

	//NullAccelerator accelerator(scene.objects());
	KDTreeAccelerator accelerator(scene.objects());
	Raytracer raytracer(std::unique_ptr<Marcher>(accelerator.CreateMarcher()));

	std::vector<const LightSource*> lights(scene.lights().size());
	std::transform(std::begin(scene.lights()), std::end(scene.lights()), std::begin(lights), [](const auto & lightPtr) { return lightPtr.get(); });
	const MonteCarloPathIntegrator integrator{ &raytracer, lights, [&](const ray3 & ray) { return scene.getEnvironmentColor(); } };

	const bool isCropped = scene.getCropWidth() > 0 && scene.getCropHeight() > 0;

	const unsigned startX = isCropped ? scene.getCropX() : 0U;
	const unsigned startY = isCropped ? scene.getCropY() : 0U;
	const unsigned endX = isCropped ? std::min(film.width(), scene.getCropX() + scene.getCropWidth()) : film.width();
	const unsigned endY = isCropped ? std::min(film.height(), scene.getCropY() + scene.getCropHeight()) : film.height();
	float total_pixels = float(endX - startX) * float(endY - startY); // for timer

	m_initializationFinishedCallback();

	StdHigheResolutionClockStopwatch timer;


	timer.Restart();
	for (unsigned int y = startY; y < endY; ++y)
	{
		for (unsigned int x = startX; x < endX; ++x)
		{
			ProcessPixel(film, scene, integrator, randomEngine, x, y);

			if (timer.Sample() > 2.0f)
			{
				timer.Restart();
				m_progressCallback(((y - startY) * (endX - startX) + x - startX) / total_pixels);
			}
		}
	}
	m_renderingFinishedCallback();
}

void Renderer::PrepareObjects(const ObjectCollection & objects) const
{
	for (auto & object : objects)
	{
		object->PrepareForRendering();
	}
}
