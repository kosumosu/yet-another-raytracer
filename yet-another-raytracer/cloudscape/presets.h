#pragma once
#include "cloudscape_scene.h"

namespace cloudscape
{
    inline void LoadAntarctica(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 0.05f; // Sample had 1.375
        scene.planet.samples = 10;
        scene.planet.groundcolor = 8421504;
        scene.planet.dispersion = 1;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.aircolor = 16765620;
        scene.planet.planetradius = 6371000.0f;

        scene.noise.multiplier = 0.6f;
        scene.noise.detail = 8;
        scene.noise.size = 4000;

        scene.optimizations.cutoffthresshold = 0.001f; //0.01f;
        scene.optimizations.stepfalloff = 0.0f;
        scene.optimizations.detailfalloff = 0.0f;
        scene.optimizations.maxsubdivs = 2;
        scene.optimizations.maxshadowsubdivs = 2;
        scene.optimizations.subdivthresshold = 0.01f;
        scene.optimizations.shadowstep = 25.0f;
        scene.optimizations.shadowsamples = 100;

        scene.rendering.samples = 100;
        scene.rendering.step = 25.0f; //25.0f;
        scene.rendering.hangle = 360.0f;
        scene.rendering.vangle = 180.0f;
        scene.rendering.width = 640;
        scene.rendering.height = 320;
        scene.rendering.bucketwidth = 16;
        scene.rendering.bucketheight = 16;
        scene.rendering.maxthreads = 2;

        scene.camera.elevation = 0.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = 0;
        scene.camera.y = 0;
        scene.camera.z = 1.85f;

        scene.vis.brightness = 1.0f;
        scene.vis.gamma = 1.0f;

        scene.sun.azimuth = 45.0f; //45.0f;
        scene.sun.elevation = 30.0f; //30.0f;
        scene.sun.ambient = 8421504;
        scene.sun.multiplier = 1.0f;
        scene.sun.color = 16777215;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.6f;
        scene.clouds.fog = 0.1f; //0.01;
        scene.clouds.height = 1000.0f;
        scene.clouds.thickness = 100.0f;
        scene.clouds.coverage = 0.52f; //0.2;
    }

    inline void LoadThickAntarctica(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 0.05f; // Sample had 1.375
        scene.planet.samples = 10;
        scene.planet.groundcolor = 8421504;
        scene.planet.dispersion = 1;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.aircolor = 16765620;
        scene.planet.planetradius = 6371000.0f;

        scene.noise.multiplier = 0.6f;
        scene.noise.detail = 10;
        scene.noise.size = 4000;
        scene.noise.seed = 1234;

        scene.optimizations.cutoffthresshold = 0.001f; //0.01f;
        scene.optimizations.stepfalloff = 0.0f;
        scene.optimizations.detailfalloff = 0.0f;
        scene.optimizations.maxsubdivs = 2;
        scene.optimizations.maxshadowsubdivs = 2;
        scene.optimizations.subdivthresshold = 0.01f;
        scene.optimizations.shadowstep = 25.0f;
        scene.optimizations.shadowsamples = 100;

        scene.rendering.samples = 2000;
        scene.rendering.step = 2.0f; //25.0f;
        scene.rendering.hangle = 360.0f;
        scene.rendering.vangle = 90.0f;
        scene.rendering.width = 640; //1920;
        scene.rendering.height = 360; //1080;
        scene.rendering.bucketwidth = 16;
        scene.rendering.bucketheight = 16;
        scene.rendering.maxthreads = 2;

        scene.camera.elevation = 0.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = 0;
        scene.camera.y = 0;
        scene.camera.z = 30.0; //2000.0; //30.0f;

        scene.vis.brightness = 1.0f;
        scene.vis.gamma = 1.0f;

        scene.sun.azimuth = 45.0f; //45.0f;
        scene.sun.elevation = 30.0f; //30.0f;
        scene.sun.ambient = 0x000000;
        scene.sun.multiplier = 0.01f;
        scene.sun.color = 16777215;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.6f;
        scene.clouds.fog = 0.01f; //0.01;
        scene.clouds.height = 1000.0f;
        scene.clouds.thickness = 500.0f;
        scene.clouds.coverage = 0.5f; //0.2;
    }
}
