#pragma once

#include <cstdint>

#include "immintrin.h"

#define SPLIT_INTO_BYTES(value, high, low) \
    *high = (value & 0xFF00) >> 8;         \
    *low = value & 0xFF;

namespace compression
{
namespace util
{
namespace bit
{
    int popcount(uint16_t x)
    {
        return _mm_popcnt_u64(x);
    }

    inline int popcount(uint32_t x)
    {
        return _mm_popcnt_u32(x);
    }
}
}
}