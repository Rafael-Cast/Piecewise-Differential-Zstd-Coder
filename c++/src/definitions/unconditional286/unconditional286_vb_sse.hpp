#pragma once

#include "../attributes.hpp"
#include "../utils.hpp"

#include "immintrin.h"

#include <cstddef>
#include <cstdint>
#include "unconditional286_utils.hpp"
#include "../rice_map_sse.hpp"

/*
 * Assumes 32 byte aligned memory, with 32 byte padding for all operations
 */
class Unconditional286VbSSE
{
public:
    template <
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    inline static void encode (
        const uint16_t * parallel_nanopore_restrict in_ptr,
        const size_t n,
        uint8_t * parallel_nanopore_restrict small_data_ptr,
        uint8_t * parallel_nanopore_restrict medium_data_ptr,
        uint8_t * parallel_nanopore_restrict large_data_ptr
        );

    template <
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    inline static void decode(
        const uint8_t *parallel_nanopore_restrict small_data_ptr,
        const uint8_t *parallel_nanopore_restrict medium_data_ptr,
        const uint8_t *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        uint16_t *out_ptr);
    //TODO: allocator
private:
    template<
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    inline static void encode_sse_section(
        const __m128i *parallel_nanopore_restrict in_ptr,
        const size_t n,
        __m128i *parallel_nanopore_restrict small_data_ptr,
        __m128i *parallel_nanopore_restrict medium_data_ptr,
        __m128i *parallel_nanopore_restrict large_data_ptr
    );

    template <
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    inline static void decode_sse_section(
        const __m128i *parallel_nanopore_restrict small_data_ptr,
        const __m128i *parallel_nanopore_restrict medium_data_ptr,
        const __m128i *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        __m128i *out_ptr);

    template <
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    static void encode_serial_section(
        const uint16_t * parallel_nanopore_restrict in_ptr,
        const size_t n,
        uint8_t * parallel_nanopore_restrict small_data_ptr,
        uint8_t * parallel_nanopore_restrict medium_data_ptr,
        uint8_t * parallel_nanopore_restrict large_data_ptr
        );

    template <
        bool AllowByteReorder = true,
        bool AssumeMemoryAlignment = true,
        bool DeltaEncode = false,
        bool RiceEncode = false>
    static void decode_serial_section(
        const uint8_t *parallel_nanopore_restrict small_data_ptr,
        const uint8_t *parallel_nanopore_restrict medium_data_ptr,
        const uint8_t *parallel_nanopore_restrict large_data_ptr,
        size_t n,
        uint16_t *out_ptr);

    template <bool AssumeMemoryAlignment = true>
    static inline void store(const __m128i* ptr, const __m128i & x)
    {
        _mm_storeu_si128(const_cast<__m128i*>(ptr), x);
    }

    template <bool AssumeMemoryAlignment = true>
    static inline __m128i load(const __m128i* ptr)
    {
        return _mm_loadu_si128(const_cast<__m128i *>(ptr));
    }
};

template <
    bool AllowByteReorder,
    bool AssumeMemoryAlignment,
    bool DeltaEncode,
    bool RiceEncode>
inline void Unconditional286VbSSE::encode(
    const uint16_t *parallel_nanopore_restrict in_ptr,
    const size_t n,
    uint8_t *parallel_nanopore_restrict small_data_ptr,
    uint8_t *parallel_nanopore_restrict medium_data_ptr,
    uint8_t *parallel_nanopore_restrict large_data_ptr)
{
    const size_t number_of_parallel_processed_samples = Unconditional286VbSizes::parallel_number_of_processed_samples(n);
    const auto parallel_nanopore_restrict serial_start_pointer = in_ptr + number_of_parallel_processed_samples;
    encode_sse_section<
        AllowByteReorder,
        AssumeMemoryAlignment,
        DeltaEncode,
        RiceEncode>(
        reinterpret_cast<const __m128i*>(in_ptr),
        n,
        reinterpret_cast<__m128i*>(small_data_ptr),
        reinterpret_cast<__m128i*>(medium_data_ptr),
        reinterpret_cast<__m128i*>(large_data_ptr));

    encode_serial_section<
        AllowByteReorder,
        AssumeMemoryAlignment,
        DeltaEncode,
        RiceEncode>(
        serial_start_pointer,
        n,
        small_data_ptr + Unconditional286VbSizes::parallel_small_data_buffer_size_bytes(n),
        medium_data_ptr + Unconditional286VbSizes::parallel_medium_data_buffer_size_bytes(n),
        large_data_ptr + Unconditional286VbSizes::parallel_large_data_buffer_size_bytes(n));
}

