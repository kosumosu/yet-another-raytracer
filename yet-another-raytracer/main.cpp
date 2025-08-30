// 184-1x-hw3.cpp : Defines the entry point for the console application.
//

#include "SceneGeneration.h"

#include "OSAbstraction.h"

#include "integrators/MonteCarloPathIntegrator.h"
#include "cloudscape/cloudscape_integrator.h"

#include "renderers/SingleThreadedScanlineRenderer.h"
#include "renderers/BucketRenderer.h"
#include "TopDownSequencer.h"
#include "accelerators/null/NullAccelerator.h"
#include "accelerators/kd_tree/KDTreeAccelerator.h"

#include "Scene.h"
#include "SceneLoader.h"
#include "ProcessTimeStopwatch.h"

#include "applications/IApplication.h"
#include "applications/ConsoleApplication.h"
#include "applications/NanaApplication.h"

#include "cloudscape/presets.h"
#include "participating_media/VoidMedium.h"

#include <argparse/argparse.hpp>

#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>

#if defined(_WIN32)
#include <windows.h>
#endif


#if defined(ENABLE_UI_DEF)
constexpr bool ENABLE_UI = true;
#else
constexpr bool ENABLE_UI = false;
#endif


using namespace std::string_view_literals;

//////////////////////////////////////////////////////////////////////////

void LoadFromFile(Scene& scene, const std::filesystem::path& filename)
{
    const std::unique_ptr<SceneLoader> loader{SceneLoader::CreateDefault()};

    loader->Load(scene, filename);
}

//////////////////////////////////////////////////////////////////////////

std::wstring GetFileName(const std::wstring& input)
{
    const auto index = input.rfind('\\');

    return index == std::string::npos ? input : input.substr(index + 1);
}

std::wstring GetFileNameWithoutExtension(const std::wstring& input)
{
    auto filename = GetFileName(input);

    const auto index = filename.rfind('.');

    return index == std::string::npos ? filename : filename.substr(0, index);
}

std::filesystem::path GetPathWithoutExtension(const std::filesystem::path& input)
{
    return input.parent_path() / input.stem();
}

renderers::Rect getCroppedRectToRender(const Scene scene)
{
    const bool isCropped = scene.getCropWidth() > 0 && scene.getCropHeight() > 0;

    const uint_vector2 viewportSize{scene.viewport_width(), scene.viewport_height()};
    uint_vector2 cropStart = isCropped
                                 ? uint_vector2{scene.getCropX(), scene.getCropY()}
                                 : uint_vector2::zero();
    uint_vector2 cropSize = isCropped
                                ? uint_vector2{scene.getCropWidth(), scene.getCropHeight()}
                                : viewportSize;
    // const uint_vector2 cropEnd = cropStart + cropSize;
    return {std::move(cropStart), std::move(cropSize)};
}

