#pragma once

#include "math/vector.hpp"
#include "xxhash32.h"
#include "pcg_hash.h"

#include <cstdint>

namespace hashing
{
	constexpr uint32_t default_1d_hash(uint32_t p)
	{
		return xxhash32(p);
	}

	constexpr uint32_t default_1d_hash(const math::vector<uint32_t, 2>& vec)
	{
		return xxhash32(vec);
	}

	constexpr uint32_t default_1d_hash(const math::vector<uint32_t, 3>& vec)
	{
		return xxhash32(vec);
	}

	constexpr uint32_t default_1d_hash(const math::vector<uint32_t, 4>& vec)
	{
		return xxhash32(vec);
	}
}