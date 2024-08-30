#pragma once

#include "math/vector.hpp"
#include "math/vector_operations.hpp"

#include <cstdint>

namespace hashing
{
    constexpr uint32_t pcg_hash(uint32_t v)
    {
        auto state = v * 747796405u + 2891336453u;
        auto word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    constexpr math::vector<uint32_t, 2> pcg_hash(const math::vector<uint32_t, 2>& vec)
    {
        auto v = vec * 1664525u + math::vector<uint32_t, 2>::fill(1013904223u);

        v[0] += v[1] * 1664525u;
        v[1] += v[0] * 1664525u;

        v = v ^ (v >> 16u);

        v[0] += v[1] * 1664525u;
        v[1] += v[0] * 1664525u;

        v = v ^ (v >> 16u);

        return v;
    }

    constexpr math::vector<uint32_t, 3> pcg_hash(const math::vector<uint32_t, 3>& vec)
    {
        auto v = vec * 1664525u + math::vector<uint32_t, 3>::fill(1013904223u);

        v[0] += v[1] * v[2];
        v[1] += v[2] * v[0];
        v[2] += v[0] * v[1];

        v = v ^ (v >> 16u);

        v[0] += v[1] * v[2];
        v[1] += v[2] * v[0];
        v[2] += v[0] * v[1];

        return v;
    }

    constexpr math::vector<uint32_t, 4> pcg_hash(const math::vector<uint32_t, 4>& vec)
    {
        auto v = vec * 1664525u + math::vector<uint32_t, 4>::fill(1013904223u);

        v[0] += v[1] * v[3];
        v[1] += v[2] * v[0];
        v[2] += v[0] * v[1];
        v[3] += v[1] * v[2];

        v = v ^ (v >> 16u);

        v[0] += v[1] * v[3];
        v[1] += v[2] * v[0];
        v[2] += v[0] * v[1];
        v[3] += v[1] * v[2];

        return v;
    }
}
