#pragma once

#include "constants.hpp"
#include "vector_utilities.hpp"

#include "immintrin.h"

#include <cstdint>
#include <type_traits>
#include <vector>
#include <iostream>

template<typename T>
class SimpleBlockDeltaZigZag
{
public:
    SimpleBlockDeltaZigZag()
    {
        static_assert(std::is_same_v<T, __m256i>, "Only __m256i is supported");
    }

    void encode(const int16_t* in_ptr, uint16_t* out_ptr, const size_t size) const noexcept;
    void decode(const uint16_t* in_ptr, int16_t* out_ptr, const size_t size) const noexcept;
};

template<>
inline void SimpleBlockDeltaZigZag<__m256i>::encode(const int16_t* in_ptr, uint16_t* out_ptr, const size_t size) const noexcept
{
    auto current_in_ptr = static_cast<const __m256i*>(static_cast<const void*>((in_ptr)));
    auto current_out_ptr = static_cast<__m256i*>(static_cast<void*>((out_ptr)));

    auto const parallel_for_times = size / DomainConstants::samples_per_avx_register;
    auto const tail_for_times = size % DomainConstants::samples_per_avx_register;

    __m256i previous_block;

    previous_block = _mm256_setzero_si256();

    for (size_t i = parallel_for_times; i > 0; i--, current_in_ptr++, current_out_ptr++)
    {
        auto const this_block = _mm256_loadu_si256(current_in_ptr);
        auto const difference = _mm256_sub_epi16(this_block, previous_block);
        previous_block = this_block;
        _mm256_storeu_si256(current_out_ptr, RiceMap::encode_single_sample(difference));
    }

    /*
        Encode tail
        For GCC, _mm256_extract_epi16 must be passed a known constant integer as a second argument
        Hence, we unroll the loop
    */

    foreach_in_mm256_tail_extract_epi16([current_in_ptr, current_out_ptr](auto y, auto i) {
        auto const this_sample = *(static_cast<const int16_t*>(static_cast<const void*>(current_in_ptr)) + i);
        auto const difference = this_sample - y;
        *(static_cast<int16_t*>(static_cast<void*>(current_out_ptr)) + i) = RiceMap::encode_single_sample(difference);
    }, previous_block, tail_for_times);
}

template<>
inline void SimpleBlockDeltaZigZag<__m256i>::decode(const uint16_t* in_ptr, int16_t* out_ptr, const size_t size) const noexcept
{
    auto current_in_ptr = static_cast<const __m256i*>(static_cast<const void*>((in_ptr)));
    auto current_out_ptr = static_cast<__m256i*>(static_cast<void*>((out_ptr)));

    auto const parallel_for_times = size / DomainConstants::samples_per_avx_register;
    auto const tail_for_times = size % DomainConstants::samples_per_avx_register;

    __m256i previous_block;

    previous_block = _mm256_setzero_si256();

    for (size_t i = parallel_for_times; i > 0; i--, current_in_ptr++, current_out_ptr++)
    {
        auto const this_block = _mm256_loadu_si256(current_in_ptr);
        auto const decoded = _mm256_add_epi16(RiceMap::decode_single_sample(this_block), previous_block);
        previous_block = decoded;
        _mm256_storeu_si256(current_out_ptr, decoded);
    }

    foreach_in_mm256_tail_extract_epi16([current_in_ptr, current_out_ptr](auto y, auto i) {
        auto const this_sample = *(static_cast<const int16_t*>(static_cast<const void*>(current_in_ptr)) + i);
        auto const decoded = RiceMap::decode_single_sample(this_sample) + y;
        *(static_cast<int16_t*>(static_cast<void*>(current_out_ptr)) + i) = decoded;
    }, previous_block, tail_for_times);
}
