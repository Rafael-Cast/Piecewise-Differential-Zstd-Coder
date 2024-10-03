#pragma once

#include "attributes.hpp"

#include "immintrin.h"

#include <cstdint>
#include <exception>
#include <stdexcept>

/* 16xi16 bitmask for high byte */
const __m256i high_byte_mask = _mm256_set1_epi16(0xFF00);
/* 16xi16 bitmask for low byte */
const __m256i low_byte_mask = _mm256_set1_epi16(0xFF);
/* 16xi16 bitmask for lowest bit */
const __m256i lowest_bit_mask = _mm256_set1_epi16(0x01);
const __m128i lowest_bit_mask_128 = _mm_set1_epi16(0x01);

/*
    Given a vector of 16xi16 with the keys scattered one key per int, pack the keys into a single 16 bit integer

    TODO: review that the order of the keys is correct

    Implementation notes: For this implementation to work, _mm256_extract_epi16 need to be passed a second constant argument.
                          Hence, we implement loop unrolling.
*/
inline uint16_t pack_keys(const __m256i &x)
{
    uint16_t packed_keys;
    packed_keys = 0;

    packed_keys |= _mm256_extract_epi16(x, 0);
    packed_keys |= _mm256_extract_epi16(x, 1) << 1;
    packed_keys |= _mm256_extract_epi16(x, 2) << 2;
    packed_keys |= _mm256_extract_epi16(x, 3) << 3;
    packed_keys |= _mm256_extract_epi16(x, 4) << 4;
    packed_keys |= _mm256_extract_epi16(x, 5) << 5;
    packed_keys |= _mm256_extract_epi16(x, 6) << 6;
    packed_keys |= _mm256_extract_epi16(x, 7) << 7;
    packed_keys |= _mm256_extract_epi16(x, 8) << 8;
    packed_keys |= _mm256_extract_epi16(x, 9) << 9;
    packed_keys |= _mm256_extract_epi16(x, 10) << 10;
    packed_keys |= _mm256_extract_epi16(x, 11) << 11;
    packed_keys |= _mm256_extract_epi16(x, 12) << 12;
    packed_keys |= _mm256_extract_epi16(x, 13) << 13;
    packed_keys |= _mm256_extract_epi16(x, 14) << 14;
    packed_keys |= _mm256_extract_epi16(x, 15) << 15;

    return packed_keys;
}

inline uint32_t pack_2bit_keys(const __m256i & x)
{
    uint32_t packed_keys = 0;

    packed_keys |= _mm256_extract_epi16(x, 0);
    packed_keys |= _mm256_extract_epi16(x, 1) << 2;
    packed_keys |= _mm256_extract_epi16(x, 2) << 4;
    packed_keys |= _mm256_extract_epi16(x, 3) << 6;
    packed_keys |= _mm256_extract_epi16(x, 4) << 8;
    packed_keys |= _mm256_extract_epi16(x, 5) << 10;
    packed_keys |= _mm256_extract_epi16(x, 6) << 12;
    packed_keys |= _mm256_extract_epi16(x, 7) << 14;
    packed_keys |= _mm256_extract_epi16(x, 8) << 16;
    packed_keys |= _mm256_extract_epi16(x, 9) << 18;
    packed_keys |= _mm256_extract_epi16(x, 10) << 20;
    packed_keys |= _mm256_extract_epi16(x, 11) << 22;
    packed_keys |= _mm256_extract_epi16(x, 12) << 24;
    packed_keys |= _mm256_extract_epi16(x, 13) << 26;
    packed_keys |= _mm256_extract_epi16(x, 14) << 28;
    packed_keys |= _mm256_extract_epi16(x, 15) << 30;

    return packed_keys;
}

inline __m256i compute_keys(const __m256i &x)
{
    // TODO: reimplement with _mm256_cmp_epi16_mask
    // FIXME: Readup _mm256_cmpeq_epi16_mask => Even though it seems perfect, I believe that uses AVX512 instruction set
    // FIXME: another implementation might rely on __mm256_bitshuffle_epi64_mask
    return _mm256_andnot_si256(_mm256_cmpeq_epi16(x, _mm256_setzero_si256()), lowest_bit_mask);
}

/*
    Implementation notes: For this implementation to work, _mm256_extract_epi16 need to be passed a second constant argument.
                          Hence, we implement loop unrolling.
*/
inline void encode_high_bytes(const __m256i &high_bytes, uint8_t *&bytes)
{
    // TODO: these if branches can be ommited with branchless programming.
    /*
    TODO: Try something similar to:
    *bytes ^= current_byte ? current_byte : 0; => cmp_neq * current_byte
    bytes += current_byte ? 1 : 0; => cmp_neq (cmp_neq un macro o instruccion de CPU con template y compilacion condicional para elegir implementacion más eficiente => Así no dependes CPU pero te aseguras abstraccion de que neq da 0 o 1 y no 0 o 0xFFFF)

    Please unit test before implementing the branchless alternative
    */

    // TODO: see if some masked AVX operation can't perform this
    uint8_t current_byte;
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 0));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 1));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 2));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 3));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 4));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 5));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 6));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 7));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 8));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 9));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 10));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 11));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 12));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 13));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 14));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
    current_byte = static_cast<uint8_t>(_mm256_extract_epi16(high_bytes, 15));
    if (current_byte)
    {
        *bytes = current_byte;
        bytes++;
    }
}

