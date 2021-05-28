#pragma once

#include "Accelerator.h"
#include "Accelerator.h"
#include "Accelerator.h"
#include "Accelerator.h"
#include "Accelerator.h"
#include "Accelerator.h"
#include "IBucketSequencer.h"
#include "IRenderer.h"

#include <functional>

#include "Types.h"

struct Bucket
{
	const uint_vector2 start;
	const uint_vector2 size;
};

class BucketRenderer : public IRenderer
{
public:
	using progress_callback = std::function<void (float progress)>;
	using initialization_finished_callback = std::function<void()>;
	using rendering_finished_callback = std::function<void()>;

	// progressCallback must support concurrent multithreaded calls
	BucketRenderer(
		uint_vector2 bucketSize,
		std::unique_ptr<IBucketSequencer> bucketSequencer,
		initialization_finished_callback initializationFinishedCallback,
		rendering_finished_callback renderingFinishedCallback,
		progress_callback progressCallback);
	void Render(Film& film, const Scene& scene) const override;

private:
	uint_vector2 bucketSize_;
	std::unique_ptr<IBucketSequencer> bucketSequencer_;
	initialization_finished_callback initializationFinishedCallback_;
	progress_callback progressCallback_;
	rendering_finished_callback renderingFinishedCallback_;

	void ProcessBucket(Film& film, const uint_vector2& wholeFilmSize, const Scene& scene, const RayIntegrator& rayIntegrator, const Bucket& bucket) const;
	void ProcessPixel(
		Film& subFilm,
		const uint_vector2&,
		const Scene& scene,
		const RayIntegrator& rayIntegrator,
		const uint_vector2& subFilmCoord,
		const uint_vector2& wholeFilmCoord) const;
};
