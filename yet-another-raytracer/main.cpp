// 184-1x-hw3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Film.h"
#include "Scene.h"
#include "SphereObject.h"
#include "FlatTriangleObject.h"
#include "BlinnMaterial.h"
#include "Renderer.h"
#include "HRTimer.h"
#include "DirectionalLightSource.h"
#include "PointLightSource.h"
#include "SkyLightSource.h"

#include "SceneLoader.h"

#include "Types.h"
#include <iostream>
#include <memory>



void callback(unsigned int x, unsigned int y, float progress)
{
	char buf[10];
	sprintf_s(buf, "%.2f", progress * 100.0f);
	std::cout << "Done " << buf << "%" << std::endl;
}


void InsertRandomSpheres(Scene & scene, unsigned int count)
{
	color_rgbx zero4;
	color_rgbx one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material(new BlinnMaterial());
		material->diffuse(math::linearRand(zero4, one4));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0f, 300.0f));
		//material->emission(math::linearRand(zero4, one4 * 0.2f));

		std::shared_ptr<SphereObject> object(new SphereObject());
		object->radius(math::linearRand(space_real(0.1), space_real(1.0)));
		object->center(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->material(material);

		scene.objects()->push_back(object);
	}
}

void InsertSkyLights(Scene & scene, unsigned int count)
{
	color_rgbx zero4;
	color_rgbx one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	color_real intensity_per_light = color_real(5.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<DirectionalLightSource> light_source(new DirectionalLightSource());
		light_source->direction(math::sphericalRand<space_real>());
		light_source->color(color_rgbx(intensity_per_light));

		scene.lights()->push_back(light_source);
	}
}

void InsertRandomPointLights(Scene & scene, unsigned int count)
{
	color_rgbx zero4;
	color_rgbx one4(1.0f);
	vector3 zero3;
	vector3 one3(1.0f);
	vector3 minus_one3(-1.0f);

	color_real intensity_per_light = color_real(60.0) / count;

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<PointLightSource> light_source(new PointLightSource());
		light_source->position(math::linearRand(minus_one3 * space_real(3.0), one3 * space_real(3.0)));
		light_source->color(math::linearRand(zero4, color_rgbx(intensity_per_light)));

		scene.lights()->push_back(light_source);
	}
}

void InsertTwoSpheres(Scene & scene)
{
	std::shared_ptr<BlinnMaterial> material1(new BlinnMaterial());
	material1->diffuse(color_rgbx(0.75f, 0.3f, 0.0f, 1.0f));
	material1->specular(color_rgbx(0.6f, 0.6f, 0.6f, 1.0f));
	material1->shininess(200.0f);
	material1->emission(color_rgbx(0.1f, 0.05f, 0.15f, 1.0f));

	std::shared_ptr<SphereObject> object1(new SphereObject());
	object1->radius(1.0f);
	object1->center(vector3(0.0f, -0.9f, 0.0f));
	object1->material(material1);

	std::shared_ptr<BlinnMaterial> material2(new BlinnMaterial());
	material2->diffuse(color_rgbx(0.3f, 0.75f, 0.0f, 1.0f));
	material2->specular(color_rgbx(0.6f, 0.6f, 0.6f, 1.0f));
	material2->shininess(200.0f);
	material2->emission(color_rgbx(0.1f, 0.05f, 0.15f, 1.0f));

	std::shared_ptr<SphereObject> object2(new SphereObject());
	object2->radius(1.0f);
	object2->center(vector3(0.0f, 0.9f, 0.0f));
	object2->material(material2);

	scene.objects()->push_back(object1);
	scene.objects()->push_back(object2);
}

void InsertTriangle(Scene & scene)
{
	std::shared_ptr<BlinnMaterial> material1(new BlinnMaterial());
	material1->diffuse(color_rgbx(0.1f, 0.3f, 0.75f, 1.0f));
	material1->specular(color_rgbx(0.5f, 0.5f, 0.5f, 1.0f));
	material1->shininess(100.0f);
	material1->emission(color_rgbx(0.01f, 0.03f, 0.075f, 1.0f));

	std::shared_ptr<FlatTriangleObject> object1(new FlatTriangleObject());
	object1->material(material1);
	object1->vertex0(vector3(0.0f, 0.0f, 0.0f));
	object1->vertex1(vector3(0.0f, 2.0f, 0.0f));
	object1->vertex2(vector3(0.0f, 0.0f, 1.0f));

	scene.objects()->push_back(object1);
}

