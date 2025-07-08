#pragma once

#include "cloudscape_scene.h"

#include "objects/SphereObject.h"

#include <Types.h>
#include <materials/DielectricMaterial.h>

#include "color/cas_colorspace.h"
#include "lights/SunLightSource.h"
#include "materials/NullMaterial.h"
#include "math/angles.h"
#include "participating_media/AtmosphericMedium.h"
#include "participating_media/HomogeneousMedium.h"
#include "participating_media/PerlinCloudsMedium.h"
#include "participating_media/VoidMedium.h"


#include "participating_media/HenyeyGreensteinPhaseFunction.h"
#include "participating_media/RayleighPhaseFunction.h"
#include "participating_media/SphericalPhaseFunction.h"
#include "participating_media/SunAwareHenyeyGreensteinPhaseFunction.h"
#include "participating_media/SunAwareRayleighPhaseFunction.h"

namespace cloudscape
{
    struct prepared_scene
    {
        cloudscape_scene scene;

        std::shared_ptr<participating_media::PhaseFunction> spherical_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> rayleigh_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> sun_aware_rayleigh_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> aerosol_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> sun_aware_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> clouds_phase_function;
        std::shared_ptr<participating_media::PhaseFunction> haze_phase_function;

        std::shared_ptr<participating_media::ParticipatingMedium> atmospheric_molecular_medium;
        std::shared_ptr<participating_media::ParticipatingMedium> atmospheric_aerosol_medium;
        std::shared_ptr<participating_media::ParticipatingMedium> homogenous_medium;
        std::shared_ptr<participating_media::ParticipatingMedium> cloud_medium;
        std::shared_ptr<participating_media::ParticipatingMedium> haze_medium;

        std::shared_ptr<materials::Material> planet_material;
        std::shared_ptr<materials::Material> null_material;
        std::shared_ptr<materials::Material> extra_material;
        std::shared_ptr<materials::Material> glass_material;

        objects::SphereObject planet;
        objects::SphereObject lower_cloud_bound;
        objects::SphereObject upper_cloud_bound;

        objects::SphereObject extra_sphere;
        objects::FlatTriangleObject extra_triangle;

        lights::SunLightSource sun;
        lights::DirectionalLightSource directional_sun;
        lights::PointLightSource point_light;

        Camera camera;
    };

    constexpr color_rgb sun_cas(0.9420, 1.0269, 1.0241);

    constexpr color_rgb int_to_linear(uint32_t color)
    {
        return color::srgb_to_linear(color::from_bgr_int(color));
    }

    constexpr color_rgb int_to_working_color_space(uint32_t color)
    {
        return color::rgb_to_cas(int_to_linear(color));
    }

