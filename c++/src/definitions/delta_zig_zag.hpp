#pragma once

#include "vector_utilities.hpp"
#include "rice_map.hpp"
#include "delta.hpp"

#include "immintrin.h"

#include "cstdint"
#include <functional>

#include <exception>

class DeltaZigZag
{
public:
    template <bool InPlace = true, bool AvxEnabled = true>
    static inline uint16_t *encode(int16_t *buffer, size_t count);
    template <bool InPlace = true, bool AvxEnabled = true>
    static inline int16_t *decode(uint16_t *buffer, size_t count);

    static inline void encode_avx(int16_t *input_buffer, uint16_t *output_buffer, size_t count);
    static inline void decode_avx(uint16_t *input_buffer, int16_t *output_buffer, size_t count);
    static inline void encode_scalar(int16_t *input_buffer, uint16_t *output_buffer, size_t count);
    static inline void decode_scalar(uint16_t *input_buffer, int16_t *output_buffer, size_t count);
private:

    static constexpr uint16_t encode_single_sample(const int16_t current, const int16_t previous);
    static __m256i encode_single_sample(const __m256i &current, const __m256i &previous);
    static constexpr int16_t decode_single_sample(const uint16_t current, const int16_t previous);
    static __m256i decode_single_sample(const __m256i &current, const __m256i &previous);

    static constexpr size_t samples_per_parallel_chunk = 256 / 16;
};

template <bool InPlace, bool AvxEnabled, typename TIn, typename TOut, void(f_scalar)(TIn *, TOut *, size_t), void(f_avx)(TIn *, TOut *, size_t)>
inline TOut *run_algorithm(TIn *input_buffer, size_t count)
{
    TOut *output_buffer;
    if constexpr (InPlace)
    {
        output_buffer = reinterpret_cast<TOut *>(input_buffer);
    }
    else
    {
        output_buffer = new TOut[count];
    }
    if constexpr (AvxEnabled)
    {
        f_avx(input_buffer, output_buffer, count);
    }
    else
    {
        f_scalar(input_buffer, output_buffer, count);
    }
    return output_buffer;
}

template <bool InPlace, bool AvxEnabled>
inline uint16_t *DeltaZigZag::encode(int16_t *buffer, size_t count)
{
    return run_algorithm<InPlace, AvxEnabled, int16_t, uint16_t, DeltaZigZag::encode_scalar, DeltaZigZag::encode_avx>(buffer, count);
}

template <bool InPlace, bool AvxEnabled>
inline int16_t *DeltaZigZag::decode(uint16_t *buffer, size_t count)
{
    return run_algorithm<InPlace, AvxEnabled, uint16_t, int16_t, DeltaZigZag::decode_scalar, DeltaZigZag::decode_avx>(buffer, count);
}

constexpr uint16_t DeltaZigZag::encode_single_sample(const int16_t current, const int16_t previous)
{
    // need to do the arithmetic in unsigned space so it wraps => TODO: I didn't understand this comment
    auto val = static_cast<uint16_t>(current - previous);
    return RiceMap::encode_single_sample(val);
}

__m256i DeltaZigZag::encode_single_sample(const __m256i &current, const __m256i &previous)
{
    __m256i val = _mm256_sub_epi16(current, previous);
    return RiceMap::encode_single_sample(val);
}

constexpr int16_t DeltaZigZag::decode_single_sample(const uint16_t current, const int16_t previous)
{
    int16_t unriced = RiceMap::decode_single_sample(current);
    return unriced + previous;
}

__m256i DeltaZigZag::decode_single_sample(const __m256i &current, const __m256i &previous)
{
    __m256i unriced = RiceMap::decode_single_sample(current);
    return _mm256_add_epi16(unriced, previous);
}

void DeltaZigZag::encode_scalar(int16_t *input_buffer, uint16_t *output_buffer, size_t count)
{
    int16_t const *current_input_pointer = input_buffer;
    int16_t const *end_input_ponter = current_input_pointer + count;
    uint16_t *current_output_pointer = output_buffer;
    int16_t previous = 0;
    for (; current_input_pointer != end_input_ponter; current_input_pointer++, current_output_pointer++)
    {
        auto current = DeltaZigZag::encode_single_sample(*current_input_pointer, previous);
        previous = *current_input_pointer;
        *current_output_pointer = current;
    }
}