template <
    bool AllowByteReorder,
    bool AssumeMemoryAlignment,
    bool DeltaEncode,
    bool RiceEncode>
inline void Unconditional286VbSSE::decode(
    const uint8_t *parallel_nanopore_restrict small_data_ptr,
    const uint8_t *parallel_nanopore_restrict medium_data_ptr,
    const uint8_t *parallel_nanopore_restrict large_data_ptr,
    size_t n,
    uint16_t *out_ptr)
{
    const size_t number_of_parallel_processed_samples = Unconditional286VbSizes::parallel_number_of_processed_samples(n);
    decode_sse_section<
        AllowByteReorder,
        AssumeMemoryAlignment,
        DeltaEncode,
        RiceEncode>
    (
        reinterpret_cast<const __m128i*>(small_data_ptr),
        reinterpret_cast<const __m128i*>(medium_data_ptr),
        reinterpret_cast<const __m128i *>(large_data_ptr),
        n,
        reinterpret_cast<__m128i *>(out_ptr));

    decode_serial_section<
        AllowByteReorder,
        AssumeMemoryAlignment,
        DeltaEncode,
        RiceEncode>(
        small_data_ptr + Unconditional286VbSizes::parallel_small_data_buffer_size_bytes(n),
        medium_data_ptr + Unconditional286VbSizes::parallel_medium_data_buffer_size_bytes(n),
        large_data_ptr + Unconditional286VbSizes::parallel_large_data_buffer_size_bytes(n),
        n,
        out_ptr + number_of_parallel_processed_samples
        );
}

