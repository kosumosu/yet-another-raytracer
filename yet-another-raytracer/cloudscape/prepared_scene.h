#pragma once

#include "cloudscape_scene.h"

#include "objects/SphereObject.h"

#include <Types.h>

namespace cloudscape
{
    struct prepared_scene
    {
        cloudscape_scene scene;

        vector3 planet_pos;
        vector3 sun_direction; // direction towards sun

        color_rgb sun_color;
        color_rgb ground_color;
        color_rgb ambient_light_color;

        float rayleigh_atmosphere_density;
        float mie_atmosphere_density;

        vector3 beta_total_rayleigh;
        vector3 beta_total_mie;

        objects::SphereObject planet;
        objects::SphereObject lower_cloud_bound;
        objects::SphereObject upper_cloud_bound;

    };
}