#pragma once

#include "IBucketSequencer.h"
#include "IRenderer.h"
#include "hashing/hashing.h"
#include "lights/LightSource.h"
#include "Scene.h"
#include "Statistics.h"
#include "Types.h"
#include "ThreadBarrier.hpp"
#include "Raytracer.h"
#include "integrators/MonteCarloPathIntegrator.h"
#include "Film.h"
#include "accelerators/kd_tree/KDTreeAccelerator.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

namespace renderers
{
    struct Bucket
    {
        const uint_vector2 start;
        const uint_vector2 size;
    };

    template <class TRayIntegrator>
    class BucketRenderer final : public IRenderer<TRayIntegrator>
    {
    public:
        using progress_callback = std::function<void(const size_t nom, const size_t denom)>;
        using initialization_finished_callback = std::function<void()>;
        using rendering_finished_callback = std::function<void()>;
        using area_finished_callback = std::function<void(const Film& result)>;
        using area_started_callback = std::function<area_finished_callback(
            const uint_vector2& top_left, const uint_vector2& bottom_right)>;

    private:
        const std::size_t thread_count_;
        const uint_vector2 bucketSize_;
        const std::unique_ptr<IBucketSequencer> bucketSequencer_;

        initialization_finished_callback initializationFinishedCallback_;
        area_started_callback areaStartedCallback_;
        rendering_finished_callback renderingFinishedCallback_;
        progress_callback progressCallback_;

        mutable std::mutex thisMutex_;
        mutable statistics::Stats stats_;

    public:
        // progressCallback must support concurrent multithreaded calls
        BucketRenderer(
            std::size_t thread_count,
            uint_vector2 bucketSize,
            std::unique_ptr<IBucketSequencer> bucketSequencer,
            initialization_finished_callback initializationFinishedCallback,
            rendering_finished_callback renderingFinishedCallback,
            area_started_callback areaStartedCallback,
            progress_callback progressCallback)
            : thread_count_(thread_count)
              ,bucketSize_(std::move(bucketSize))
              , bucketSequencer_(std::move(bucketSequencer))
              , initializationFinishedCallback_(std::move(initializationFinishedCallback))
              , areaStartedCallback_(std::move(areaStartedCallback))
              , renderingFinishedCallback_(std::move(renderingFinishedCallback))
              , progressCallback_(std::move(progressCallback))
        {
        }


        void Render(
            Film& film,
            const Rect& rectToRender,
            const Camera& camera,
            const std::size_t samplesPerPixel,
            typename IRenderer<TRayIntegrator>::ray_integrator_factory_t rayIntegratorFactory,
            const uint32_t seed,
            const std::stop_token& stopToken) const override
        {
            const auto cropStart = rectToRender.top_left;
            const auto cropEnd = rectToRender.top_left + rectToRender.size;

            const uint_vector2 gridSize = math::intDivideRoundUp(rectToRender.size, bucketSize_);

            const auto totalBuckets = gridSize[0] * gridSize[1];

            auto sequence = bucketSequencer_->CreateSequence(gridSize);

            std::atomic_size_t completedCount = 0;

            std::vector<std::thread> threads;
            threads.reserve(thread_count_);

            ThreadBarrier barrier{thread_count_};

            for (unsigned int i = 0; i < thread_count_; ++i)
            {
                std::thread thread{
                    [&, sequence = sequence.get(), subFilm = film.CreateSubFilm(bucketSize_)] () mutable
                    {
                        auto integrator = rayIntegratorFactory();

                        barrier.arrive_and_wait();

                        for (
                            auto bucketCoord = sequence->getNext();
                            bucketCoord.has_value() && !stopToken.stop_requested();
                            bucketCoord = sequence->getNext())
                        {
                            subFilm.clear();

                            const uint_vector2 bucketMinCorner = cropStart + bucketSize_ * *bucketCoord;
                            const uint_vector2 bucketMaxCorner = math::min(bucketMinCorner + bucketSize_, cropEnd);
                            const uint_vector2 bucketSize = bucketMaxCorner - bucketMinCorner;

                            auto reportAreaFinished = areaStartedCallback_(bucketMinCorner, bucketMaxCorner);

                            ProcessBucket(
                                subFilm,
                                film.size(),
                                camera,
                                samplesPerPixel,
                                integrator,
                                Bucket{bucketMinCorner, bucketSize},
                                seed,
                                stopToken);

                            film.transferFilm(subFilm, bucketMinCorner, bucketSize);

                            reportAreaFinished(subFilm);

                            {
                                std::unique_lock lock{thisMutex_};
                                const auto localCompletedCount = ++completedCount;
                                progressCallback_(localCompletedCount, totalBuckets);
                            }
                        }

                        {
                            std::unique_lock lock{thisMutex_};
                            stats_.mergeIn(integrator.getStats());
                        }
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

        void PrintStats(std::wostream& stream) const override
        {
            stats_.printResult(stream);
        }

    private:
        void
        ProcessBucket(
            Film& film,
            const uint_vector2& wholeFilmSize,
            const Camera& camera,
            const std::size_t samplesPerPixel,
            RayIntegrator& rayIntegrator,
            const Bucket& bucket,
            const uint32_t seed,
            const std::stop_token& stopToken) const
        {
            for (size_t y = 0; y < bucket.size[1]; ++y)
            {
                for (size_t x = 0; x < bucket.size[0]; ++x)
                {
                    if (stopToken.stop_requested())
                        return;
                    const uint_vector2 localCoord{x, y};
                    ProcessPixel(film, wholeFilmSize, camera, samplesPerPixel, rayIntegrator, localCoord,
                                 bucket.start + localCoord, seed);
                }
            }
        }

        void ProcessPixel(
            Film& subFilm,
            const uint_vector2& wholeFilmSize,
            const Camera& camera,
            const std::size_t samplesPerPixel,
            RayIntegrator& rayIntegrator,
            const uint_vector2& subFilmCoord,
            const uint_vector2& wholeFilmCoord,
            const uint32_t seed) const
        {
            const unsigned finalSeed = hashing::default_1d_hash(uint_vector3(wholeFilmCoord, seed));
            math::SimpleSampler<space_real, std::mt19937> pixelPersonalSampler(std::mt19937{finalSeed});

            const bool doJitter = samplesPerPixel > 1U;
            color_rgb accumulatedColor = color_rgb::zero();
            const vector2 pixelLeftBottomCoord = wholeFilmCoord;
            const vector2 sizeNormalizationFactor =
                vector2(1.0, 1.0) / wholeFilmSize; //(1.0 / subFilm.width(), 1.0 / subFilm.height());
            const auto aspectRatio = space_real(wholeFilmSize[0]) / space_real(wholeFilmSize[1]);

            for (std::size_t i = 0; i < samplesPerPixel; i++)
            {
                const auto shiftInsidePixel = doJitter
                                                  ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0),
                                                                     pixelPersonalSampler)
                                                  : vector2(0.5, 0.5);
                const auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

                const auto ray = camera.GetViewRay(jitteredCoord * sizeNormalizationFactor, aspectRatio);

                const auto rayPayload =
                    rayIntegrator.EvaluateRay(ray, space_real(0.0), pixelPersonalSampler);

                assert(!math::anyNan(rayPayload));

                accumulatedColor += rayPayload;
            }

            subFilm.registerAtPixel(subFilmCoord, std::make_pair(accumulatedColor, samplesPerPixel));
        }
    };
}
