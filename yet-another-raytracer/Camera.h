#pragma once

#include "math/ray.hpp"

#include "Types.h"
#include "math/angles.h"


class Camera
{
public:
    Camera()
        : m_position(1.0, 0.0, 0.0)
          , m_up(0.0, 1.0, 0.0)
          , m_direction(vector3::zero())
          , m_fovy_degrees(60.0)
          , m_tan_half_fov_y(std::tan(math::deg_to_rad(m_fovy_degrees * space_real(0.5))))
    {
    }

    Camera(vector3 position, vector3 up, vector3 direction, space_real fovy_degrees)
        : m_position(std::move(position))
          , m_up(std::move(up))
          , m_direction(std::move(direction))
          , m_fovy_degrees(fovy_degrees)
          , m_tan_half_fov_y(std::tan(space_real(math::pi) * fovy_degrees / space_real(360.0)))
    {
    }


    const vector3& position() const { return m_position; }

    void position(const vector3& value)
    {
        m_position = value;
    }

    const vector3& up() const { return m_up; }
    void up(const vector3& value) { m_up = value; }

    void target(const vector3& value)
    {
        m_direction = math::normalize(value - m_position);
    }

    space_real fovy_degrees() const { return m_fovy_degrees; }

    void fovy_degrees(space_real value)
    {
        m_fovy_degrees = value;
        m_tan_half_fov_y = std::tan(math::deg_to_rad(m_fovy_degrees * space_real(0.5)));
    }

    ray3 GetViewRay(vector2 imageSpaceCoord, space_real aspect) const
    {
        auto w = m_direction;
        auto v = m_up;
        auto u = math::cross(w, v);

        auto scale_y = m_tan_half_fov_y;
        auto scale_x = m_tan_half_fov_y * aspect;

        vector3 ray_direction = w + u * (MapToCamera(imageSpaceCoord[0]) * scale_x) - v * (
            MapToCamera(imageSpaceCoord[1]) * scale_y); // -v because of inverted y

        return ray3(m_position, math::normalize(ray_direction));
    }

private:
    vector3 m_position;
    vector3 m_up;
    vector3 m_direction;

    space_real m_fovy_degrees;
    space_real m_tan_half_fov_y;

    static space_real MapToCamera(space_real value)
    {
        return (value - 0.5f) * 2.0f;
    }
};
