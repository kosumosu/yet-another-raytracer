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

    inline void LoadThinAntarctica(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 2.375f; // Sample had 1.375
        scene.planet.turbidity = 1.375f;
        scene.planet.groundcolor = 8421504;
        //scene.planet.groundcolor = 0x407F40;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.planetradius = 6371000.0f;

        //scene.rendering.samples = 4096*16*4;
        scene.rendering.samples = 1024;
        scene.rendering.hangle = 360.0f;
        scene.rendering.vangle = 45.0f;
        scene.rendering.width = 1280; //1920;
        scene.rendering.height = 720; //1080;

        scene.rendering.width = 640; //1920;
        scene.rendering.height = 360; //1080;
        //

        scene.rendering.bucketwidth = 24;
        scene.rendering.bucketheight = 24;
        scene.rendering.maxthreads = 30;

        scene.camera.elevation = 7.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = -10; //0;
        scene.camera.y = 0;
        //scene.camera.z = 1000.0; //30.0f;
        scene.camera.z = 70.0; //30.0f;

        scene.extensions.sphere_pos = {800.0, -50.0, 55.0};
        scene.extensions.sphere_radius = 50.0;

        scene.sun.azimuth = 10.0f; //45.0f;
        scene.sun.elevation = 30.0f; //-2.8; //30.0f;
        scene.sun.multiplier = 0.5; //0.25f;
        scene.sun.color = 16777215;
        scene.sun.size_mult = 1;

        scene.sun.multiplier = 4.0;
        scene.sun.elevation = 0.0f; //-2.8; //30.0f;
        scene.sun.azimuth = 10.0f; //45.0f;

        scene.sun.azimuth = 170.0f;
        scene.sun.elevation = 2.0f; //-2.8; //30.0f;

        scene.noise.multiplier = 0.75f;
        scene.noise.detail = 11;
        scene.noise.size = 1000;
        scene.noise.seed = 1234;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.8f;
        scene.clouds.fog = 0.4f; //0.01;
        scene.clouds.height = 300.0f;
        scene.clouds.thickness = 500.0f;
        scene.clouds.coverage = 0.5f; //0.2;


        // high
        scene.noise.multiplier = 0.8f;
        scene.noise.detail = 10;
        scene.noise.size = 15000;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.2f;
        scene.clouds.fog = 0.05f; //0.01;
        // scene.clouds.fog = 0.0f; //0.01;
        // scene.clouds.fog = 1.0f;
        scene.clouds.height = 1000.0f;
        scene.clouds.thickness = 2000.0f;
        scene.clouds.coverage = 0.48f; // 0.44f; //0.2;
        scene.clouds.coverage = 0.4f;
        scene.clouds.coverage = 0.35f;

        scene.extensions.haze_density = 0.00003f;
        //scene.extensions.haze_density = 0.0003f;
        scene.extensions.haze_scale_height = 300.0f;
        scene.extensions.haze_fwd_bck = 0.3f;
    }

    inline void LoadThickAntarctica(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 1.375f; // Sample had 1.375
        scene.planet.samples = 10;
        scene.planet.groundcolor = 8421504;
        //scene.planet.groundcolor = 0x407F40;
        scene.planet.dispersion = 1;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.aircolor = 16765620;
        scene.planet.planetradius = 6371000.0f;

        scene.optimizations.cutoffthresshold = 0.001f; //0.01f;
        scene.optimizations.stepfalloff = 0.0f;
        scene.optimizations.detailfalloff = 0.0f;
        scene.optimizations.maxsubdivs = 2;
        scene.optimizations.maxshadowsubdivs = 2;
        scene.optimizations.subdivthresshold = 0.01f;
        scene.optimizations.shadowstep = 25.0f;
        scene.optimizations.shadowsamples = 100;

        scene.rendering.samples = 2048;
        //scene.rendering.samples = 1024;
        scene.rendering.step = 2.0f; //25.0f;
        scene.rendering.hangle = 360.0f;
        scene.rendering.vangle = 90.0f;
        scene.rendering.width = 1280; //1920;
        scene.rendering.height = 720; //1080;

        scene.rendering.width = 297*20; //1920;
        scene.rendering.height = 210*20; //1080;

        scene.rendering.bucketwidth = 24;
        scene.rendering.bucketheight = 24;
        scene.rendering.maxthreads = 16;

        scene.camera.elevation = 0.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = -10; //0;
        scene.camera.y = 0;
        //scene.camera.z = 1000.0; //30.0f;
        scene.camera.z = 70.0; //30.0f;

        scene.vis.brightness = 1.0f;
        scene.vis.gamma = 1.0f;

        scene.extensions.sphere_pos = {200.0, -50.0, 55.0};
        scene.extensions.sphere_radius = 50.0;

        scene.sun.azimuth = 30.0f; //45.0f;
        scene.sun.elevation = 1.5f; //30.0f;
        scene.sun.ambient = 0x000000;
        scene.sun.multiplier = 0.1; //0.25f;
        scene.sun.color = 16777215;
        scene.sun.size_mult = 1;

        scene.noise.multiplier = 0.75f;
        scene.noise.detail = 11;
        scene.noise.size = 1000;
        scene.noise.seed = 1234;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.8f;
        scene.clouds.fog = 0.4f; //0.01;
        scene.clouds.height = 300.0f;
        scene.clouds.thickness = 500.0f;
        scene.clouds.coverage = 0.42f; //0.2;


        // high
        scene.noise.multiplier = 0.75f;
        scene.noise.detail = 12;
        scene.noise.size = 20000;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.8f;
        scene.clouds.fog = 0.002f; //0.01;
        scene.clouds.height = 7000.0f;
        scene.clouds.thickness = 500.0f;
        scene.clouds.coverage = 0.42f; //0.2;
    }

     inline void LoadPlanetFromSide(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 2.375f; // Sample had 1.375
        //scene.planet.turbidity = 1.0f; // Sample had 1.375
        scene.planet.groundcolor = 8421504;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.planetradius = 6371000.0f;

        scene.rendering.vangle = 45.0f;
        scene.rendering.vangle = 5.0f;
        scene.rendering.vangle = 0.5f;

        scene.camera.elevation = 0.0f;
        scene.camera.elevation = 0.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = -scene.planet.planetradius * 4.0; //0;
        scene.camera.x = -scene.planet.planetradius * 40.0; //0;
        scene.camera.y = 0;
        //scene.camera.z = 1000.0; //30.0f;
        scene.camera.z = -scene.planet.planetradius; //30.0f;
        scene.camera.z = 0; //30.0f;

        // scene.camera.elevation = 7.0f;
        // scene.camera.azimuth = 0.0f;
        // scene.camera.x = -10; //0;
        // scene.camera.y = 0;
        // //scene.camera.z = 1000.0; //30.0f;
        // scene.camera.z = 70.0; //30.0f;

        scene.rendering.bucketwidth = 24;
        scene.rendering.bucketheight = 24;
        scene.rendering.maxthreads = 16;
        // scene.rendering.maxthreads = 1;

        scene.rendering.samples = 32;
        scene.rendering.width = 640; //1920;
        scene.rendering.height = 640; //1080;
        scene.rendering.width = 2048;
        scene.rendering.height = 1024;

        scene.extensions.sphere_pos = {800.0, -50.0, 55.0};
        scene.extensions.sphere_radius = 50.0;

        scene.sun.ambient = 0x000000;
        scene.sun.multiplier = 1.0; //0.25f;
        //scene.sun.multiplier = 0.00004; //0.25f;
        scene.sun.color = 16777215;
        scene.sun.size_mult = 1;

        scene.sun.azimuth = 90.0f; //45.0f;
        // scene.sun.azimuth = 0.0f; //45.0f;
        scene.sun.elevation = 0.0f; //-2.8; //30.0f;

        scene.noise.multiplier = 0.75f;
        scene.noise.seed = 1234;
        scene.noise.multiplier = 0.8f;
        scene.noise.detail = 10;
        scene.noise.size = 8000;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.9f;
        scene.clouds.fog = 0.0f; //0.01;
        // scene.clouds.fog = 1.0f;
        scene.clouds.height = -10000.0f;
        scene.clouds.thickness = 1000.0f;
        scene.clouds.coverage = 0.48f; // 0.44f; //0.2;
        scene.clouds.coverage = -0.05f;

        scene.extensions.haze_density = 0.00000f;
        //scene.extensions.haze_density = 0.0003f;
        scene.extensions.haze_scale_height = 300.0f;
        scene.extensions.haze_fwd_bck = 0.3f;
    }

    inline void LoadPlanetFromSideWithThinAntarctica(cloudscape_scene& scene)
    {
        scene.planet.turbidity = 1.375f; // Sample had 1.375
        scene.planet.groundcolor = 8421504;
        scene.planet.groundcolor = 0x2e3217; // Real averageish west-russian plane color
        // scene.planet.groundcolor = 0;
        // scene.planet.groundcolor = 0x407F40;
        scene.planet.atmosradius = 6381000.0f;
        scene.planet.airdensity = 1.0f;
        scene.planet.aircolor = 16765620;
        scene.planet.planetradius = 6371000.0f;


        //scene.rendering.samples = 4096*16*4;
        scene.rendering.samples = 64;

        scene.rendering.width = 640; //1920;
        scene.rendering.height = 360; //1080;

        scene.rendering.width = 2560;
        scene.rendering.height = 1440;

        scene.rendering.bucketwidth = 24;
        scene.rendering.bucketheight = 24;
        scene.rendering.maxthreads = 30;

        scene.rendering.vangle = 45.0f;
        scene.rendering.vangle = 5.0f;
        scene.rendering.vangle = 0.5f;

        scene.camera.elevation = 0.0f;
        scene.camera.azimuth = 0.0f;
        scene.camera.x = -scene.planet.planetradius * 4.0; //0;
        scene.camera.x = -scene.planet.planetradius * 40.0; //0;
        scene.camera.y = 0;
        //scene.camera.z = 1000.0; //30.0f;
        scene.camera.z = -scene.planet.planetradius; //30.0f;
        scene.camera.z = 0; //30.0f;

        scene.vis.brightness = 1.0f;
        scene.vis.gamma = 1.0f;

        scene.extensions.sphere_pos = {800.0, -50.0, 55.0};
        scene.extensions.sphere_radius = 50.0;

        scene.sun.azimuth = 10.0f; //45.0f;
        scene.sun.elevation = 30.0f; //-2.8; //30.0f;
        scene.sun.ambient = 0x000000;
        scene.sun.multiplier = 1.0; //0.25f;
        scene.sun.color = 16777215;
        scene.sun.size_mult = 1;

        scene.sun.azimuth = 90.0f;
        scene.sun.elevation = 0.0f;

        scene.sun.multiplier = 10.0;

        scene.noise.multiplier = 0.75f;
        scene.noise.detail = 1;
        scene.noise.size = 1000;
        scene.noise.seed = 1234;


        // high
        scene.noise.multiplier = 0.8f;
        scene.noise.detail = 10;
        scene.noise.size = 800000;

        scene.clouds.color = 16777215;
        scene.clouds.trans_falloff = 1.0f;
        scene.clouds.fwd_bck = 0.8f;
        scene.clouds.fog = 0.05f; //0.01;
        scene.clouds.height = 5000.0f;
        scene.clouds.thickness = 500.0f;
        // scene.clouds.thickness = 200.0f;
        scene.clouds.coverage = 0.42f; //0.2;
        scene.clouds.coverage = 0.38f; //0.2;

        scene.extensions.haze_density = 0.00003f;
        //scene.extensions.haze_density = 0.0003f;
        scene.extensions.haze_scale_height = 300.0f;
        scene.extensions.haze_fwd_bck = 0.3f;
    }
}
