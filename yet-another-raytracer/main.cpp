// 184-1x-hw3.cpp : Defines the entry point for the console application.
//

#include "Film.h"
#include "Scene.h"
#include "SphereObject.h"
#include "FlatTriangleObject.h"
#include "BlinnMaterial.h"
#include "IRenderer.h"
#include "StdHigheResolutionClockStopwatch.h"
#include "ProcessTimeStopwatch.h"
#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "SkyLightSource.h"

#include "SceneLoader.h"

#include "Types.h"

#include "OSAbstraction.h"
#include "SingleThreadedScanlineRenderer.h"
#include "BucketRenderer.h"
#include "TopDownSequencer.h"

#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>


template <typename TRandomEngine>
void InsertRandomSpheres(Scene& scene, unsigned int count, TRandomEngine& engine)
{
	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material(new BlinnMaterial());
		material->diffuse(math::linearRand(color_rgbx::zero(), color_rgbx::fill(1.0), engine));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0f, 300.0f));
		//material->emission(math::linearRand(zero4, one4 * 0.2f));

		std::shared_ptr<SphereObject> object(new SphereObject());
		object->radius(math::linearRand(space_real(0.1), space_real(1.0), engine));
		object->center(math::linearRand(vector3::fill(-1.0) * space_real(2.0), vector3::fill(1.0) * space_real(2.0), engine));
		object->material(material.get());

		scene.objects().push_back(object);
	}
}

template <typename TRandomEngine>
void InsertSkyLights(Scene& scene, unsigned int count, TRandomEngine& engine)
{
	const color_real intensity_per_light = color_real(5.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		scene.lights().push_back(
			std::make_shared<DirectionalLightSource>(math::sphericalRand<space_real>(engine), color_rgbx::fill(intensity_per_light), scene));
	}
}

template <typename TRandomEngine>
void InsertRandomPointLights(Scene& scene, unsigned int count, TRandomEngine& engine)
{
	color_real intensity_per_light = color_real(60.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<PointLightSource> light_source(new PointLightSource());
		light_source->position(math::linearRand(vector3::fill(-1.0) * space_real(3.0), vector3::fill(1.0) * space_real(3.0), engine));
		light_source->color(math::linearRand(color_rgbx::zero(), color_rgbx::fill(intensity_per_light), engine));

		scene.lights().push_back(light_source);
	}
}

void InsertTwoSpheres(Scene& scene)
{
	std::shared_ptr<BlinnMaterial> material1(
		new BlinnMaterial{
			color_rgbx(0.1f, 0.05f, 0.15f, 1.0f),
			nullptr,
			color_rgbx(0.75f, 0.3f, 0.0f, 1.0f),
			color_rgbx(0.6f, 0.6f, 0.6f, 1.0f),
			200.0f,
			color_rgbx::zero()
		});

	scene.getMaterials().insert(std::make_pair("InsertTwoSpheres()::material1", material1));

	std::shared_ptr<SphereObject> object1(new SphereObject());
	object1->radius(1.0f);
	object1->center(vector3(0.0f, -0.9f, 0.0f));
	object1->material(material1.get());

	std::shared_ptr<BlinnMaterial> material2(new BlinnMaterial());

	scene.getMaterials().insert(std::make_pair("InsertTwoSpheres()::material2", material2));

	material2->diffuse(color_rgbx(0.3f, 0.75f, 0.0f, 1.0f));
	material2->specular(color_rgbx(0.6f, 0.6f, 0.6f, 1.0f));
	material2->shininess(200.0f);
	material2->emission(color_rgbx(0.1f, 0.05f, 0.15f, 1.0f));

	std::shared_ptr<SphereObject> object2(new SphereObject());
	object2->radius(1.0f);
	object2->center(vector3(0.0f, 0.9f, 0.0f));
	object2->material(material2.get());

	scene.objects().push_back(object1);
	scene.objects().push_back(object2);
}

void InsertCalibrationSpheres(Scene& scene)
{
	std::shared_ptr<BlinnMaterial> material1(new BlinnMaterial());
	material1->diffuse(color_rgbx(1.0f, 1.0f, 1.0f, 0.0f));
	scene.getMaterials().insert(std::make_pair("InsertCalibrationSpheres()::material1", material1));

	std::shared_ptr<SphereObject> object1(new SphereObject());
	object1->radius(1.0f);
	object1->center(vector3(0.0f, -0.9f, 0.0f));
	object1->material(material1.get());

	std::shared_ptr<BlinnMaterial> material2{new BlinnMaterial{}};
	material2->diffuse(color_rgbx(0.5f, 0.5f, 0.5f, 0.0f));
	scene.getMaterials().insert(std::make_pair("InsertCalibrationSpheres()::material2", material2));

	std::shared_ptr<SphereObject> object2(new SphereObject());
	object2->radius(1.0f);
	object2->center(vector3(0.0f, 0.9f, 0.0f));
	object2->material(material2.get());

	scene.objects().push_back(std::move(object1));
	scene.objects().push_back(std::move(object2));
}

