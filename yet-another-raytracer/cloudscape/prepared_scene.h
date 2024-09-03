#pragma once

#include "cloudscape_scene.h"

#include "objects/SphereObject.h"

#include <Types.h>

#include "materials/NullMaterial.h"
#include "math/angles.h"
#include "participating_media/HomogeneousMedium.h"
#include "participating_media/VoidMedium.h"

namespace cloudscape
{
    struct prepared_scene
    {
        cloudscape_scene scene;

        //vector3 planet_pos;
        vector3 sun_direction; // direction towards sun
        color_rgb sun_color;

        color_rgb ambient_light_color;
        color_rgb cloud_color;
        color_rgb ground_color;

        vector3 extinction_rayleigh;
        vector3 extinction_mie;

        std::shared_ptr<participating_media::ParticipatingMedium> atmospheric_medium;
        std::shared_ptr<participating_media::ParticipatingMedium> cloud_medium;

        std::shared_ptr<materials::Material> planet_material;
        std::shared_ptr<materials::Material> null_material;

        objects::SphereObject planet;
        objects::SphereObject lower_cloud_bound;
        objects::SphereObject upper_cloud_bound;

        DirectionalLightSource sun;

        Camera camera;
    };

    inline prepared_scene prepare_scene(const cloudscape_scene& scene)
    {
        const auto scaled_rayleigh_height = space_real(7993.0 * scene.planet.planetradius / 6371000.0);
        const auto scaled_mie_height = space_real(1200.0 * scene.planet.planetradius / 6371000.0);

        const auto planet_center = vector3{0.0, 0.0, -scene.planet.planetradius};
        const auto camera_pos = vector3{scene.camera.x, scene.camera.y, scene.camera.z};

        auto atmospheric_medium = std::make_shared<participating_media::VoidMedium>();
        auto cloud_medium = std::make_shared<participating_media::HomogeneousMedium>(
            participating_media::optical_thickness_t::zero(),
            participating_media::optical_thickness_t::fill(scene.clouds.fog),
            participating_media::spectral_coeffs::zero()
        );

        auto planet_material = std::make_shared<materials::BlinnMaterial>(
            color_rgb::zero(),
            nullptr,
            color::srgb_to_linear(color::from_bgr_int(scene.planet.groundcolor)),
            color_rgb::zero(),
            0.0,
            color_rgb::zero()
        );

        auto planet_object = objects::SphereObject{
            planet_center,
            scene.planet.planetradius
        };

        planet_object.material(planet_material.get());

        auto null_material = std::make_shared<materials::NullMaterial>();

        auto lower_clouds_bound = objects::SphereObject{
            planet_center, scene.planet.planetradius + scene.clouds.height, true
        };
        lower_clouds_bound.material(null_material.get());
        lower_clouds_bound.inner_medium(cloud_medium.get());
        auto upper_clouds_bound = objects::SphereObject{
            planet_center, scene.planet.planetradius + scene.clouds.height + scene.clouds.thickness
        };
        upper_clouds_bound.material(null_material.get());
        upper_clouds_bound.inner_medium(cloud_medium.get());

        DirectionalLightSource sun{
            math::from_angles(math::deg_to_rad(scene.sun.azimuth), math::deg_to_rad(scene.sun.elevation)),
            color::srgb_to_linear(color::from_bgr_int(scene.sun.color)) * scene.sun.multiplier
        };

        return {
            scene,
            math::from_angles(math::deg_to_rad(scene.sun.azimuth), math::deg_to_rad(scene.sun.elevation)),
            color::srgb_to_linear(color::from_bgr_int(scene.sun.color)) * scene.sun.multiplier,
            color::srgb_to_linear(color::from_bgr_int(scene.sun.ambient)),
            color::srgb_to_linear(color::from_bgr_int(scene.clouds.color)),
            color::srgb_to_linear(color::from_bgr_int(scene.planet.groundcolor)),
            {7.2865e-6, 1.2863e-5, 2.7408e-5}, // for wavelengths 615,535,445
            vector3::fill(turbidity_to_mie_extinction(scaled_rayleigh_height, scaled_mie_height,
                                                      scene.planet.turbidity)),
            std::move(atmospheric_medium),
            std::move(cloud_medium),
            std::move(planet_material),
            std::move(null_material),
            std::move(planet_object),
            std::move(lower_clouds_bound),
            std::move(upper_clouds_bound),
            std::move(sun),
            Camera{
                camera_pos,
                math::normalize(camera_pos - planet_center),
                math::from_angles(math::deg_to_rad(scene.camera.azimuth), math::deg_to_rad(scene.camera.elevation)),
                std::min(140.0f, scene.rendering.vangle)
            }
        };
    }
}
