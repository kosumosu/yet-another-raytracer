#include "NullMarcher.h"

NullMarcher::NullMarcher(const std::vector<GeometryObject *> *objects)
        : m_objects(objects), m_marched(false) {
}

void NullMarcher::Restart(const ray3 &ray, space_real near, space_real) {
    m_marched = false;
}

const std::vector<GeometryObject *> *NullMarcher::GetCurrentObjects() {
    return m_objects;
}

bool NullMarcher::MarcheNext() {
    const bool marched = m_marched;
    m_marched = true;
    return !marched;
}

bool NullMarcher::IsCorrectIntersectionForCurrentState(space_real dist) const {
    return true;
}
