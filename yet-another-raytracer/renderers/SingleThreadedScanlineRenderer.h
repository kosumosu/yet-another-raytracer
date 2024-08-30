#pragma once

#include "IRenderer.h"
#include "Film.h"
#include "hashing/hashing.h"
#include "Scene.h"
#include "Statistics.h"
#include "integrators/RayIntegrator.h"
#include "Raytracer.h"
#include "LightSource.h"
#include "integrators/MonteCarloPathIntegrator.h"
#include "StdHighResolutionClockStopwatch.h"

#include <functional>

namespace renderers
{
    template <class TRayIntegrator>
    class SingleThreadedScanlineRenderer final : public IRenderer<TRayIntegrator>
    {
        mutable statistics::Stats stats_;

    public:
        using progress_callback = std::function<void (float progress)>;
        using initialization_finished_callback = std::function<void()>;
        using rendering_finished_callback = std::function<void()>;

        SingleThreadedScanlineRenderer(
            initialization_finished_callback initializationFinishedCallback,
            rendering_finished_callback renderingFinishedCallback,
            progress_callback progressCallback)
            : progressCallback_(std::move(progressCallback))
              , initializationFinishedCallback_(std::move(initializationFinishedCallback))
              , renderingFinishedCallback_(std::move(renderingFinishedCallback))
        {
        }

        void Render(
            Film& film,
            const Rect& rectToRender,
            const Camera& camera,
            const std::size_t samplesPerPixel,
            typename IRenderer<TRayIntegrator>::ray_integrator_factory_t rayIntegratorFactory,
            const std::stop_token& stopToken
        ) const override
        {
            auto integrator = rayIntegratorFactory();

            const unsigned int startX = rectToRender.top_left[0];
            const unsigned int startY = rectToRender.top_left[1];
            const unsigned int endX = startX + rectToRender.size[0];
            const unsigned int endY = startY + rectToRender.size[1];
            const float oneOverTotalPixels = 1.0f / (float(endX - startX) * float(endY - startY)); // for timer

            initializationFinishedCallback_();

            StdHigheResolutionClockStopwatch timer;


            timer.Restart();
            for (unsigned int y = startY; y < endY && !stopToken.stop_requested(); ++y)
            {
                for (unsigned int x = startX; x < endX && !stopToken.stop_requested(); ++x)
                {
                    ProcessPixel(film, camera, samplesPerPixel, integrator, {x, y});

                    if (timer.Sample() > 2.0f)
                    {
                        timer.Restart();
                        progressCallback_(float((y - startY) * (endX - startX) + x - startX) * oneOverTotalPixels);
                    }
                }
            }
            stats_ = integrator.getStats();
            renderingFinishedCallback_();
        }

        void PrintStats(std::wostream& stream) const override
        {
            stats_.printResult(stream);
        }

    private:
        progress_callback progressCallback_;
        initialization_finished_callback initializationFinishedCallback_;
        rendering_finished_callback renderingFinishedCallback_;

        void ProcessPixel(
            Film& film,
            const Camera& camera,
            const std::size_t samplesPerPixel,
            RayIntegrator& rayIntegrator,
            const uint_vector2& wholeFilmCoord) const
        {
            const unsigned seed = hashing::default_1d_hash(wholeFilmCoord);
            math::SimpleSampler<space_real, std::mt19937> pixelPersonalSampler(std::mt19937{seed});

            const bool doJitter = samplesPerPixel > 1;
            const color_real sampleWeight = color_real(1.0) / color_real(samplesPerPixel);
            color_rgb averageColor = color_rgb::zero();
            const vector2 pixelLeftBottomCoord = math::cast<space_real>(wholeFilmCoord);
            const vector2 sizeNormalizationFactor(1.0 / film.width(), 1.0 / film.height());

            for (std::size_t i = 0; i < samplesPerPixel; i++)
            {
                const auto shiftInsidePixel = doJitter
                                                  ? math::linearRand(vector2(0.0, 0.0), vector2(1.0, 1.0),
                                                                     pixelPersonalSampler)
                                                  : vector2(0.5, 0.5);
                const auto jitteredCoord = pixelLeftBottomCoord + shiftInsidePixel;

                const auto ray = camera.GetViewRay(jitteredCoord * sizeNormalizationFactor,
                                                   space_real(film.width()) / space_real(film.height()));
                averageColor += rayIntegrator.EvaluateRay(ray, space_real(0.0),
                                                          pixelPersonalSampler) * sampleWeight;
            }

            film.setPixel(wholeFilmCoord, averageColor);
        }
    };
}
