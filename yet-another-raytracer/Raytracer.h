#pragma once

#include "Hit.h"
#include "Marcher.h"

#include <concepts>
#include <functional>

using accept_predicate = std::function<bool(const Hit& hit)>;

template <class T>
concept Marcher_c = std::derived_from<T, Marcher>;

template <Marcher_c TMarcher>
class Raytracer
{
    TMarcher m_marcher;
public:
    explicit Raytracer(TMarcher marcher)
        : m_marcher(std::move(marcher))
    {
    }


    [[nodiscard]] Hit TraceRay(const ray3& ray, space_real bias)
    {
        return TraceRay(ray, bias, std::numeric_limits<space_real>::max(), nullptr, vector3::zero());
    }

    // intersections with objectToIgonore are ignored if (ignoreBackface == true and ray hits it's backface) or (ignoreBackface == false and ray hits frontface).
    [[nodiscard]] Hit TraceRay(const ray3& ray, space_real minDistance, space_real maxDistance, const GeometryObject* objectToIgnore,
                 const vector3& directionToIgnore)
    {
        m_marcher.Restart(ray, minDistance, maxDistance);

        Hit nearest_hit;

        while (m_marcher.MarcheNext())
        {
            const auto objects = m_marcher.GetCurrentObjects();

            for (const auto & object : *objects)
            {
                const auto hit = object->FindHit(ray, minDistance, maxDistance);
                if (hit.has_occurred())
                {
                    if (!nearest_hit.has_occurred() || hit.distance() < nearest_hit.distance())
                    {
                        if (object != objectToIgnore || math::is_obtuse_angle(ray.direction(), directionToIgnore))
                            nearest_hit = hit;
                    }
                }
            }

            // If found and it's guaranteed to be the closest one
            if (nearest_hit.has_occurred() && m_marcher.IsCorrectIntersectionForCurrentState(nearest_hit.distance()))
                return nearest_hit;
        }

        return nearest_hit;
    }

    [[nodiscard]] bool DoesIntersect(const ray3& ray, space_real minDistance, space_real maxDistance,
                       const GeometryObject* objectToIgnore, const vector3& directionToIgnore)
    {
        m_marcher.Restart(ray, minDistance, maxDistance);
        while (m_marcher.MarcheNext())
        {
            const auto objects = m_marcher.GetCurrentObjects();

            for (const auto & object : *objects)
            {
                const auto hit = object->FindHit(ray, minDistance, maxDistance);
                if (hit.has_occurred()
                    && (object != objectToIgnore || math::is_obtuse_angle(ray.direction(), directionToIgnore))
                    )
                {
                    return true;
                }
            }
        }

        // Nothing found
        return false;
    }
};