// TODO: this function is QUITE bad, try to avoid it!
inline void _mm256_storeu_si128(__m256i *const parallel_nanopore_restrict buffer, const __m256i &parallel_nanopore_restrict x)
{
    const __m256i mask = _mm256_set_epi64x(-1, -1, 0, 0);
    const __m256i current_contents = _mm256_loadu_si256(buffer);
    _mm256_storeu_si256(buffer, _mm256_blendv_epi8(x, current_contents, mask));
}

/*
* {x0, x1, x2, ..., x15} -> {0, x1, x2, ..., x14}
*/
inline __m256i _mm256_slr_1_epi16_element(__m256i x)
{
    // {0, x0, x1, x2, x3, x4, x5, x6,  0, x8, x9, x10, x11, x12, x13, x14}
    const __m256i sx = _mm256_srli_si256(x, 2);
    
    const __m256i mask = _mm256_set_epi16(0, 0, 0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0);
    // {0,  0,  0,  0,  0,  0,  0,  0, x8,  0,  0,   0,   0,   0,   0,   0}
    const __m256i mx = _mm256_and_si256(mask, x);
    // {x8,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,   0,   0,   0,   0,   0}
    const __m256i mx_p = _mm256_permute2f128_si256(mx, _mm256_setzero_si256(), 0x81);
    // {0,  0,  0,  0,  0,  0,  0,  x8, 0,  0,  0,   0,   0,   0,   0,   0}
    const __m256i mx_pp = _mm256_slli_si256(mx_p, 14);
    
    return _mm256_or_si256(mx_pp, sx);
}

inline __m256i _mm256_swap128_si256(__m256i x)
{
    return _mm256_permute2x128_si256(x, _mm256_setzero_si256(), 1);
}

inline __m256i_u _mm256_not_si256(const __m256i &x)
{
    const __m256i_u mask = _mm256_set1_epi32(-1);
    return _mm256_xor_si256(x, mask);
}

inline __m256i_u _mm256_not_zero_epu16(const __m256i_u & x)
{
    return _mm256_not_si256(_mm256_cmpeq_epi16(x, _mm256_setzero_si256()));
}

/*
* {x0, ..., x15}
* li = xi & 0xFF
* --------------
* { l0[0:7], l8[0:7], l1[0:7], l9[0:7], ..., l15[0:7], 0[0:127]} (reordered)
*/
template<bool AllowReorder = false>
inline __m256i pack_low_bytes(__m256i x)
{
    auto swapped = _mm256_swap128_si256(x);
    auto shifted = _mm256_slli_epi16(swapped, 8);
    auto res = _mm256_castsi128_si256(_mm_xor_si128(_mm256_castsi256_si128(x), _mm256_castsi256_si128(shifted)));
    if constexpr (AllowReorder)
    {
        return res;
    }
    else
    {
        throw new std::exception();
    }
}


template<bool AllowReorder = false>
inline __m256i unpack_low_bytes(__m256i x)
{
    const static __m256i low_mask = _mm256_set1_epi16(0xFF);
    const static __m256i high_mask = _mm256_set1_epi16(0xFF00);

    auto low_dirty = _mm256_and_si256(x, low_mask);
    auto high_dirty = _mm256_and_si256(x, high_mask);
    
    // These ands can be ommited if the high 128 lane is a assumed all 0's
    const static auto cleanup_mask = _mm256_set_m128i(_mm_setzero_si128(), _mm_set1_epi16(-1));
    auto low = _mm256_and_si256(low_dirty, cleanup_mask);
    auto high = _mm256_and_si256(high_dirty, cleanup_mask);

    auto res = _mm256_set_m128i(_mm256_castsi256_si128(_mm256_srli_epi16(high, 8)), _mm256_castsi256_si128(low));
    if constexpr (AllowReorder)
    {
        return res;
    }
    else
    {
        throw new std::exception();
    }
}

//template<bool AllowReorder = false>
//inline __m256i unpack_low_bytes(__m256i x)
//{
//    const static __m256i low_mask = _mm256_set1_epi16(0xFF);
//    const static __m256i high_mask = _mm256_set1_epi16(0xFF00);
//
//    auto low_dirty = _mm256_and_si256(x, low_mask);
//    auto high_dirty = _mm256_and_si256(x, high_mask);
//    
//    // These ands can be ommited if the high 128 lane is a assumed all 0's
//    const static auto cleanup_mask = _mm256_set_m128i(_mm_set1_epi16(-1), _mm_setzero_si128());
//    auto low = _mm256_and_si256(low_dirty, cleanup_mask);
//    auto high = _mm256_and_si256(high_dirty, cleanup_mask);
//
//    auto res = _mm256_xor_si256(
//        low,
//        _mm256_swap128_si256(_mm256_srli_epi16(high, 8))
//    );
//    if constexpr (AllowReorder)
//    {
//        return res;
//    }
//    else
//    {
//        throw new std::exception();
//    }
//}

