#pragma once

#include "Types.h"
#include <cstdint>

namespace cloudscape
{
    struct planet
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

    struct noise
    {
        float multiplier;
        uint32_t detail;
        float size;

        uint32_t seed;
    };

    struct optimizations
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

    struct rendering
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

    struct camera
    {
        float elevation;
        float azimuth;
        float z;
        float x;
        float y;
    };

    struct vis
    {
        float brightness;
        float gamma;
    };

    struct sun
    {
        float azimuth;
        float elevation;
        uint32_t ambient;
        float multiplier;
        uint32_t color;
        float size_mult;
    };

    struct clouds
    {
        uint32_t color;
        float trans_falloff;
        float fwd_bck;
        float height;
        float fog;
        float thickness;
        float coverage;
    };

    struct extensions
    {
        vector3 sphere_pos = vector3::zero();
        space_real sphere_radius;
    };

    struct cloudscape_scene
    {
        planet planet;
        noise noise;
        optimizations optimizations;
        rendering rendering;
        camera camera;
        vis vis;
        sun sun;
        clouds clouds;
        extensions extensions;
    };
}
