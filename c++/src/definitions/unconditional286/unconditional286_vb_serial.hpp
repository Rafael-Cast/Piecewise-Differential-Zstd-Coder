#pragma once

#include "../attributes.hpp"
#include "../utils.hpp"
#include "immintrin.h"

#include <cstddef>
#include <cstdint>
#include "stddef.h"
#include "unconditional286_utils.hpp"
#include "../rice_map_sse.hpp"
#include "unconditional286_small_data_output_stream.hpp"
#include "unconditional286_medium_large_data_output_stream.hpp"
#include "unconditional286_read_stream_types.hpp"
#include "unconditional286_read_streams.hpp"

/*

 * Assumes 32 byte aligned memory, with 32 byte padding for all operations
 */
class Unconditional286VbSerial
{
public:
    template <bool AllowByteReorder = true>
    inline static void encode (
        const uint16_t * parallel_nanopore_restrict in_ptr,
        const std::size_t n,
        uint8_t * parallel_nanopore_restrict small_data_ptr,
        uint8_t * parallel_nanopore_restrict medium_data_ptr,
        uint8_t * parallel_nanopore_restrict large_data_ptr
        );

    template <bool AllowByteReorder = true>
    inline static void decode(
        const uint8_t *parallel_nanopore_restrict small_data_ptr,
        const uint8_t *parallel_nanopore_restrict medium_data_ptr,
        const uint8_t *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        uint16_t *out_ptr);
    //TODO: allocator
private:
    template<bool AllowByteReorder = true>
    inline static void encode_sse_section(
        const uint16_t *parallel_nanopore_restrict in_ptr,
        const size_t n,
        uint8_t *parallel_nanopore_restrict small_data_ptr,
        uint8_t *parallel_nanopore_restrict medium_data_ptr,
        uint8_t *parallel_nanopore_restrict large_data_ptr
    );

    template <bool AllowByteReorder = true>
    inline static void decode_sse_section(
        const uint8_t *parallel_nanopore_restrict small_data_ptr,
        const uint8_t *parallel_nanopore_restrict medium_data_ptr,
        const uint8_t *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        uint16_t *out_ptr);

    template <bool AllowByteReorder = true>
    static void encode_serial_section(
        const uint16_t * parallel_nanopore_restrict in_ptr,
        const size_t n,
        uint8_t * parallel_nanopore_restrict small_data_ptr,
        uint8_t * parallel_nanopore_restrict medium_data_ptr,
        uint8_t * parallel_nanopore_restrict large_data_ptr
        );

    template <bool AllowByteReorder = true>
    static void decode_serial_section(
        const uint8_t *parallel_nanopore_restrict small_data_ptr,
        const uint8_t *parallel_nanopore_restrict medium_data_ptr,
        const uint8_t *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        uint16_t *out_ptr);