    inline prepared_scene prepare_scene(const cloudscape_scene& scene)
    {
        const auto sun_srgb = color::cas_to_rgb(sun_cas);
        const auto sun_cas_again = color::rgb_to_cas(sun_srgb);

        const auto wieghts_cas = color::rgb_to_cas(color::LUMINOCITY_WEIGHTS);

        const auto planet_center = vector3{0.0, 0.0, -scene.planet.planetradius};
        const auto camera_pos = vector3{scene.camera.x, scene.camera.y, scene.camera.z};

        auto spherical_phase_function = std::make_shared<participating_media::SphericalPhaseFunction>();
        auto rayleigh_phase_function = std::make_shared<participating_media::RayleighPhaseFunction>();
        auto sun_aware_rayleigh_phase_function = std::make_shared<participating_media::SunAwareRayleighPhaseFunction>(
            0.81,
            0.5,
            math::from_angles(math::deg_to_rad(scene.sun.azimuth), math::deg_to_rad(scene.sun.elevation))
            );
        auto aerosol_atmosphere_phase_function = std::make_shared<participating_media::HenyeyGreensteinPhaseFunction>(
            0.9);
        auto sun_aware_aerosol_atmosphere_phase_function = std::make_shared<participating_media::SunAwareHenyeyGreensteinPhaseFunction>(
            0.9,
            0.81,
            0.5,
            math::from_angles(math::deg_to_rad(scene.sun.azimuth), math::deg_to_rad(scene.sun.elevation))
            );
        auto henyey_greenstein_phase_function = std::make_shared<participating_media::HenyeyGreensteinPhaseFunction>(
            scene.clouds.fwd_bck);

        auto haze_phase_function = std::make_shared<participating_media::HenyeyGreensteinPhaseFunction>(
            scene.extensions.haze_fwd_bck);

        //auto atmospheric_medium = std::make_shared<participating_media::VoidMedium>();

        auto atmospheric_molecular_medium = std::make_shared<participating_media::AtmosphericMedium<decltype(
            rayleigh_phase_function)::element_type>>(
            scene.planet.planetradius,
            7994.0,
            planet_center,
            participating_media::optical_thickness_t::zero(),
            participating_media::optical_thickness_t{7.2865e-6, 1.2863e-5, 2.7408e-5},
            *rayleigh_phase_function
        );

        auto atmospheric_aerosol_medium = std::make_shared<participating_media::AtmosphericMedium<decltype(
            aerosol_atmosphere_phase_function)::element_type>>(
            scene.planet.planetradius,
            1200.0,
            planet_center,
            participating_media::optical_thickness_t::zero(),
            color_rgb::fill(turbidity_to_mie_extinction(7994.0, 1200.0, scene.planet.turbidity)),
            *aerosol_atmosphere_phase_function
        );

        auto haze_medium = std::make_shared<participating_media::AtmosphericMedium<decltype(
            haze_phase_function)::element_type>>(
            scene.planet.planetradius,
            scene.extensions.haze_scale_height,
            planet_center,
            participating_media::optical_thickness_t::zero(),
            color_rgb::fill(scene.extensions.haze_density),
            *haze_phase_function
        );


        auto homogeneous_medium = std::make_shared<participating_media::HomogeneousMedium<decltype(
            spherical_phase_function)::element_type>>(
            participating_media::optical_thickness_t::zero(),
            //participating_media::optical_thickness_t::fill(scene.clouds.fog),
            participating_media::optical_thickness_t{7.2865e-6, 1.2863e-5, 2.7408e-5} * 1000,
            participating_media::spectral_coeffs::zero(),
            *spherical_phase_function
        );

        auto lower_clouds_radius = scene.planet.planetradius + scene.clouds.height;
        auto upper_clouds_radius = scene.planet.planetradius + scene.clouds.height + scene.clouds.thickness;

        auto density_evaluator = [planet_center, lower_clouds_radius, upper_clouds_radius](const vector3& point)
        {
            const auto radius = math::length(point - planet_center);
            return color_real(std::clamp(std::lerp(space_real(0.0), space_real(0.01), radius - lower_clouds_radius),
                                         space_real(0.0), space_real(1.0))
                * std::clamp(std::lerp(space_real(0.0), space_real(0.01), upper_clouds_radius - radius),
                             space_real(0.0), space_real(1.0))
            );
            //return color_real(1);
        };

        auto perlin_cloud_medium = std::make_shared<participating_media::PerlinCloudsMedium<
            decltype(henyey_greenstein_phase_function)::element_type,
            decltype(density_evaluator)
        >>(
            participating_media::optical_thickness_t::zero(),
            participating_media::optical_thickness_t::fill(scene.clouds.fog),
            participating_media::spectral_coeffs::zero(),
            scene.noise.size,
            scene.clouds.coverage,
            scene.noise.detail,
            scene.noise.multiplier,
            std::move(density_evaluator),
            scene.noise.seed,
            *henyey_greenstein_phase_function
        );


        //auto cloud_medium = std::move(homogenous_cloud_medium);
        auto cloud_medium = std::move(perlin_cloud_medium);

        auto planet_material = std::make_shared<materials::BlinnMaterial>(
            color_rgb::zero(),
            nullptr,
            int_to_working_color_space(scene.planet.groundcolor),
            color_rgb::zero(),
            0.0,
            color_rgb::zero()
        );

        auto null_material = std::make_shared<materials::NullMaterial>();

        auto extra_material = std::make_shared<materials::BlinnMaterial>(
            color_rgb::zero(),
            nullptr,
            color_rgb::fill(1.0),
            color_rgb::zero(),
            0.0,
            color_rgb::zero()
        );

        auto glass_material = std::make_shared<materials::DielectricMaterial>(
            1.33,
            1.0,
            color_rgb::one()
        );

        auto planet_object = objects::SphereObject{
            planet_center,
            scene.planet.planetradius
        };

        planet_object.material(planet_material.get());


        // auto extra_sphere = objects::SphereObject{
        //     {1500.0, 100.0, 1500.0},
        //     500.0
        // };
        auto extra_sphere = objects::SphereObject{
            scene.extensions.sphere_pos,
            scene.extensions.sphere_radius
        };

        extra_sphere.material(glass_material.get());
        extra_sphere.inner_medium(homogeneous_medium.get());

        auto extra_triangle = objects::FlatTriangleObject(
            {1000.0, 0.0, 1500.1},
            {800.0, -200.0, 1500.1},
            {800.0, 200.0, 1500.1}
        );

        extra_triangle.material(extra_material.get());


        auto lower_clouds_bound = objects::SphereObject{
            planet_center, lower_clouds_radius, true
        };
        lower_clouds_bound.material(null_material.get());
        lower_clouds_bound.inner_medium(cloud_medium.get());
        auto upper_clouds_bound = objects::SphereObject{
            planet_center, upper_clouds_radius
        };
        upper_clouds_bound.material(null_material.get());
        upper_clouds_bound.inner_medium(cloud_medium.get());

        lights::DirectionalLightSource directional_sun{
            math::from_angles(math::deg_to_rad(scene.sun.azimuth), math::deg_to_rad(scene.sun.elevation)),
            sun_cas * int_to_linear(scene.sun.color) * scene.sun.multiplier
        };

        lights::SunLightSource sun{
            sun_cas * int_to_linear(scene.sun.color) * scene.sun.multiplier,
            std::atan(
                lights::SunLightSource::sun_radius * scene.sun.size_mult /
                lights::SunLightSource::earth_to_sun_distance) * 2.0,
            math::deg_to_rad(scene.sun.azimuth),
            math::deg_to_rad(scene.sun.elevation)
        };

        lights::PointLightSource point_light;
        point_light.position({100.0, 0.0, 20.0});
        point_light.color(color_rgb::fill(100000.0));


        return {
            scene,
            //vector3::fill(turbidity_to_mie_extinction(scaled_rayleigh_height, scaled_mie_height, scene.planet.turbidity)),
            std::move(spherical_phase_function),
            std::move(rayleigh_phase_function),
            std::move(sun_aware_rayleigh_phase_function),
            std::move(aerosol_atmosphere_phase_function),
            std::move(sun_aware_aerosol_atmosphere_phase_function),
            std::move(henyey_greenstein_phase_function),
            std::move(haze_phase_function),
            std::move(atmospheric_molecular_medium),
            std::move(atmospheric_aerosol_medium),
            std::move(homogeneous_medium),
            std::move(cloud_medium),
            std::move(haze_medium),
            std::move(planet_material),
            std::move(null_material),
            std::move(extra_material),
            std::move(glass_material),
            std::move(planet_object),
            std::move(lower_clouds_bound),
            std::move(upper_clouds_bound),
            std::move(extra_sphere),
            std::move(extra_triangle),
            std::move(sun),
            std::move(directional_sun),
            std::move(point_light),
            Camera{
                camera_pos,
                math::normalize(camera_pos - planet_center),
                math::from_angles(math::deg_to_rad(scene.camera.azimuth), math::deg_to_rad(scene.camera.elevation)),
                std::min(140.0f, scene.rendering.vangle)
            }
        };
    }
}
