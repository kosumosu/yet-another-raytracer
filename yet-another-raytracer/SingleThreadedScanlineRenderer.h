#pragma once

#include "IRenderer.h"

#include <functional>

class RayIntegrator;
class Film;
class Scene;

class SingleThreadedScanlineRenderer final : public IRenderer
{
public:
	using progress_callback = std::function<void (float progress)>;
	using initialization_finished_callback = std::function<void()>;
	using rendering_finished_callback = std::function<void()>;

	SingleThreadedScanlineRenderer(
		initialization_finished_callback initializationFinishedCallback,
		rendering_finished_callback renderingFinishedCallback,
		progress_callback progressCallback);
	void Render(Film& film, const Scene& scene) const override;

private:
	progress_callback progressCallback_;
	initialization_finished_callback initializationFinishedCallback_;
	rendering_finished_callback renderingFinishedCallback_;

	void ProcessPixel(Film& film, const Scene& scene, const RayIntegrator& rayIntegrator, unsigned int x, unsigned int y) const;
};
