#pragma once

#include <functional>

class RayIntegrator;
class Film;
class Scene;

class Renderer
{
public:
	using progress_callback = std::function<void (float progress)>;
	using initialization_finished_callback = std::function<void()>;
	using rendering_finished_callback = std::function<void()>;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer& other) = delete;

	Renderer(initialization_finished_callback initializationFinishedCallback, rendering_finished_callback renderingFinishedCallback,
		progress_callback progressCallback);
	void Render(Film& film, const Scene& scene) const;

private:
	progress_callback progressCallback_;
	initialization_finished_callback initializationFinishedCallback_;
	rendering_finished_callback renderingFinishedCallback_;

	void ProcessPixel(Film& film, const Scene& scene, const RayIntegrator& rayIntegrator, unsigned int x, unsigned int y) const;
};
