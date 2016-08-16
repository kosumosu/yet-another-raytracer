#pragma once

#include "GeometryObject.h"
#include "CameraCommandFactory.h"
#include "UniformRandomBitGenerator.h"
#include <functional>

class RayIntegrator;
class Film;
class Scene;

class Renderer
{
public:
	typedef std::function<void (float progress)> progress_callback;
	typedef std::function<void()> initialization_finished_callback;
	typedef std::function<void()> rendering_finished_callback;

	Renderer(const initialization_finished_callback & initializationFinishedCallback, const rendering_finished_callback & renderingFinishedCallback, const progress_callback & progressCallback);
	void Render(Film & film, const Scene & scene) const;

private:
	progress_callback m_progressCallback;
	initialization_finished_callback m_initializationFinishedCallback;
	rendering_finished_callback m_renderingFinishedCallback;

	void ProcessPixel(Film& film, const Scene& scene, const RayIntegrator & rayIntegrator, math::UniformRandomBitGenerator<unsigned int> & randomEngine, unsigned int x, unsigned int y) const;
	void PrepareObjects(const ObjectCollection & objects) const;
};