void InsertRandomTriangles(Scene & scene, unsigned int count)
{
	color_rgbx zero4;
	color_rgbx one4(1.0);
	vector3 zero3;
	vector3 one3(1.0);
	vector3 minus_one3(-1.0);

	for (unsigned int i = 0; i < count; i++)
	{
		std::shared_ptr<BlinnMaterial> material(new BlinnMaterial());
		material->diffuse(math::linearRand(zero4, one4));
		//material->specular(math::linearRand(zero4, one4));
		//material->shininess(math::linearRand(10.0, 300.0));
		//material->emission(math::linearRand(zero4, one4 * 0.2));

		std::shared_ptr<FlatTriangleObject> object(new FlatTriangleObject());
		object->material(material);
		object->vertex0(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->vertex1(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));
		object->vertex2(math::linearRand(minus_one3 * space_real(2.0), one3 * space_real(2.0)));

		scene.objects()->push_back(object);
	}
}

void InsertPointLight(Scene & scene)
{
	std::shared_ptr<PointLightSource> light_source(new PointLightSource());
	light_source->position(vector3(2.0, 2.0, 2.0));
	light_source->color(color_rgbx(20.0, 20.0, 30.0, 1.0));
	light_source->attenuation(Attenuation(1.0, 0.0, 0.0));

	scene.lights()->push_back(light_source);
}

void InsertDirectionalLight(Scene & scene)
{
	std::shared_ptr<DirectionalLightSource> light_source(new DirectionalLightSource());
	light_source->direction(math::normalize(vector3(1.0, 1.0, 1.0)));
	light_source->color(color_rgbx(0.6f, 0.6f, 0.9f, 1.0f));

	scene.lights()->push_back(light_source);
}

void InsertSkyLight(Scene & scene, unsigned int samples)
{
	std::shared_ptr<SkyLightSource> light_source(new SkyLightSource());
	light_source->color(color_rgbx(0.5f, 0.64f, 0.82f, 0.0f));
	light_source->samples(samples);

	scene.lights()->push_back(light_source);
}


void InitCamera(Scene & scene, unsigned int width, unsigned int height)
{
	scene.camera()->fovy(90.0);
	scene.camera()->position(vector3(4.0, 0.0, 0.0));
	scene.camera()->up(vector3(0.0, 0.0, 1.0));

	scene.viewport_width(width);
	scene.viewport_height(height);
}



//////////////////////////////////////////////////////////////////////////

void LoadFromFile(Scene & scene, const std::string & filename)
{
	std::unique_ptr<SceneLoader> loader(SceneLoader::CreateDefault());

	loader->Load(scene, filename);
}

//////////////////////////////////////////////////////////////////////////

std::string GetFileName(const std::string & input)
{
	auto index = input.rfind('\\');

	if (index == std::string::npos)
	{
		return input;
	}
	else
	{
		return input.substr(index + 1);
	}
}

std::string GetFileNameWithoutExtension(const std::string & input)
{
	auto filename = GetFileName(input);

	auto index = filename.rfind('.');

	if (index == std::string::npos)
	{
		return filename;
	}
	else
	{
		auto tmp = filename.substr(0, index);
		return tmp;
	}
}

std::string GetPathWithoutExtension(const std::string & input)
{
	auto index = input.rfind('.');

	if (index == std::string::npos)
	{
		return input;
	}
	else
	{
		auto tmp = input.substr(0, index);
		return tmp;
	}
}

void Render(const std::string & scene_file, const std::string & output_image_file)
{
	Scene scene;

#if false
	LoadFromFile(scene, scene_file);

#else

	InitCamera(scene, 1280, 960);

	InsertDirectionalLight(scene);
	//InsertSkyLights(scene, 200);
	//InsertRandomPointLights(scene, 7);

	//InsertTwoSpheres(scene);
	InsertRandomSpheres(scene, 20);
	//InsertTriangle(scene);
	InsertRandomTriangles(scene, 200);
	InsertSkyLight(scene, 64);
#endif

	Film film(scene.viewport_width(), scene.viewport_height());
	Renderer renderer;
	renderer.callback(callback);

	HRTimer timer;
	timer.Restart();

	renderer.Render(film, scene);

	auto elapsed = timer.Sample();

	std::cout << "Elapsed : " << elapsed << "sec" << std::endl;

	film.SaveAsPng(output_image_file.c_str());
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return 0;

	std::string image_file = GetPathWithoutExtension(argv[1]) + ".png";

	Render(std::string(argv[1]), image_file);

	ShellExecuteA(NULL, "Open", image_file.c_str(), NULL, NULL, SW_SHOWNORMAL);

	//std::cin.clear();
	//std::cin.get();

	return 0;
}

