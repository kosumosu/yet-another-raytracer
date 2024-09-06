#pragma once

#include "Scene.h"

#include "objects/SphereObject.h"
#include "objects/FlatTriangleObject.h"
#include "materials/BlinnMaterial.h"
#include "StdHighResolutionClockStopwatch.h"
#include "lights/DirectionalLightSource.h"
#include "lights/PointLightSource.h"
#include "lights/SkyLightSource.h"

#include "SceneLoader.h"

#include "Types.h"

template <typename TRandomEngine>
void InsertRandomSpheres(Scene& scene, unsigned int count, TRandomEngine& engine)
{
    for (unsigned int i = 0; i < count; i++)
    {
        assert(false); // material only lives inside this scope along with shared_ptr
        auto material = std::make_shared<materials::BlinnMaterial>();
        material->diffuse(math::linearRand(color_rgbx::zero(), color_rgbx::fill(1.0), engine));
        //material->specular(math::linearRand(zero4, one4));
        //material->shininess(math::linearRand(10.0f, 300.0f));
        //material->emission(math::linearRand(zero4, one4 * 0.2f));

        auto object = std::make_shared<objects::SphereObject>();
        object->radius(math::linearRand(space_real(0.1), space_real(1.0), engine));
        object->center(math::linearRand(vector3::fill(-1.0) * space_real(2.0), vector3::fill(1.0) * space_real(2.0),
                                        engine));
        object->material(material.get());

        scene.objects().push_back(std::move(object));
    }
}

template <typename TRandomEngine>
void InsertSkyLights(Scene& scene, unsigned int count, TRandomEngine& engine)
{
    const color_real intensity_per_light = color_real(5.0) / color_real(count);

    for (unsigned int i = 0; i < count; i++)
    {
        scene.lights().push_back(
            std::make_shared<lights::DirectionalLightSource>(math::sphericalRand<space_real>(engine),
                                                     color_rgbx::fill(intensity_per_light), scene));
    }
}

template <typename TRandomEngine>
void InsertRandomPointLights(Scene& scene, unsigned int count, TRandomEngine& engine)
{
    color_real intensity_per_light = color_real(60.0) / color_real(count);

    for (unsigned int i = 0; i < count; i++)
    {
        std::shared_ptr<lights::PointLightSource> light_source(new lights::PointLightSource());
        light_source->position(math::linearRand(vector3::fill(-1.0) * space_real(3.0),
                                                vector3::fill(1.0) * space_real(3.0), engine));
        light_source->color(math::linearRand(color_rgbx::zero(), color_rgbx::fill(intensity_per_light), engine));

        scene.lights().push_back(light_source);
    }
}

inline void InsertTwoSpheres(Scene& scene)
{
    std::shared_ptr<materials::BlinnMaterial> material1(
        new materials::BlinnMaterial{
            color_rgb(0.1f, 0.05f, 0.15f),
            nullptr,
            color_rgb(0.75f, 0.3f, 0.0f),
            color_rgb(0.6f, 0.6f, 0.6f),
            200.0f,
            color_rgb::zero()
        });

    scene.getMaterials().insert(std::make_pair("InsertTwoSpheres()::material1", material1));

    auto object1 = std::make_shared<objects::SphereObject>();
    object1->radius(1.0f);
    object1->center(vector3(0.0f, -0.9f, 0.0f));
    object1->material(material1.get());

    std::shared_ptr<materials::BlinnMaterial> material2(new materials::BlinnMaterial());

    scene.getMaterials().insert(std::make_pair("InsertTwoSpheres()::material2", material2));

    material2->diffuse(color_rgb(0.3f, 0.75f, 0.0f));
    material2->specular(color_rgb(0.6f, 0.6f, 0.6f));
    material2->shininess(200.0f);
    material2->emission(color_rgb(0.1f, 0.05f, 0.15f));

    auto object2 = std::make_shared<objects::SphereObject>();
    object2->radius(1.0f);
    object2->center(vector3(0.0f, 0.9f, 0.0f));
    object2->material(material2.get());

    scene.objects().push_back(object1);
    scene.objects().push_back(object2);
}