template <bool AllowByteReorder, bool AssumeMemoryAlignment, bool DeltaEncode, bool RiceEncode>
inline void Unconditional286VbSSE::encode_sse_section(
    const __m128i *parallel_nanopore_restrict in_ptr,
    const size_t n,
    __m128i *parallel_nanopore_restrict small_data_ptr,
    __m128i *parallel_nanopore_restrict medium_data_ptr,
    __m128i *parallel_nanopore_restrict large_data_ptr)
{
    // We process 8 128 bit registers at once
    const __m128i *end_ptr = in_ptr + 2 * (Unconditional286VbSizes::parallel_number_of_processed_samples(n) / 16);
    auto curr_ptr = in_ptr;

    constexpr uint_fast8_t medium_data_shift = 2;
    constexpr uint_fast8_t large_data_shift = 10;

    const __m128i small_data_mask = _mm_set1_epi16(0x3);
    const __m128i medium_data_mask = _mm_set1_epi16(((1 << 8) - 1) << medium_data_shift);
    const __m128i large_data_mask = _mm_set1_epi16((-1) << large_data_shift);

    for (; curr_ptr != end_ptr;)
    {
        //TODO: we might actually want to read up to 8 registers all at once...
        // TODO: isn't that a little bit excessive? In 32 bit mode that would automatically occupy all registers

        // TODO: reorder instructions to get better register pressure
        // and get a better use of the pipeline architecture
        const auto first_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto second_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto third_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto fourth_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto fifth_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto sixth_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto seventh_chunk = load<AssumeMemoryAlignment>(curr_ptr++);
        const auto eight_chunk = load<AssumeMemoryAlignment>(curr_ptr++);

        const auto first_chunk_riced = RiceMapSSE::encode_single_sample(first_chunk);
        const auto second_chunk_riced = RiceMapSSE::encode_single_sample(second_chunk);
        const auto third_chunk_riced = RiceMapSSE::encode_single_sample(third_chunk);
        const auto fourth_chunk_riced = RiceMapSSE::encode_single_sample(fourth_chunk);
        const auto fifth_chunk_riced = RiceMapSSE::encode_single_sample(fifth_chunk);
        const auto sixth_chunk_riced = RiceMapSSE::encode_single_sample(sixth_chunk);
        const auto seventh_chunk_riced = RiceMapSSE::encode_single_sample(seventh_chunk);
        const auto eight_chunk_riced = RiceMapSSE::encode_single_sample(eight_chunk);

        const auto first_chunk_small_data = _mm_and_si128(first_chunk_riced, small_data_mask);
        const auto second_chunk_small_data = _mm_and_si128(second_chunk_riced, small_data_mask);
        const auto third_chunk_small_data = _mm_and_si128(third_chunk_riced, small_data_mask);
        const auto fourth_chunk_small_data = _mm_and_si128(fourth_chunk_riced, small_data_mask);
        const auto fifth_chunk_small_data = _mm_and_si128(fifth_chunk_riced, small_data_mask);
        const auto sixth_chunk_small_data = _mm_and_si128(sixth_chunk_riced, small_data_mask);
        const auto seventh_chunk_small_data = _mm_and_si128(seventh_chunk_riced, small_data_mask);
        const auto eight_chunk_small_data = _mm_and_si128(eight_chunk_riced, small_data_mask);
        // TODO: writing here instead of at the end might be better performance-wise

        const auto first_chunk_medium_data = _mm_and_si128(first_chunk_riced, medium_data_mask);
        const auto second_chunk_medium_data = _mm_and_si128(second_chunk_riced, medium_data_mask);
        const auto third_chunk_medium_data = _mm_and_si128(third_chunk_riced, medium_data_mask);
        const auto fourth_chunk_medium_data = _mm_and_si128(fourth_chunk_riced, medium_data_mask);
        const auto fifth_chunk_medium_data = _mm_and_si128(fifth_chunk_riced, medium_data_mask);
        const auto sixth_chunk_medium_data = _mm_and_si128(sixth_chunk_riced, medium_data_mask);
        const auto seventh_chunk_medium_data = _mm_and_si128(seventh_chunk_riced, medium_data_mask);
        const auto eight_chunk_medium_data = _mm_and_si128(eight_chunk_riced, medium_data_mask);

        //TODO: we might be able to avoid some of all these ands
        const auto first_chunk_large_data = _mm_and_si128(first_chunk_riced, large_data_mask);
        const auto second_chunk_large_data = _mm_and_si128(second_chunk_riced, large_data_mask);
        const auto third_chunk_large_data = _mm_and_si128(third_chunk_riced, large_data_mask);
        const auto fourth_chunk_large_data = _mm_and_si128(fourth_chunk_riced, large_data_mask);
        const auto fifth_chunk_large_data = _mm_and_si128(fifth_chunk_riced, large_data_mask);
        const auto sixth_chunk_large_data = _mm_and_si128(sixth_chunk_riced, large_data_mask);
        const auto seventh_chunk_large_data = _mm_and_si128(seventh_chunk_riced, large_data_mask);
        const auto eight_chunk_large_data = _mm_and_si128(eight_chunk_riced, large_data_mask);

        //FIXME: I should srli second chunk rather thank slli first chunk
        // that implies that I also have to modify the decoding code.
        const auto first_quarter_packed_large_data = _mm_or_si128(
            second_chunk_large_data,
            _mm_srli_epi16(first_chunk_large_data, 8));

        const auto second_quarter_packed_large_data = _mm_or_si128(
            fourth_chunk_large_data,
            _mm_srli_epi16(third_chunk_large_data, 8));

        const auto third_quarter_packed_large_data = _mm_or_si128(
            sixth_chunk_large_data,
            _mm_srli_epi16(fifth_chunk_large_data, 8));

        const auto fourth_quarter_packed_large_data = _mm_or_si128(
            eight_chunk_large_data,
            _mm_srli_epi16(seventh_chunk_large_data, 8));

        const auto first_quarter_packed_medium_data = _mm_or_si128(
            _mm_slli_epi16(second_chunk_medium_data, 6),
            _mm_srli_epi16(first_chunk_medium_data, 2));

        const auto second_quarter_packed_medium_data = _mm_or_si128(
            _mm_slli_epi16(fourth_chunk_medium_data, 6),
            _mm_srli_epi16(third_chunk_medium_data, 2));

        const auto third_quarter_packed_medium_data = _mm_or_si128(
            _mm_slli_epi16(sixth_chunk_medium_data, 6),
            _mm_srli_epi16(fifth_chunk_medium_data, 2));//TODO: we can omit this srli by changing the coded, but that might damage compression

        const auto fourth_quarter_packed_medium_data = _mm_or_si128(
            _mm_slli_epi16(eight_chunk_medium_data, 6),
            _mm_srli_epi16(seventh_chunk_medium_data, 2));
        
        // TODO: pack 4 bit wide data => Just use slli_epi16...

        const auto packed_small_data =
            _mm_or_si128(
                _mm_or_si128(
                    _mm_or_si128(
                        _mm_or_si128(
                            _mm_or_si128(
                                _mm_or_si128(
                                    _mm_or_si128(
                                        _mm_slli_epi16(eight_chunk_small_data, 14),
                                        _mm_slli_epi16(seventh_chunk_small_data, 12)),
                                    _mm_slli_epi16(sixth_chunk_small_data, 10)),
                                _mm_slli_epi16(fifth_chunk_small_data, 8)),
                            _mm_slli_epi16(fourth_chunk_small_data, 6)),
                        _mm_slli_epi16(third_chunk_small_data, 4)),
                    _mm_slli_epi16(second_chunk_small_data, 2)),
                first_chunk_small_data);

        store<AssumeMemoryAlignment>(large_data_ptr++, first_quarter_packed_large_data);
        store<AssumeMemoryAlignment>(large_data_ptr++, second_quarter_packed_large_data);
        store<AssumeMemoryAlignment>(large_data_ptr++, third_quarter_packed_large_data);
        store<AssumeMemoryAlignment>(large_data_ptr++, fourth_quarter_packed_large_data);

        store<AssumeMemoryAlignment>(medium_data_ptr++, first_quarter_packed_medium_data);
        store<AssumeMemoryAlignment>(medium_data_ptr++, second_quarter_packed_medium_data);
        store<AssumeMemoryAlignment>(medium_data_ptr++, third_quarter_packed_medium_data);
        store<AssumeMemoryAlignment>(medium_data_ptr++, fourth_quarter_packed_medium_data);

        store<AssumeMemoryAlignment>(small_data_ptr++, packed_small_data);
    }
    if (AssumeMemoryAlignment)
    {
        _mm_mfence();
    }
}

