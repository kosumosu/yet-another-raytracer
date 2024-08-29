#pragma once

#include <cstdint>

namespace cloudscape
{
    struct planet
    {
        float turbidity;
        float samples;
        float groundcolor;
        float dispersion;
        float atmosradius;
        float airdensity;
        float aircolor;
        float planetradius;
    };

    struct noise
    {
        float multiplier;
        uint32_t detail;
        float size;
    };

    struct optimizations
    {
        float optimizations_cutoffthresshold;
        float optimizations_stepfalloff;
        float optimizations_detailfalloff;
        uint32_t optimizations_maxsubdivs;
        uint32_t optimizations_maxshadowsubdivs;
        float optimizations_subdivthresshold;
        float optimizations_shadowstep;
        uint32_t optimizations_shadowsamples;
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
    };
}