inline void InsertCalibrationSpheres(Scene& scene)
{
    std::shared_ptr<materials::BlinnMaterial> material1(new materials::BlinnMaterial());
    material1->diffuse(color_rgb(1.0f, 1.0f, 1.0f));
    scene.getMaterials().insert(std::make_pair("InsertCalibrationSpheres()::material1", material1));

    auto object1 = std::make_shared<objects::SphereObject>();
    object1->radius(1.0f);
    object1->center(vector3(0.0f, -0.9f, 0.0f));
    object1->material(material1.get());

    std::shared_ptr<materials::BlinnMaterial> material2{new materials::BlinnMaterial{}};
    material2->diffuse(color_rgb(0.5f, 0.5f, 0.5f));
    scene.getMaterials().insert(std::make_pair("InsertCalibrationSpheres()::material2", material2));

    auto object2 = std::make_shared<objects::SphereObject>();
    object2->radius(1.0f);
    object2->center(vector3(0.0f, 0.9f, 0.0f));
    object2->material(material2.get());

    scene.objects().push_back(std::move(object1));
    scene.objects().push_back(std::move(object2));
}

inline void InsertTriangle(Scene& scene)
{
    std::shared_ptr<materials::BlinnMaterial> material1{new materials::BlinnMaterial{}};
    material1->diffuse(color_rgb(0.1f, 0.3f, 0.75f));
    material1->specular(color_rgb(0.5f, 0.5f, 0.5f));
    material1->shininess(100.0f);
    material1->emission(color_rgb(0.01f, 0.03f, 0.075f));
    scene.getMaterials().insert(std::make_pair("InsertTriangle()::material1", material1));

    std::shared_ptr<objects::FlatTriangleObject> object1(new objects::FlatTriangleObject(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    ));
    object1->material(material1.get());

    scene.objects().push_back(std::move(object1));
}

template <typename TRandomEngine>
void InsertRandomTriangles(Scene& scene, unsigned int count, const space_real& size, TRandomEngine& sampler)
{
    for (unsigned int i = 0; i < count; i++)
    {
        std::shared_ptr<materials::BlinnMaterial> material{new materials::BlinnMaterial{}};
        scene.getMaterials().insert(std::make_pair("InsertTriangle()::material #" + std::to_string(i), material));
        material->diffuse(math::linearRand(color_rgbx::zero(), color_rgbx::fill(1.0), sampler));
        //material->specular(math::linearRand(zero4, one4));
        //material->shininess(math::linearRand(10.0, 300.0));
        //material->emission(math::linearRand(zero4, one4 * 0.2));

        const vector3 min_bound = vector3::fill(-size);
        const vector3 max_bound = vector3::fill(size);

        const auto pivot = math::linearRand(vector3::fill(-2.0), vector3::fill(2.0), sampler);

        std::shared_ptr<objects::FlatTriangleObject> object{
            new objects::FlatTriangleObject{
                pivot + math::linearRand(min_bound, max_bound, sampler),
                pivot + math::linearRand(min_bound, max_bound, sampler),
                pivot + math::linearRand(min_bound, max_bound, sampler)
            }
        };
        object->material(material.get());

        scene.objects().push_back(object);
    }
}

inline void InsertPointLight(Scene& scene)
{
    std::shared_ptr<lights::PointLightSource> lightSource{new lights::PointLightSource{}};
    lightSource->position(vector3(2.0, 2.0, 2.0));
    lightSource->color(color_rgb(20.0, 20.0, 30.0));
    lightSource->attenuation(Attenuation(1.0, 0.0, 0.0));

    scene.lights().push_back(std::move(lightSource));
}

inline void InsertDirectionalLight(Scene& scene)
{
    scene.lights().push_back(
        std::make_shared<lights::DirectionalLightSource>(math::normalize(vector3(1.0, 1.0, 1.0)),
                                                 color_rgb(0.6f, 0.6f, 0.6f), scene));
}

inline void InsertSkyLight(Scene& scene, unsigned int samples)
{
    std::shared_ptr<lights::SkyLightSource> lightSource{new lights::SkyLightSource{}};
    lightSource->color(color_rgb(0.5f, 0.64f, 0.82f));
    lightSource->samples(samples);

    scene.lights().push_back(lightSource);
}

inline void InitCamera(Scene& scene, unsigned int width, unsigned int height)
{
    scene.camera()->fovy_degrees(90.0);
    scene.camera()->position(vector3(4.0, 0.0, 0.0));
    scene.camera()->up(vector3(0.0, 0.0, 1.0));

    scene.viewport_width(width);
    scene.viewport_height(height);
}