void InsertTriangle(Scene& scene)
{
	std::shared_ptr<BlinnMaterial> material1{new BlinnMaterial{}};
	material1->diffuse(color_rgbx(0.1f, 0.3f, 0.75f, 1.0f));
	material1->specular(color_rgbx(0.5f, 0.5f, 0.5f, 1.0f));
	material1->shininess(100.0f);
	material1->emission(color_rgbx(0.01f, 0.03f, 0.075f, 1.0f));
	scene.getMaterials().insert(std::make_pair("InsertTriangle()::material1", material1));

	std::shared_ptr<FlatTriangleObject> object1(new FlatTriangleObject());
	object1->material(material1.get());
	object1->vertex0(vector3(0.0f, 0.0f, 0.0f));
	object1->vertex1(vector3(0.0f, 2.0f, 0.0f));
	object1->vertex2(vector3(0.0f, 0.0f, 1.0f));

	scene.objects().push_back(std::move(object1));
}

template <typename TRandomEngine>
void InsertRandomTriangles(Scene& scene, unsigned int count, const space_real& size, TRandomEngine& engine)
{
	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material{new BlinnMaterial{}};
		scene.getMaterials().insert(std::make_pair("InsertTriangle()::material #" + std::to_string(i), material));
		material->diffuse(math::linearRand(color_rgbx::zero(), color_rgbx::fill(1.0), engine));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0, 300.0));
		//material->emission(math::linearRand(zero4, one4 * 0.2));

		const vector3 min_bound = vector3::fill(-size);
		const vector3 max_bound = vector3::fill(size);

		auto pivot = math::linearRand(vector3::fill(-2.0), vector3::fill(2.0), engine);

		std::shared_ptr<FlatTriangleObject> object{new FlatTriangleObject{}};
		object->material(material.get());
		object->vertex0(pivot + math::linearRand(min_bound, max_bound, engine));
		object->vertex1(pivot + math::linearRand(min_bound, max_bound, engine));
		object->vertex2(pivot + math::linearRand(min_bound, max_bound, engine));

		scene.objects().push_back(object);
	}
}

void InsertPointLight(Scene& scene)
{
	std::shared_ptr<PointLightSource> lightSource{new PointLightSource{}};
	lightSource->position(vector3(2.0, 2.0, 2.0));
	lightSource->color(color_rgbx(20.0, 20.0, 30.0, 1.0));
	lightSource->attenuation(Attenuation(1.0, 0.0, 0.0));

	scene.lights().push_back(std::move(lightSource));
}

void InsertDirectionalLight(Scene& scene)
{
	scene.lights().push_back(std::make_shared<DirectionalLightSource>(math::normalize(vector3(1.0, 1.0, 1.0)), color_rgbx(0.6f, 0.6f, 0.6f, 1.0f), scene));
}

void InsertSkyLight(Scene& scene, unsigned int samples)
{
	std::shared_ptr<SkyLightSource> lightSource{new SkyLightSource{}};
	lightSource->color(color_rgbx(0.5f, 0.64f, 0.82f, 0.0f));
	lightSource->samples(samples);

	scene.lights().push_back(lightSource);
}

void InitCamera(Scene& scene, unsigned int width, unsigned int height)
{
	scene.camera()->fovy(90.0);
	scene.camera()->position(vector3(4.0, 0.0, 0.0));
	scene.camera()->up(vector3(0.0, 0.0, 1.0));

	scene.viewport_width(width);
	scene.viewport_height(height);
}


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

#if true
	const BucketRenderer renderer(
		{16, 16},
		std::make_unique<TopDownSequencer>(),
		[&]()
		{
			processInitTime = processTimeStopwatch.Sample();
			realInitTime = realTimeStopwatch.Sample();
			std::wcout << "Initialization finished. Real time=" << processInitTime << "sec. Process time=" << processInitTime << "sec" << std::endl;
		},
		[&]()
		{
			const auto processTotalElapsed = processTimeStopwatch.Sample();
			const auto realTotalElapsed = realTimeStopwatch.Sample();
			std::wcout << "Rendering finished." << std::endl;
			std::wcout << "Real time : " << realTotalElapsed - realInitTime << "sec" << std::endl;
			std::wcout << "Total real time : " << realTotalElapsed << "sec" << std::endl;
			std::wcout << "Process time : " << processTotalElapsed - processInitTime << "sec" << std::endl;
			std::wcout << "Total process time : " << processTotalElapsed << "sec" << std::endl;
		},
		[&coutMutex] (float progress)
		{
			std::lock_guard guard{ coutMutex };
			std::cout << "Done " << std::setprecision(2) << std::fixed << progress * 100.0f << "%" << std::endl;
		});
#else
	const SingleThreadedScanlineRenderer renderer(
		[&]()
		{
			processInitTime = processTimeStopwatch.Sample();
			realInitTime = realTimeStopwatch.Sample();
			std::wcout << "Initialization finished. Real time=" << processInitTime << "sec. Process time=" << processInitTime << "sec" << std::endl;
		},
		[&]()
		{
			const auto processTotalElapsed = processTimeStopwatch.Sample();
			const auto realTotalElapsed = realTimeStopwatch.Sample();
			std::wcout << "Rendering finished." << std::endl;
			std::wcout << "Real time : " << realTotalElapsed - realInitTime << "sec" << std::endl;
			std::wcout << "Total real time : " << realTotalElapsed << "sec" << std::endl;
			std::wcout << "Process time : " << processTotalElapsed - processInitTime << "sec" << std::endl;
			std::wcout << "Total process time : " << processTotalElapsed << "sec" << std::endl;
		},
			[&coutMutex](float progress)
		{
			std::lock_guard guard{ coutMutex };
			std::cout << "Done " << std::setprecision(2) << std::fixed << progress * 100.0f << "%" << std::endl;
		});
#endif

	scene.PrepareForRendering();

	renderer.Render(film, scene);

	film.SaveAsPng(output_image_file);
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
