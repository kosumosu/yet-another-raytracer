#pragma once

#include "Types.h"
#include <cstdint>

namespace cloudscape
{
    struct planet_t
    {
        float turbidity;
        float samples;
        uint32_t groundcolor;
        float dispersion;
        float atmosradius;
        float airdensity;
        uint32_t aircolor;
        float planetradius;
    };

    struct noise_t
    {
        float multiplier;
        uint32_t detail;
        float size;

        uint32_t seed;
    };

    struct optimizations_t
    {
        float cutoffthresshold;
        float stepfalloff;
        float detailfalloff;
        uint32_t maxsubdivs;
        uint32_t maxshadowsubdivs;
        float subdivthresshold;
        float shadowstep;
        uint32_t shadowsamples;
    };

    struct rendering_t
    {
        uint32_t samples;
        float step;
        float hangle;
        float vangle;
        uint32_t width;
        uint32_t height;
        uint32_t bucketwidth;
        uint32_t bucketheight;
        uint32_t maxthreads;
    };

    struct camera_t
    {
        float elevation;
        float azimuth;
        float z;
        float x;
        float y;
    };

    struct vis_t
    {
        float brightness;
        float gamma;
    };

    struct sun_t
    {
        float azimuth;
        float elevation;
        uint32_t ambient;
        float multiplier;
        uint32_t color;
        float size_mult;
    };

    struct clouds_t
    {
        uint32_t color;
        float trans_falloff;
        float fwd_bck;
        float height;
        float fog;
        float thickness;
        float coverage;
    };

    struct extensions_t
    {
        vector3 sphere_pos = vector3::zero();
        space_real sphere_radius;

        float haze_density = 0.0f;
        float haze_scale_height;
        float haze_fwd_bck;
    };

    struct cloudscape_scene
    {
        planet_t planet;
        noise_t noise;
        optimizations_t optimizations;
        rendering_t rendering;
        camera_t camera;
        vis_t vis;
        sun_t sun;
        clouds_t clouds;
        extensions_t extensions;
    };
}