template <CApplication TApplication>
void RenderRegularImpl(
    TApplication application,
    const std::filesystem::path& scene_file,
    const std::filesystem::path& output_image_file_without_extension
) {

    application.run(
        [&scene_file, &output_image_file_without_extension](
        const auto& stopToken,
        const auto& initialize,
        const auto& reportProgress,
        const auto& reportAreaStarted,
        const auto& reportRenderingFinihsed,
        const auto& print_info)
        {
            Scene scene;

            if constexpr (true)
            {
                LoadFromFile(scene, scene_file);
            }
            else
            {
                std::mt19937 engine;
                InitCamera(scene, 1280, 960);
                scene.max_trace_depth(8);
                //scene.setEnvironmentColor(color_rgbx(0.5f, 0.64f, 0.82f, 0.0f));
                //scene.setEnvironmentColor(color_rgbx(1.0));

                InsertDirectionalLight(scene);
                //InsertSkyLights(scene, 200, engine);
                //InsertRandomPointLights(scene, 7, engine);

                //InsertTwoSpheres(scene);
                //InsertCalibrationSpheres(scene);
                InsertRandomSpheres(scene, 20, engine);
                //InsertTriangle(scene);
                InsertRandomTriangles(scene, 20, 1.0, engine);
                //InsertSkyLight(scene, 128);
            }

            std::mutex coutMutex;
            ProcessTimeStopwatch processTimeStopwatch;
            StdHigheResolutionClockStopwatch realTimeStopwatch;
            processTimeStopwatch.Restart();
            realTimeStopwatch.Restart();

            Film film({scene.viewport_width(), scene.viewport_height()});
            float processInitTime;
            float realInitTime;

            initialize({scene.viewport_width(), scene.viewport_height()});

            scene.PrepareForRendering();

            //    NullAccelerator accelerator{ scene.objects() };
            accelerators::kd_tree::KDTreeAccelerator accelerator{scene.objects()};

            std::vector<const lights::LightSource*> lights(scene.lights().size());
            std::transform(std::begin(scene.lights()), std::end(scene.lights()), std::begin(lights),
                           [](const auto& lightPtr) { return lightPtr.get(); });

            using integrator_t = MonteCarloPathIntegrator<accelerators::kd_tree::KDTreeMarcher>;

#if true
            const renderers::BucketRenderer<integrator_t> renderer(
                std::thread::hardware_concurrency(),
                {32, 32},
                std::make_unique<TopDownSequencer>(),
                [&]()
                {
                    processInitTime = processTimeStopwatch.Sample();
                    realInitTime = realTimeStopwatch.Sample();

                    print_info(std::format(L"Initialization finished. Real time={:.3f}sec. Process time={:.3f}sec\n",
                                           realInitTime, processInitTime));
                },
                [&]()
                {
                    const auto processTotalElapsed = processTimeStopwatch.Sample();
                    const auto realTotalElapsed = realTimeStopwatch.Sample();

                    print_info(std::format(
                        L"Rendering finished.\n"
                        "Real time : {:.3f}sec\n"
                        "Total real time : {:.3f}sec\n"
                        "Process time : {:.3f}sec\n"
                        "Total process time : {:.3f}sec\n",
                        realTotalElapsed - realInitTime,
                        realTotalElapsed,
                        processTotalElapsed - processInitTime,
                        processTotalElapsed
                    ));
                },
                reportAreaStarted,
                reportProgress);
#else
	const SingleThreadedScanlineRenderer renderer(
		[&]()
		{
			processInitTime = processTimeStopwatch.Sample();
			realInitTime = realTimeStopwatch.Sample();
		    print_info(std::format(L"Initialization finished. Real time={.3f}sec. Process time={.3f}sec\n",
                       realInitTime, processInitTime));
		},
		[&]()
		{
			const auto processTotalElapsed = processTimeStopwatch.Sample();
			const auto realTotalElapsed = realTimeStopwatch.Sample();
		    print_info(std::format(
                "Rendering finished.\n"
                "Real time : {}sec\n"
                "Total real time : {}sec\n"
                "Process time : {}sec\n"
                "Total process time : {}sec\n",
                realTotalElapsed - realInitTime,
                realTotalElapsed,
                processTotalElapsed - processInitTime,
                processTotalElapsed
            ));
		},
			progressReporter);
#endif

            renderer.Render(
                film,
                getCroppedRectToRender(scene),
                *scene.camera(),
                scene.getSamplesPerPixel(),
                [&accelerator, &scene, &lights]
                {
                    auto raytracer = Raytracer{accelerator.CreateMarcher()};
                    MonteCarloPathIntegrator<accelerators::kd_tree::KDTreeMarcher> integrator(
                        std::move(raytracer),
                        lights,
                        scene.max_trace_depth(),
                        [&](const ray3& ray) { return scene.getEnvironmentColor(); }
                    );

                    return integrator;
                },
                0,
                stopToken);

            reportRenderingFinihsed(film, 1);

            renderer.PrintStats(std::wcout); {
                auto output_image_file = output_image_file_without_extension;

                output_image_file.replace_extension(".exr");
                film.SaveAsExr(output_image_file);
                output_image_file.replace_extension(".png");
                film.SaveAsPng(output_image_file);
            }

        }
    );
}

void RenderRegular(const std::filesystem::path& scene_file,
                   const std::filesystem::path& output_image_file_without_extension
)
{
    if constexpr (ENABLE_UI) {
        RenderRegularImpl(applications::NanaApplicaion(), scene_file, output_image_file_without_extension);
    } else {
        RenderRegularImpl(applications::ConsoleApplication(), scene_file, output_image_file_without_extension);
    }
}


