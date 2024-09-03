#pragma once

#include "Marcher.h"

namespace accelerators::null
{
    class NullMarcher final : public Marcher
    {
        const std::vector<objects::GeometryObject*>* m_objects;
        bool m_marched;

    public:
        // for performance issues we are not using smart pointers
        explicit NullMarcher(const std::vector<objects::GeometryObject*>* objects) : m_objects(objects),
            m_marched(false)
        {
        }

        void Restart(const ray3& ray, space_real near, space_real far) override
        {
            m_marched = false;
        }

        const std::vector<objects::GeometryObject*>* GetCurrentObjects() override
        {
            return m_objects;
        }

        bool MarcheNext() override
        {
            const bool marched = m_marched;
            m_marched = true;
            return !marched;
        }

        bool IsCorrectIntersectionForCurrentState(space_real dist) const override
        {
            return true;
        }
    };
}
