#pragma once

#include "NullMarcher.h"
#include "../Accelerator.h"
#include <vector>


namespace accelerators::null
{
    class NullAccelerator final : public Accelerator<NullMarcher>
    {
    public:
        explicit NullAccelerator(std::vector<objects::GeometryObject*> objects): m_objects{std::move(objects)}
        {
            // m_objects.resize(objects.size());
            // std::transform(std::begin(objects), std::end(objects), std::begin(m_objects), [](auto objectPtr) { return objectPtr.get(); });
        }

        [[nodiscard]] NullMarcher CreateMarcher() const override
        {
            return NullMarcher(&m_objects);
        }

    private:
        std::vector<objects::GeometryObject*> m_objects;
    };
}