template <bool AllowByteReorder, bool AssumeMemoryAlignment, bool DeltaEncode, bool RiceEncode>
inline void Unconditional286VbSSE::decode_sse_section(
    const __m128i *parallel_nanopore_restrict small_data_ptr,
    const __m128i *parallel_nanopore_restrict medium_data_ptr,
    const __m128i *parallel_nanopore_restrict large_data_ptr,
    size_t n,
    __m128i *out_ptr)
{

    const auto small_data_extract_mask = _mm_set1_epi16(0x3);
    const auto low_byte_extract_mask = _mm_set1_epi16(0xff);
    const auto high_byte_extract_mask = _mm_set1_epi16(0xff00);

    size_t remaining_iterations = 2 * (Unconditional286VbSizes::parallel_number_of_processed_samples(n) / (8 * 16)); // 8 * 16 samples per iterations
    for (; remaining_iterations != 0; remaining_iterations--)
    {
        const auto small_data = load<AssumeMemoryAlignment>(small_data_ptr++);
        const auto first_quarter_large_data = load<AssumeMemoryAlignment>(large_data_ptr++);
        const auto second_quarter_large_data = load<AssumeMemoryAlignment>(large_data_ptr++);
        const auto third_quarter_large_data = load<AssumeMemoryAlignment>(large_data_ptr++);
        const auto fourth_quarter_large_data = load<AssumeMemoryAlignment>(large_data_ptr++);
        const auto first_quarter_medium_data = load<AssumeMemoryAlignment>(medium_data_ptr++);
        const auto second_quarter_medium_data = load<AssumeMemoryAlignment>(medium_data_ptr++);
        const auto third_quarter_medium_data = load<AssumeMemoryAlignment>(medium_data_ptr++);
        const auto fourth_quarter_medium_data = load<AssumeMemoryAlignment>(medium_data_ptr++);

        // TODO: we should use 'and' masks rather than shifting to introduce 0's
        // They have a higher throughput
        // on the other hand, that might need us to use the low_byte_extract_mask and high_byte_extract_mask
        // and thus use 2 more registers
        const auto first_data_small_data = _mm_and_si128(small_data, small_data_extract_mask);
        const auto first_data_medium_data = _mm_slli_epi16(_mm_and_si128(first_quarter_medium_data, low_byte_extract_mask), 2);
        const auto first_data_large_data = _mm_slli_epi16(_mm_and_si128(first_quarter_large_data, low_byte_extract_mask), 8); //TODO: we might be able to omit these ands, as we are already shifting the values one byte
        const auto first_data_chunk = _mm_or_si128(first_data_small_data, _mm_or_si128(first_data_medium_data, first_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(first_data_chunk));

        const auto second_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 2), small_data_extract_mask);
        const auto second_data_medium_data = _mm_srli_epi16(_mm_and_si128(first_quarter_medium_data, high_byte_extract_mask), 6);
        const auto second_data_large_data = _mm_and_si128(first_quarter_large_data, high_byte_extract_mask);
        const auto second_data_chunk = _mm_or_si128(second_data_small_data, _mm_or_si128(second_data_medium_data, second_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(second_data_chunk));

        const auto third_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 4), small_data_extract_mask);
        const auto third_data_medium_data = _mm_slli_epi16(_mm_and_si128(second_quarter_medium_data, low_byte_extract_mask), 2);
        const auto third_data_large_data = _mm_slli_epi16(_mm_and_si128(second_quarter_large_data, low_byte_extract_mask), 8); //TODO: we might be able to omit these ands, as we are already shifting the values one byte
        const auto third_data_chunk = _mm_or_si128(third_data_small_data, _mm_or_si128(third_data_medium_data, third_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(third_data_chunk));

        const auto fourth_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 6), small_data_extract_mask);
        const auto fourth_data_medium_data = _mm_srli_epi16(_mm_and_si128(second_quarter_medium_data, high_byte_extract_mask), 6);
        const auto fourth_data_large_data = _mm_and_si128(second_quarter_large_data, high_byte_extract_mask);
        const auto fourth_data_chunk = _mm_or_si128(fourth_data_small_data, _mm_or_si128(fourth_data_medium_data, fourth_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(fourth_data_chunk));

        const auto fifth_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 8), small_data_extract_mask);
        const auto fifth_data_medium_data = _mm_slli_epi16(_mm_and_si128(third_quarter_medium_data, low_byte_extract_mask), 2);
        const auto fifth_data_large_data = _mm_slli_epi16(_mm_and_si128(third_quarter_large_data, low_byte_extract_mask), 8); //TODO: we might be able to omit these ands, as we are already shifting the values one byte
        const auto fifth_data_chunk = _mm_or_si128(fifth_data_small_data, _mm_or_si128(fifth_data_medium_data, fifth_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(fifth_data_chunk));

        const auto sixth_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 10), small_data_extract_mask);
        const auto sixth_data_medium_data = _mm_srli_epi16(_mm_and_si128(third_quarter_medium_data, high_byte_extract_mask), 6);
        const auto sixth_data_large_data = _mm_and_si128(third_quarter_large_data, high_byte_extract_mask);
        const auto sixth_data_chunk = _mm_or_si128(sixth_data_small_data, _mm_or_si128(sixth_data_medium_data, sixth_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(sixth_data_chunk));
        //TODO: FIXME: reorder small data
        const auto seventh_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 12), small_data_extract_mask);
        const auto seventh_data_medium_data = _mm_slli_epi16(_mm_and_si128(fourth_quarter_medium_data, low_byte_extract_mask), 2);
        const auto seventh_data_large_data = _mm_slli_epi16(_mm_and_si128(fourth_quarter_large_data, low_byte_extract_mask), 8); //TODO: we might be able to omit these ands, as we are already shifting the values one byte
        const auto seventh_data_chunk = _mm_or_si128(seventh_data_small_data, _mm_or_si128(seventh_data_medium_data, seventh_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(seventh_data_chunk));

        const auto eight_data_small_data = _mm_and_si128(_mm_srli_epi16(small_data, 14), small_data_extract_mask);
        const auto eight_data_medium_data = _mm_srli_epi16(_mm_and_si128(fourth_quarter_medium_data, high_byte_extract_mask), 6);
        const auto eight_data_large_data = _mm_and_si128(fourth_quarter_large_data, high_byte_extract_mask);
        const auto eight_data_chunk = _mm_or_si128(eight_data_small_data, _mm_or_si128(eight_data_medium_data, eight_data_large_data));
        store<AssumeMemoryAlignment>(out_ptr++, RiceMapSSE::decode_single_sample(eight_data_chunk));
    }
    if (AssumeMemoryAlignment)
    {
        _mm_mfence();
    }
}

template <bool AllowByteReorder, bool AssumeMemoryAlignment, bool DeltaEncode, bool RiceEncode>
inline void Unconditional286VbSSE::encode_serial_section(const uint16_t *parallel_nanopore_restrict in_ptr, const size_t n, uint8_t *parallel_nanopore_restrict small_data_ptr, uint8_t *parallel_nanopore_restrict medium_data_ptr, uint8_t *parallel_nanopore_restrict large_data_ptr)
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

template <bool AllowByteReorder, bool AssumeMemoryAlignment, bool DeltaEncode, bool RiceEncode>
inline void Unconditional286VbSSE::decode_serial_section(
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