    static constexpr uint_fast8_t medium_data_shift = 2;
    static constexpr uint_fast8_t large_data_shift = 10;
    static constexpr size_t samples_per_sse_register = 8;
};

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::encode(
    const uint16_t *parallel_nanopore_restrict in_ptr,
    const size_t n,
    uint8_t *parallel_nanopore_restrict small_data_ptr,
    uint8_t *parallel_nanopore_restrict medium_data_ptr,
    uint8_t *parallel_nanopore_restrict large_data_ptr)
{
    const size_t number_of_parallel_processed_samples = Unconditional286VbSizes::parallel_number_of_processed_samples(n);
    const auto parallel_nanopore_restrict serial_start_pointer = in_ptr + number_of_parallel_processed_samples;
    encode_sse_section<AllowByteReorder>(
        in_ptr,
        n,
        small_data_ptr,
        medium_data_ptr,
        large_data_ptr);

    encode_serial_section<AllowByteReorder>(
        serial_start_pointer,
        n,
        small_data_ptr + Unconditional286VbSizes::parallel_small_data_buffer_size_bytes(n),
        medium_data_ptr + Unconditional286VbSizes::parallel_medium_data_buffer_size_bytes(n),
        large_data_ptr + Unconditional286VbSizes::parallel_large_data_buffer_size_bytes(n));
}

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::decode(
    const uint8_t *parallel_nanopore_restrict small_data_ptr,
    const uint8_t *parallel_nanopore_restrict medium_data_ptr,
    const uint8_t *parallel_nanopore_restrict large_data_ptr,
    size_t n,
    uint16_t *out_ptr)
{
    const size_t number_of_parallel_processed_samples = Unconditional286VbSizes::parallel_number_of_processed_samples(n);
    decode_sse_section<AllowByteReorder>(
        small_data_ptr,
        medium_data_ptr,
        large_data_ptr,
        n,
        out_ptr);

    decode_serial_section<
        AllowByteReorder>(
        small_data_ptr + Unconditional286VbSizes::parallel_small_data_buffer_size_bytes(n),
        medium_data_ptr + Unconditional286VbSizes::parallel_medium_data_buffer_size_bytes(n),
        large_data_ptr + Unconditional286VbSizes::parallel_large_data_buffer_size_bytes(n),
        n,
        out_ptr + number_of_parallel_processed_samples
        );
}

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::encode_sse_section(
    const uint16_t *parallel_nanopore_restrict in_ptr,
    const std::size_t n,
    uint8_t *parallel_nanopore_restrict small_data_ptr,
    uint8_t *parallel_nanopore_restrict medium_data_ptr,
    uint8_t *parallel_nanopore_restrict large_data_ptr)
{
    const auto iterations = Unconditional286VbSizes::parallel_number_of_processed_samples(n) / 8;

    const uint16_t small_data_mask = 0x3;
    const uint16_t medium_data_mask = (((1 << 8) - 1) << medium_data_shift);
    const uint16_t large_data_mask = ((-1) << large_data_shift);

    Unconditional286SmallDataOutputStream small_data_output_stream(small_data_ptr);
    Unconditional286MediumLargeDataOutputStream medium_data_output_stream(medium_data_ptr);
    Unconditional286MediumLargeDataOutputStream large_data_output_stream(large_data_ptr);

    for (size_t i = 0; i < iterations; ++i)
    {
        const auto sample_0 = *in_ptr++;
        const auto sample_1 = *in_ptr++;
        const auto sample_2 = *in_ptr++;
        const auto sample_3 = *in_ptr++;
        const auto sample_4 = *in_ptr++;
        const auto sample_5 = *in_ptr++;
        const auto sample_6 = *in_ptr++;
        const auto sample_7 = *in_ptr++;

        const auto riced_sample_0 = RiceMapSSE::encode_single_sample(sample_0);
        const auto riced_sample_1 = RiceMapSSE::encode_single_sample(sample_1);
        const auto riced_sample_2 = RiceMapSSE::encode_single_sample(sample_2);
        const auto riced_sample_3 = RiceMapSSE::encode_single_sample(sample_3);
        const auto riced_sample_4 = RiceMapSSE::encode_single_sample(sample_4);
        const auto riced_sample_5 = RiceMapSSE::encode_single_sample(sample_5);
        const auto riced_sample_6 = RiceMapSSE::encode_single_sample(sample_6);
        const auto riced_sample_7 = RiceMapSSE::encode_single_sample(sample_7);

        const uint8_t medium_data_0 = (riced_sample_0 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_1 = (riced_sample_1 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_2 = (riced_sample_2 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_3 = (riced_sample_3 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_4 = (riced_sample_4 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_5 = (riced_sample_5 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_6 = (riced_sample_6 & medium_data_mask) >> medium_data_shift;
        const uint8_t medium_data_7 = (riced_sample_7 & medium_data_mask) >> medium_data_shift;

        medium_data_output_stream.write(
            medium_data_0,
            medium_data_1,
            medium_data_2,
            medium_data_3,
            medium_data_4,
            medium_data_5,
            medium_data_6,
            medium_data_7);

        const uint8_t large_data_0 = (riced_sample_0 & large_data_mask) >> 8;
        const uint8_t large_data_1 = (riced_sample_1 & large_data_mask) >> 8;
        const uint8_t large_data_2 = (riced_sample_2 & large_data_mask) >> 8;
        const uint8_t large_data_3 = (riced_sample_3 & large_data_mask) >> 8;
        const uint8_t large_data_4 = (riced_sample_4 & large_data_mask) >> 8;
        const uint8_t large_data_5 = (riced_sample_5 & large_data_mask) >> 8;
        const uint8_t large_data_6 = (riced_sample_6 & large_data_mask) >> 8;
        const uint8_t large_data_7 = (riced_sample_7 & large_data_mask) >> 8;

        large_data_output_stream.write(
            large_data_0,
            large_data_1,
            large_data_2,
            large_data_3,
            large_data_4,
            large_data_5,
            large_data_6,
            large_data_7);

        const uint8_t small_data_0 = riced_sample_0 & small_data_mask;
        const uint8_t small_data_1 = riced_sample_1 & small_data_mask;
        const uint8_t small_data_2 = riced_sample_2 & small_data_mask;
        const uint8_t small_data_3 = riced_sample_3 & small_data_mask;
        const uint8_t small_data_4 = riced_sample_4 & small_data_mask;
        const uint8_t small_data_5 = riced_sample_5 & small_data_mask;
        const uint8_t small_data_6 = riced_sample_6 & small_data_mask;
        const uint8_t small_data_7 = riced_sample_7 & small_data_mask;

        small_data_output_stream.write(
            small_data_0,
            small_data_1,
            small_data_2,
            small_data_3,
            small_data_4,
            small_data_5,
            small_data_6,
            small_data_7);
    }
}

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::decode_sse_section(
    const uint8_t *parallel_nanopore_restrict small_data_ptr,
    const uint8_t *parallel_nanopore_restrict medium_data_ptr,
    const uint8_t *parallel_nanopore_restrict large_data_ptr,
    size_t n,
    uint16_t *out_ptr)
{
    size_t iterations = Unconditional286VbSizes::parallel_number_of_processed_samples(n) / 8;
    Unconditional286SmallDataStream small_data_stream(small_data_ptr);
    Unconditional286MediumLargeDataStream medium_data_stream(medium_data_ptr);
    Unconditional286MediumLargeDataStream large_data_stream(large_data_ptr);

    for (size_t i = 0; i < iterations; ++i)
    {
        const auto small_data = small_data_stream.next();

        for (size_t j = 0; j < 8; ++j)
        {
            const auto medium_data = medium_data_stream.next() << medium_data_shift;
            const auto large_data = large_data_stream.next() << 8;
            const auto decoded_bytes = small_data.get(j) | medium_data | large_data;
            *out_ptr++ = RiceMapSSE::decode_single_sample(decoded_bytes);
        }
    }
}

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::encode_serial_section(
    const uint16_t *parallel_nanopore_restrict in_ptr,
    const size_t n,
    uint8_t *parallel_nanopore_restrict small_data_ptr,
    uint8_t *parallel_nanopore_restrict medium_data_ptr,
    uint8_t *parallel_nanopore_restrict large_data_ptr)
{
    constexpr uint16_t small_data_mask = 0x3;
    constexpr uint16_t medium_data_mask = ((1 << 8) - 1) << 2;
    constexpr uint16_t large_data_mask = 0xFC00;
    uint_fast8_t small_data_shift = 0;
    uint8_t small_data_buffer = 0;
    for (size_t i = 0; i < Unconditional286VbSizes::serial_number_of_processed_samples(n); i++)
    {
        const auto sample = in_ptr[i];
        const auto small_data = sample & small_data_mask;
        const auto medium_data = sample & medium_data_mask;
        const auto large_data = sample & large_data_mask;
        *medium_data_ptr = (medium_data >> 2);
        medium_data_ptr++;
        *large_data_ptr = (large_data >> 10);
        large_data_ptr++;
        small_data_buffer |= small_data << small_data_shift;
        small_data_shift += 2;
        if (small_data_shift == 8)
        {
            *small_data_ptr = small_data_buffer;
            small_data_ptr++;
            small_data_buffer = 0;
            small_data_shift = 0;
        }
    }
    if (small_data_shift != 0)
    {
        *small_data_ptr = small_data_buffer;
    }
}

template <bool AllowByteReorder>
inline void Unconditional286VbSerial::decode_serial_section(
    const uint8_t *parallel_nanopore_restrict small_data_ptr,
    const uint8_t *parallel_nanopore_restrict medium_data_ptr,
    const uint8_t *parallel_nanopore_restrict large_data_ptr,
    size_t n,
    uint16_t *out_ptr)
{
    constexpr uint8_t small_data_mask = 0x3;
    uint_fast8_t small_data_shift = 8;
    uint8_t small_data_buffer = 0;
    for (size_t i = 0; i < Unconditional286VbSizes::serial_number_of_processed_samples(n); i++)
    {
        const auto large_data = *(large_data_ptr++);
        const auto medium_data = *(medium_data_ptr++);
        if (small_data_shift == 8)
        {
            small_data_buffer = *(small_data_ptr++);
            small_data_shift = 0;
        }
        const auto small_data = static_cast<uint8_t>((small_data_buffer >> small_data_shift) & small_data_mask);
        small_data_shift += 2;
        const auto sample = static_cast<uint16_t>(static_cast<uint16_t>(small_data) | static_cast<uint16_t>((medium_data << 2)) | static_cast<uint16_t>((large_data << 10)));
        *(out_ptr++) = sample;
    }
}



/*
* TODO: use shifts to remove noisy part in order to quickly expand to other bit sizes
* TODO: in order to expand to other register widths, abstract away the encoding/decoding parallel code from the end ptr calculation
*    And move that logic to the parent (and coordinator) function encode/decode
*       Or make them return pointers to avoid abstraction leakage
*/