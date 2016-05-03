#include "Renderer.h"

#include "Film.h"
#include "Scene.h"
#include "NullAccelerator.h"
#include "KDTreeAccelerator.h"
#include "Raytracer.h"
#include "LightingServer.h"
#include "RayEvaluator.h"
#include "HRTimer.h"
#include "Types.h"


Renderer::Renderer(const initialization_finished_callback & initializationFinishedCallback, const rendering_finished_callback & renderingFinishedCallback, const progress_callback & progressCallback)
	: m_progressCallback(progressCallback)
	, m_initializationFinishedCallback(initializationFinishedCallback)
	, m_renderingFinishedCallback(renderingFinishedCallback)
{
}

void Renderer::Render(Film & film, const Scene & scene) const
{
	float total_pixels = (float)film.width() * (float)film.height(); // for timer

	PrepareObjects(scene.objects());

	//NullAccelerator accelerator(scene.objects());
	KDTreeAccelerator accelerator(scene.objects());
	Raytracer raytracer(std::unique_ptr<Marcher>(accelerator.CreateMarcher()));
	LightingServer lightingServer;
	lightingServer.shadows_enabled(true);
	RayEvaluator rayEvaluator(&raytracer, &lightingServer);
	rayEvaluator.background_color(scene.environmentColor());

	lightingServer.lights(&scene.lights());

	m_initializationFinishedCallback();

	HRTimer timer;

	timer.Restart();
	for (unsigned int y = 0; y < film.height(); y++)
	{
		for (unsigned int x = 0; x < film.width(); x++)
		{
			auto ray = scene.camera()->GetViewRay((x + space_real(0.5)) / film.width(), (y + space_real(0.5)) / film.height(), (space_real)film.width() / (space_real)film.height());
			*film.pixel_at(x, y) = rayEvaluator.TraceRay(ray, scene.max_trace_depth(), space_real(0.0), true);

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
