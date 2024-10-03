#pragma once

#include <cstdint>
#include "immintrin.h"
#include "vector_utilities.hpp"

class RiceMap
{
public:
    static constexpr inline uint16_t encode_single_sample(const int16_t x);
    static constexpr inline int16_t decode_single_sample(const uint16_t x);
    static inline __m256i encode_single_sample(const __m256i &x);
    static inline __m256i decode_single_sample(const __m256i &x);
    static inline __m128i encode_single_sample(const __m128i &x);
    static inline __m128i decode_single_sample(const __m128i &x);
};

// Implementation copied from: https://github.com/nanoporetech/pod5-file-format/blob/3da54259cf42d656a7332e2867fffb4c25e36a0f/c%2B%2B/pod5_format/svb16/encode_scalar.hpp#L13
// TODO: revise
// TODO: this implementation heavily relies on 2's compliment representation
// TODO: For AVX this is OK, but I'd like to revise the portability of such assumptions for the scalar code
// Maybe wrapping such code with a template compile time trick testing for the assumption can make code more robust
constexpr uint16_t RiceMap::encode_single_sample(const int16_t x)
{
    return (x + x) ^ static_cast<uint16_t>(static_cast<int16_t>(x) >> 15);
} // TODO: can val + val be replaced by val << 1?

constexpr int16_t RiceMap::decode_single_sample(const uint16_t x)
{
    return (x >> 1) ^ static_cast<uint16_t>(0 - (x & 1)); // TODO: Once again this assumes 2's compliment, right?
}

// Implementation copied from: https://github.com/nanoporetech/pod5-file-format/blob/3da54259cf42d656a7332e2867fffb4c25e36a0f/c%2B%2B/pod5_format/svb16/encode_x64.hpp#L23
// (Adapted for AVX256 instead of SSSE3) => TODO: revise for correctness
__m256i RiceMap::encode_single_sample(const __m256i & x)
{
    // TODO: why in ONT code they use _mm_alignr_epi8
    //  It could be to just encode the last sample
    //  That'd mean that the provided code is not correct
    return _mm256_xor_si256(_mm256_add_epi16(x, x), _mm256_srai_epi16(x, 16));
}

__m256i RiceMap::decode_single_sample(const __m256i &x)
{
    return _mm256_xor_si256(_mm256_srli_epi16(x, 1),
                     _mm256_sub_epi16(
                         _mm256_setzero_si256(),
                         _mm256_and_si256(x, lowest_bit_mask)));
}

inline __m128i RiceMap::encode_single_sample(const __m128i &x)
{
    return _mm_xor_si128(_mm_add_epi16(x, x), _mm_srai_epi16(x, 16));
}

inline __m128i RiceMap::decode_single_sample(const __m128i &x)
{
    return _mm_xor_si128(_mm_srli_epi16(x, 1),
                         _mm_sub_epi16(_mm_setzero_si128(),
                                       _mm_and_si128(x, lowest_bit_mask_128)));
}