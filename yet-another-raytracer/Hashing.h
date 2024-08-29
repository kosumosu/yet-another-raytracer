#pragma once

#include "math/vector.hpp"

#include <cstdint>

constexpr inline uint32_t xxhash32(uint32_t p)
{
	const uint32_t PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint32_t PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint32_t h32 = p + PRIME32_5;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

constexpr inline uint32_t xxhash32(const math::vector<uint32_t, 2>& vec)
{
	const uint32_t PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint32_t PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint32_t h32 = vec[1] + PRIME32_5 + vec[0] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

constexpr inline uint32_t xxhash32(const math::vector<uint32_t, 3>& vec)
{
	const uint32_t PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint32_t PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint32_t h32 = vec[2] + PRIME32_5 + vec[0] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec[1] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

constexpr inline uint32_t xxhash32(const math::vector<uint32_t, 4>& vec)
{
	const uint32_t PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint32_t PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint32_t h32 = vec[3] + PRIME32_5 + vec[0] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec[1] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec[2] * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}
