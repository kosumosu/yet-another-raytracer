#include "BucketRenderer.h"

#include "Film.h"
#include "Hashing.h"
#include "KDTreeAccelerator.h"
#include "Marcher.h"
#include "MonteCarloPathIntegrator.h"
#include "RayIntegrator.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Types.h"
#include "Sampler.h"

#include "ThreadBarrier.hpp"

#include <atomic>
#include <cassert>
#include <mutex>
#include <random>
#include <thread>

BucketRenderer::BucketRenderer(
	uint_vector2 bucketSize,
	std::unique_ptr<IBucketSequencer> bucketSequencer,
	initialization_finished_callback initializationFinishedCallback,
	rendering_finished_callback renderingFinishedCallback,
	progress_callback progressCallback)
	: bucketSize_(std::move(bucketSize))
	, bucketSequencer_(std::move(bucketSequencer))
	, initializationFinishedCallback_(std::move(initializationFinishedCallback))
	, progressCallback_(std::move(progressCallback))
	, renderingFinishedCallback_(std::move(renderingFinishedCallback))
{
}

void BucketRenderer::Render(Film& film, const Scene& scene) const
{
	const KDTreeAccelerator accelerator(scene.objects());

	std::vector<const LightSource*> lights(scene.lights().size());
	std::transform(std::begin(scene.lights()), std::end(scene.lights()), std::begin(lights), [](const auto& lightPtr) { return lightPtr.get(); });

	const bool isCropped = scene.getCropWidth() > 0 && scene.getCropHeight() > 0;

	const uint_vector2 viewportSize{scene.viewport_width(), scene.viewport_height()};
	const uint_vector2 cropStart = isCropped ? uint_vector2{scene.getCropX(), scene.getCropY()} : uint_vector2::zero();
	const uint_vector2 cropSize = isCropped ? uint_vector2{scene.getCropWidth(), scene.getCropHeight()} : viewportSize;
	const uint_vector2 cropEnd = cropStart + cropSize;
	const uint_vector2 gridSize = math::intDivideRoundUp(cropSize, bucketSize_);

	const float oneOverTotalBuckets = 1.0f / float(gridSize[0] * gridSize[1]); // for timer

	auto sequence = bucketSequencer_->CreateSequence(gridSize);

	std::atomic_size_t completedCount = 0;

	const auto threadCount = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.reserve(threadCount);

	
	std::mutex statsMutex;

	ThreadBarrier barrier{threadCount};

	for (unsigned int i = 0; i < threadCount; ++i)
	{
		std::thread thread{
			[&, sequence = sequence.get()]
			{
				Raytracer raytracer(std::unique_ptr<Marcher>(accelerator.CreateMarcher()));
				const MonteCarloPathIntegrator integrator{&raytracer, lights, [&](const ray3& ray) { return scene.getEnvironmentColor(); }};

				Film subFilm{bucketSize_};

				barrier.arrive_and_wait();

				for (auto bucketCoord = sequence->getNext(); bucketCoord; bucketCoord = sequence->getNext())
				{
					const uint_vector2 bucketMinCorner = cropStart + bucketSize_ * *bucketCoord;
					const uint_vector2 bucketMaxCorner = math::min(bucketMinCorner + bucketSize_, cropEnd);
					const uint_vector2 bucketSize = bucketMaxCorner - bucketMinCorner;

					ProcessBucket(subFilm, film.size(), scene, integrator, Bucket{bucketMinCorner, bucketSize});
					film.transferFilm(subFilm, bucketMinCorner, bucketSize);
					const auto localCompletedCount = ++completedCount;
					progressCallback_(float(localCompletedCount) * oneOverTotalBuckets);
				}

				stats_.mergeIn(integrator.getStats());
			}
		};
		threads.push_back(std::move(thread));
	}

	barrier.wait();

	initializationFinishedCallback_();

	for (auto& thread : threads)
	{
		thread.join();
	}

	renderingFinishedCallback_();
}

void BucketRenderer::PrintStats(std::ostream& stream) const
{
	stats_.printResult(stream);
}

void BucketRenderer::ProcessBucket(
	Film& film,
	const uint_vector2& wholeFilmSize,
	const Scene& scene,
	const RayIntegrator& rayIntegrator,
	const Bucket& bucket) const
{
	for (size_t y = 0; y < bucket.size[1]; ++y)
	{
		for (size_t x = 0; x < bucket.size[0]; ++x)
		{
			const uint_vector2 localCoord{x, y};
			ProcessPixel(film, wholeFilmSize, scene, rayIntegrator, localCoord, bucket.start + localCoord);
		}
	}
}

void BucketRenderer::ProcessPixel(
	Film& subFilm,
	const uint_vector2& wholeFilmSize,
	const Scene& scene,
	const RayIntegrator& rayIntegrator,
	const uint_vector2& subFilmCoord,
	const uint_vector2& wholeFilmCoord) const
{
	const unsigned seed = xxhash32(wholeFilmCoord);
	math::SimpleSampler<space_real, std::mt19937> pixelPersonalSampler(std::mt19937{seed});

	const bool doJitter = scene.getSamplesPerPixel() > 1;
	const color_real sampleWeight = color_real(1.0) / color_real(scene.getSamplesPerPixel());
	color_rgbx averageColor = color_rgbx::zero();
	const vector2 pixelLeftBottomCoord = wholeFilmCoord;
	const vector2 sizeNormalizationFactor = vector2(1.0, 1.0) / wholeFilmSize; //(1.0 / subFilm.width(), 1.0 / subFilm.height());
	const auto aspectRatio = space_real(wholeFilmSize[0]) / space_real(wholeFilmSize[1]);

	for (std::size_t i = 0; i < scene.getSamplesPerPixel(); i++)
	{
		const auto shiftInsidePixel = doJitter ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0), pixelPersonalSampler) : vector2(0.5, 0.5);
		const auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

		const auto ray = scene.camera()->GetViewRay(jitteredCoord * sizeNormalizationFactor, aspectRatio);

		const auto rayPayload = rayIntegrator.EvaluateRay(ray, scene.max_trace_depth(), space_real(0.0), pixelPersonalSampler) * sampleWeight;
        assert(!std::isnan(rayPayload[0]) && !std::isnan(rayPayload[1]) && !std::isnan(rayPayload[2]) && !std::isnan(rayPayload[3]));
		averageColor += rayPayload;
	}

	subFilm.setPixel(subFilmCoord, averageColor);
}
