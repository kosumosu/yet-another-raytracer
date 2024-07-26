#pragma once

#include "Marcher.h"
#include <memory>

class NullMarcher final : public Marcher {
    const std::vector<GeometryObject *> *m_objects;
    bool m_marched;

public:
    // for performance issues we are not using smart pointers
    explicit NullMarcher(const std::vector<GeometryObject *> *objects);

    void Restart(const ray3 &ray, space_real near, space_real far) override;

    const std::vector<GeometryObject *> *GetCurrentObjects() override;

    bool MarcheNext() override;

    bool IsCorrectIntersectionForCurrentState(space_real dist) const override;
};

