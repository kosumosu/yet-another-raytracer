#pragma once

#include "cloudscape_scene.h"

#include "objects/SphereObject.h"

#include <Types.h>

#include "math/angles.h"

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

        objects::SphereObject planet;
        objects::SphereObject lower_cloud_bound;
        objects::SphereObject upper_cloud_bound;

        Camera camera;
    };

    inline prepared_scene prepare_scene(const cloudscape_scene& scene)
    {
        const auto scaled_rayleigh_height = space_real(7993.0 * scene.planet.planetradius / 6371000.0);
        const auto scaled_mie_height = space_real(1200.0 * scene.planet.planetradius / 6371000.0);

        const auto planet_center = vector3{0.0, 0.0, -scene.planet.planetradius};
        const auto camera_pos = vector3{scene.camera.x, scene.camera.y, scene.camera.z};

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
            objects::SphereObject{planet_center, scene.planet.planetradius},
            objects::SphereObject{planet_center, scene.planet.planetradius + scene.clouds.height},
            objects::SphereObject{
                planet_center,
                scene.planet.planetradius + scene.clouds.height + scene.clouds.thickness
            },
            Camera{
                camera_pos,
                math::normalize(camera_pos - planet_center),
                math::from_angles(math::deg_to_rad(scene.camera.azimuth), math::deg_to_rad(scene.camera.elevation)),
                std::min(140.0f, scene.rendering.vangle)
            }
        };
    }
}