void DeltaZigZag::encode_avx(int16_t *input_buffer, uint16_t *output_buffer, size_t count)
{
    if (count < samples_per_parallel_chunk)
    {
        encode_scalar(input_buffer, output_buffer, count);
        return;
    }

    //Delta::encode<true>(input_buffer, count);//FIXME: this encoding cannot be done inplace

    input_buffer = Delta::encode<false>(input_buffer, count);

    __m256i const *current_parallel_input_buffer = reinterpret_cast<__m256i const *>(input_buffer);
    __m256i const *end_parallel_input_buffer = current_parallel_input_buffer + (count / samples_per_parallel_chunk);
    auto current_parallel_output_buffer = reinterpret_cast<__m256i *>(output_buffer);

    for (; current_parallel_input_buffer != end_parallel_input_buffer; current_parallel_input_buffer++)
    {
        __m256i chunk = _mm256_loadu_si256(current_parallel_input_buffer);
        __m256i coded_sample = RiceMap::encode_single_sample(chunk);
        _mm256_storeu_si256(current_parallel_output_buffer, coded_sample);
        current_parallel_output_buffer++;
    }

    for (size_t i = 0; i < count % samples_per_parallel_chunk; i++)
    {
        int16_t sample = *(reinterpret_cast<int16_t const *>(current_parallel_input_buffer) + i);
        uint16_t coded_sample = RiceMap::encode_single_sample(sample);
        *(reinterpret_cast<int16_t *>(current_parallel_output_buffer) + i) = coded_sample;
    }
    delete[] input_buffer;
}

inline void DeltaZigZag::decode_scalar(uint16_t *input_buffer, int16_t *output_buffer, size_t count)
{
    uint16_t const *current_input_pointer = input_buffer;
    uint16_t const *const end_input_pointer = current_input_pointer + count;
    int16_t *current_output_pointer = output_buffer;
    int16_t previous = 0;
    for (; current_input_pointer != end_input_pointer; current_input_pointer++, current_output_pointer++)
    {
        uint16_t decoded_sample = DeltaZigZag::decode_single_sample(*current_input_pointer, previous);
        *current_output_pointer = decoded_sample;
        previous = decoded_sample;
    }
}


inline void DeltaZigZag::decode_avx(uint16_t *input_buffer, int16_t *output_buffer, size_t count)
{
    if (count < samples_per_parallel_chunk)
    {
        decode_scalar(input_buffer, output_buffer, count);
        return;
    }

    auto current_parallel_input_pointer = reinterpret_cast<__m256i *>(input_buffer);
    auto end_parallel_input_pointer = current_parallel_input_pointer + (count / samples_per_parallel_chunk);
    auto current_parallel_output_pointer = reinterpret_cast<__m256i *>(output_buffer);
    for (; current_parallel_input_pointer != end_parallel_input_pointer; current_parallel_input_pointer++, current_parallel_output_pointer++)
    {
        __m256i chunk = _mm256_loadu_si256(current_parallel_input_pointer);
        __m256i decoded_chunk = RiceMap::decode_single_sample(chunk);
        _mm256_storeu_si256(current_parallel_output_pointer, decoded_chunk);
    }

    for (size_t i = 0; i < (count % samples_per_parallel_chunk); i++)
    {
        uint16_t current_sample = *(reinterpret_cast<uint16_t*>(current_parallel_input_pointer) + i);
        int16_t decoded_sample = RiceMap::decode_single_sample(current_sample);
        *(reinterpret_cast<int16_t *>(current_parallel_output_pointer) + i) = decoded_sample;
    }

    Delta::decode<true>(output_buffer, count);
}

// TODO: split into encode_avx and encode_scalar into different classes
// TODO: this iteration code can be abstracted into a template function for code reuse.
//  We'll use this pattern a whole lot
// While the consumption of the input buffer can be abstracted, the writing to the output buffer is trickier
// This is because different writting patterns are exposed by different algorithms which might even write to separate buffers
// While this surely can be abstracted, doing so without taking a performance hit
// Which might or not be significant
// is not trivial

// TODO: replace _m256_i with _m256_i_u when appropriate
// TODO: enable special intrinsics compilation flags