template <CApplication TApplication>
void RenderCloudscapeImpl(
    TApplication application,
    const std::filesystem::path& scene_file,
    const std::filesystem::path& output_image_file_without_extension,
    bool persistent_mode
)
{
    application.run(
        [&scene_file, &output_image_file_without_extension, persistent_mode](
        const auto& stopToken,
        const auto& initialize,
        const auto& reportProgress,
        const auto& reportAreaStarted,
        const auto& reportRenderingFinished,
        const auto& print_info)
        {
            cloudscape::cloudscape_scene scene{};

            if constexpr (false)
            {
                //LoadFromFile(scene, scene_file);
            }
            else
            {
                // cloudscape::LoadThinAntarctica(scene);
                // cloudscape::LoadPlanetFromSide(scene);
                cloudscape::LoadPlanetFromSideWithThinAntarctica(scene);
            }

            auto prepared_scene = cloudscape::prepare_scene(scene);

            std::mutex coutMutex;
            ProcessTimeStopwatch processTimeStopwatch;
            StdHigheResolutionClockStopwatch realTimeStopwatch;
            processTimeStopwatch.Restart();
            realTimeStopwatch.Restart();

            Film film({scene.rendering.width, scene.rendering.height}, color::cas_to_rgb);

            auto persistent_file = [&] -> std::optional<std::filesystem::path> {
                if (persistent_mode) {
                    auto persistent_file = output_image_file_without_extension;
                    persistent_file.replace_extension(".framebuffer");

                    film.TryLoadFromFile(persistent_file);
                    return std::make_optional(persistent_file);
                } else {
                    return std::nullopt;
                }
            }();

            float processInitTime;
            float realInitTime;

            initialize({scene.rendering.width, scene.rendering.height});

            std::vector<objects::GeometryObject*> objects = {
                &prepared_scene.planet,
                &prepared_scene.lower_cloud_bound,
                &prepared_scene.upper_cloud_bound,
                &prepared_scene.extra_sphere,
                &prepared_scene.extra_triangle
            };

            accelerators::null::NullAccelerator accelerator{objects};

            using integrator_t = cloudscape::CloudscapeIntegrator<accelerators::null::NullMarcher>;

            const renderers::BucketRenderer<integrator_t> renderer(
                scene.rendering.maxthreads ? scene.rendering.maxthreads : std::thread::hardware_concurrency(),
                {scene.rendering.bucketwidth, scene.rendering.bucketheight},
                std::make_unique<TopDownSequencer>(),
                [&]()
                {
                    processInitTime = processTimeStopwatch.Sample();
                    realInitTime = realTimeStopwatch.Sample();

                    print_info(std::format(L"Initialization finished. Real time={:.3f}sec. Process time={:.3f}sec\n",
                                           realInitTime, processInitTime));
                },
                [&]()
                {
                    const auto processTotalElapsed = processTimeStopwatch.Sample();
                    const auto realTotalElapsed = realTimeStopwatch.Sample();

                    print_info(std::format(
                        L"Rendering finished.\n"
                        "Real time : {:.3f}sec\n"
                        "Total real time : {:.3f}sec\n"
                        "Process time : {:.3f}sec\n"
                        "Total process time : {:.3f}sec\n",
                        realTotalElapsed - realInitTime,
                        realTotalElapsed,
                        processTotalElapsed - processInitTime,
                        processTotalElapsed
                    ));
                },
                reportAreaStarted,
                reportProgress);

            participating_media::VoidMedium atmospheric_medium;
            participating_media::VoidMedium cloud_medium;

            for (int i = 0; !stopToken.stop_requested(); ++i) {
                renderer.Render(
                    film,
                    {uint_vector2::zero(), film.size()},
                     // { uint_vector2 { 200, 200 }, {1, 1}},
                    prepared_scene.camera,
                    scene.rendering.samples,
                    [&prepared_scene, &accelerator, &atmospheric_medium, &cloud_medium]
                    {
                        auto raytracer = Raytracer{accelerator.CreateMarcher()};
                        return cloudscape::CloudscapeIntegrator{
                            prepared_scene,
                            raytracer
                        };
                    },
                    i,
                    stopToken);

                reportRenderingFinished(film, i + 1);

                // renderer.PrintStats(std::wcout);


                if (!stopToken.stop_requested() && persistent_file) {
                    film.PersistToFile(*persistent_file);
                }

                if (i == 0 || !stopToken.stop_requested()) {
                    auto output_image_file = output_image_file_without_extension;

                    output_image_file.replace_extension(".exr");
                    film.SaveAsExr(output_image_file);
                    output_image_file.replace_extension(".png");
                    film.SaveAsPng(output_image_file);
                }
            }
        }
    );
}

void RenderCloudscape(
    const std::filesystem::path& scene_file,
    const std::filesystem::path& output_image_file_without_extension,
    bool persistent_mode
) {
    if constexpr (ENABLE_UI) {
        RenderCloudscapeImpl(applications::NanaApplicaion(), scene_file, output_image_file_without_extension, persistent_mode);
    } else {
        RenderCloudscapeImpl(applications::ConsoleApplication(), scene_file, output_image_file_without_extension, persistent_mode);
    }
}


int main_impl(int argc, const char* argv[])
{
    argparse::ArgumentParser arguments("yet-another-raytracer");

    arguments.add_argument("-c").flag().help("Switches to cloudscape mode instead of regular renderer.");
    arguments.add_argument("scene_file").help("The scene file path.");
    arguments.add_argument("-p").flag().help("Persist frame buffer between runs");

    try {
        arguments.parse_args(argc, argv); // Example: ./main --input_files config.yml System.xml
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arguments;
        return EXIT_FAILURE;
    }

    const bool persistent_mode = arguments.get<bool>("-p");

    const auto scene_path = std::filesystem::path(arguments.get<std::string>("scene_file"));
    auto image_path = std::filesystem::path(scene_path);
    image_path.replace_extension("");

    if (arguments["-c"] == true) {
        RenderCloudscape(scene_path, image_path, persistent_mode);
    } else {
        RenderRegular(scene_path, image_path);
    }

    image_path.replace_extension(".png");
    openImageFileForDisplay(image_path);

    return 0;
}

#if defined (__linux__)

int main(int argc, const char* argv[])
{
    return main_impl(argc, argv);
}

#elif defined(_WIN32) // && defined(__MINGW32__)

int main(int argc, const char* argv[])
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, ".UTF8");
    return main_impl(argc, argv);
}

#endif