/*
//GPT code
template <int N, typename Func>
void unroll_loop(Func&& func) {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (func(std::integral_constant<int, Is>{}), ...);
    }(std::make_index_sequence<N>{});
}
*/

/*
    This function is QUITE slow, it should only be used on non performance critical tail operations
*/
template<typename Func>
inline void foreach_in_mm256_tail_extract_epi16(Func && func, const __m256i& x, uint_fast8_t n)
{
    if (n >= 1)
    {
        int16_t y = _mm256_extract_epi16(x, 0);
        func(y, 0);
    }
    if (n >= 2)
    {
        auto y = _mm256_extract_epi16(x, 1);
        func(y, 1);
    }
    if (n >= 3)
    {
        int16_t y = _mm256_extract_epi16(x, 2);
        func(y, 2);
    }
    if (n >= 4)
    {
        auto y = _mm256_extract_epi16(x, 3);
        func(y, 3);
    }
    if (n >= 5)
    {
        int16_t y = _mm256_extract_epi16(x, 4);
        func(y, 4);
    }
    if (n >= 6)
    {
        auto y = _mm256_extract_epi16(x, 5);
        func(y, 5);
    }
    if (n >= 7)
    {
        int16_t y = _mm256_extract_epi16(x, 6);
        func(y, 6);
    }
    if (n >= 8)
    {
        auto y = _mm256_extract_epi16(x, 7);
        func(y, 7);
    }
    if (n >= 9)
    {
        int16_t y = _mm256_extract_epi16(x, 8);
        func(y, 8);
    }
    if (n >= 10)
    {
        auto y = _mm256_extract_epi16(x, 9);
        func(y, 9);
    }
    if (n >= 11)
    {
        int16_t y = _mm256_extract_epi16(x, 10);
        func(y, 10);
    }
    if (n >= 12)
    {
        auto y = _mm256_extract_epi16(x, 11);
        func(y, 11);
    }
    if (n >= 13)
    {
        int16_t y = _mm256_extract_epi16(x, 12);
        func(y, 12);
    }
    if (n >= 14)
    {
        auto y = _mm256_extract_epi16(x, 13);
        func(y, 13);
    }
    if (n >= 15)
    {
        int16_t y = _mm256_extract_epi16(x, 14);
        func(y, 14);
    }
    if (n >= 16)
    {
        auto y = _mm256_extract_epi16(x, 15);
        func(y, 15);
    }
}

/*
* Some GCC versions wont allow passing a non immediate argument to _extract_epi
* hence we implement this SLOW utility function for testing purposes
*/
uint8_t _mm256_extract_epi8_runtime(const __m256i& x, int n)
{
    switch (n)
    {
    case 0:
        return _mm256_extract_epi8(x, 0);
    case 1:
        return _mm256_extract_epi8(x, 1);
    case 2:
        return _mm256_extract_epi8(x, 2);
    case 3:
        return _mm256_extract_epi8(x, 3);
    case 4:
        return _mm256_extract_epi8(x, 4);
    case 5:
        return _mm256_extract_epi8(x, 5);
    case 6:
        return _mm256_extract_epi8(x, 6);
    case 7:
        return _mm256_extract_epi8(x, 7);
    case 8:
        return _mm256_extract_epi8(x, 8);
    case 9:
        return _mm256_extract_epi8(x, 9);
    case 10:
        return _mm256_extract_epi8(x, 10);
    case 11:
        return _mm256_extract_epi8(x, 11);
    case 12:
        return _mm256_extract_epi8(x, 12);
    case 13:
        return _mm256_extract_epi8(x, 13);
    case 14:
        return _mm256_extract_epi8(x, 14);
    case 15:
        return _mm256_extract_epi8(x, 15);
    case 16:
        return _mm256_extract_epi8(x, 16);
    case 17:
        return _mm256_extract_epi8(x, 17);
    case 18:
        return _mm256_extract_epi8(x, 18);
    case 19:
        return _mm256_extract_epi8(x, 19);
    case 20:
        return _mm256_extract_epi8(x, 20);
    case 21:
        return _mm256_extract_epi8(x, 21);
    case 22:
        return _mm256_extract_epi8(x, 22);
    case 23:
        return _mm256_extract_epi8(x, 23);
    case 24:
        return _mm256_extract_epi8(x, 24);
    case 25:
        return _mm256_extract_epi8(x, 25);
    case 26:
        return _mm256_extract_epi8(x, 26);
    case 27:
        return _mm256_extract_epi8(x, 27);
    case 28:
        return _mm256_extract_epi8(x, 28);
    case 29:
        return _mm256_extract_epi8(x, 29);
    case 30:
        return _mm256_extract_epi8(x, 30);
    case 31:
        return _mm256_extract_epi8(x, 31);
    default:
        throw std::out_of_range("index out of range");
    }
}