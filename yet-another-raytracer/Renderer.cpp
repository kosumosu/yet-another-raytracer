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

Renderer::Renderer(void)
	: m_callback(NULL)
{
}


Renderer::~Renderer(void)
{
}

void Renderer::Render( Film & film, const Scene & scene ) const
{
	float total_pixels = (float)film.width() * (float)film.height(); // for timer

	PrepareObjects(*scene.objects());

	//NullAccelerator accelerator(scene.objects().get());
	KDTreeAccelerator accelerator(scene.objects().get());
	Raytracer raytracer(&accelerator);
	LightingServer lighting_server(&raytracer);
	lighting_server.shadows_enabled(true);
	RayEvaluator ray_evaluator(&raytracer, &lighting_server);
	//ray_evaluator.background_color(color4(0.5f, 0.64f, 0.82f, 1.0f));

	lighting_server.lights(scene.lights().get());

	HRTimer timer;

	timer.Restart();
	for (unsigned int y = 0; y < film.height(); y++)
	{
		for (unsigned int x = 0; x < film.width(); x++)
		{
			auto ray = scene.camera()->GetViewRay((x + space_real(0.5)) / film.width(), (y + space_real(0.5)) / film.height(), (space_real)film.width() / (space_real)film.height());
			*film.pixel_at(x, y) = ray_evaluator.TraceRay(ray, scene.max_trace_depth(), space_real(0.0));

			if (m_callback != NULL && timer.Sample() > 2.0f)
			{
				timer.Restart();
				m_callback(x, y, (y * film.width() + x) / total_pixels );
			}
		}
	}
}

void Renderer::PrepareObjects( const ObjectCollection & objects ) const
{
	for (auto & object : objects)
	{
		object->PrepareForRendering();
	}
}
