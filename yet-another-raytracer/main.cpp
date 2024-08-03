// 184-1x-hw3.cpp : Defines the entry point for the console application.
//

#include "SceneGeneration.h"

#include "OSAbstraction.h"
#include "SingleThreadedScanlineRenderer.h"
#include "BucketRenderer.h"
#include "TopDownSequencer.h"
#include "NullAccelerator.h"
#include "KDTreeAccelerator.h"

#include "Scene.h"
#include "SceneLoader.h"
#include "ProcessTimeStopwatch.h"

#include "applications/IApplication.h"
#include "applications/ConsoleApplication.h"

#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#if defined(_WIN32)
#include <tchar.h>
#endif


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

void Render(const std::filesystem::path& scene_file, const std::filesystem::path& output_image_file)
{
    applications::ConsoleApplication application;

    application.run(
        [&scene_file, &output_image_file](auto progressReporter, auto print_info)
        {
            Scene scene;

#if true
            LoadFromFile(scene, scene_file);
#else

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
#endif


            std::mutex coutMutex;
            ProcessTimeStopwatch processTimeStopwatch;
            StdHigheResolutionClockStopwatch realTimeStopwatch;
            processTimeStopwatch.Restart();
            realTimeStopwatch.Restart();

            Film film({scene.viewport_width(), scene.viewport_height()});
            float processInitTime;
            float realInitTime;

            scene.PrepareForRendering();

            //    NullAccelerator accelerator{ scene.objects() };
            KDTreeAccelerator accelerator{scene.objects()};

#if true
            const BucketRenderer<typeof(accelerator)> renderer(
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
                progressReporter);
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

            renderer.Render(film, scene, accelerator);

            renderer.PrintStats(std::wcout);

            film.SaveAsPng(output_image_file);
        }
    );
}

#if defined(_WIN32)
int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
        return 0;

    const auto image_file = std::filesystem::path(argv[1]).replace_extension(".png");

    Render(std::filesystem::path(argv[1]), image_file);

    openImageFileForDisplay(image_file.c_str());

    return 0;
}
#elif defined (__linux__)
int main(int argc, const char* argv[])
{
    if (argc < 2)
        return 0;

    const auto image_file = std::filesystem::path(argv[1]).replace_extension(".png");

    Render(std::filesystem::path(argv[1]), image_file);

    openImageFileForDisplay(image_file.c_str());

    return 0;
}

#endif
