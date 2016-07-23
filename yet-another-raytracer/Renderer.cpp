#include "Renderer.h"

#include "Film.h"
#include "Scene.h"
#include "NullAccelerator.h"
#include "KDTreeAccelerator.h"
#include "Raytracer.h"
#include "LightingServer.h"
#include "RayEvaluator.h"
#include "StdHigheResolutionClockStopwatch.h"
#include "Types.h"


Renderer::Renderer(const initialization_finished_callback & initializationFinishedCallback, const rendering_finished_callback & renderingFinishedCallback, const progress_callback & progressCallback)
	: m_progressCallback(progressCallback)
	, m_initializationFinishedCallback(initializationFinishedCallback)
	, m_renderingFinishedCallback(renderingFinishedCallback)
{
}

void Renderer::ProcessPixel(Film& film, const Scene& scene, RayEvaluator & rayEvaluator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, unsigned int x, unsigned int y) const
{
	bool doJitter = scene.getSamplesPerPixel() > 1;
	color_real sampleWeight = color_real(1.0) / color_real(scene.getSamplesPerPixel());
	color_rgbx averageColor;
	vector2 pixelLeftBottomCoord(x, y);
	vector2 sizeNormalizationFactor(1.0 / film.width(), 1.0 / film.height());

	for (size_t i = 0; i < scene.getSamplesPerPixel(); i++)
	{
		auto shiftInsidePixel = doJitter ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0), randomEngine) : vector2(0.5, 0.5);
		auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

		auto ray = scene.camera()->GetViewRay(jitteredCoord * sizeNormalizationFactor, space_real(film.width()) / space_real(film.height()));
		averageColor += rayEvaluator.TraceRay(ray, scene.max_trace_depth(), space_real(0.0), true) * sampleWeight;
	}

	*film.pixel_at(x, y) = averageColor;
}

void Renderer::Render(Film & film, const Scene & scene) const
{
	math::StdUniformRandomBitGenerator<unsigned int, std::mt19937> randomEngine(std::move(std::mt19937()));

	float total_pixels = float(film.width()) * float(film.height()); // for timer

	PrepareObjects(scene.objects());

	//NullAccelerator accelerator(scene.objects());
	KDTreeAccelerator accelerator(scene.objects());
	Raytracer raytracer(std::unique_ptr<Marcher>(accelerator.CreateMarcher()));
	LightingServer lightingServer;
	lightingServer.shadows_enabled(true);
	RayEvaluator rayEvaluator(&raytracer, &lightingServer, &randomEngine);
	rayEvaluator.background_color(scene.getEnvironmentColor());

	lightingServer.lights(&scene.lights());

	m_initializationFinishedCallback();

	StdHigheResolutionClockStopwatch timer;


	timer.Restart();
	for (unsigned int y = 0; y < film.height(); y++)
	{
		for (unsigned int x = 0; x < film.width(); x++)
		{
			ProcessPixel(film, scene, rayEvaluator, randomEngine, x, y);

			if (timer.Sample() > 2.0f)
			{
				timer.Restart();
				m_progressCallback(x, y, (y * film.width() + x) / total_pixels);
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
