#pragma once

#include "attributes.hpp"
#include "delta_zig_zag.hpp"
#include "unconditional_2_8_6_vb.hpp"
#include "memory_utils.hpp"
#include "utils.hpp"
#include "codecs.hpp"

#include "zstd.h"

#include <cstddef>
#include <cstdint>
#include <string>

class Unconditional286CompressorHeader
{
public:
    Unconditional286CompressorHeader(
        const uint64_t number_of_samples,
        const uint64_t first_byte_data_compressed_size,
        const uint64_t second_byte_data_compressed_size) : 
        m_n(number_of_samples),
        m_first_byte_compressed_size(first_byte_data_compressed_size),
        m_second_byte_compressed_size(second_byte_data_compressed_size)
        {}

    void encode(uint8_t *ptr) const noexcept
    {
        encode_uint64(m_n, ptr);
        encode_uint64(m_first_byte_compressed_size, ptr + sizeof(m_n));
        encode_uint64(m_second_byte_compressed_size, ptr + sizeof(m_n) + sizeof(m_first_byte_compressed_size));
    }
    static Unconditional286CompressorHeader decode(const uint8_t *ptr) noexcept
    {
        const auto n = decode_uint64(ptr);
        const auto first_byte_compressed_size = decode_uint64(ptr + sizeof(uint64_t));
        const auto second_byte_compressed_size = decode_uint64(ptr + 2 * sizeof(uint64_t));
        return Unconditional286CompressorHeader(n, first_byte_compressed_size, second_byte_compressed_size);
    }
    static constexpr size_t size() noexcept
    {
        return sizeof(m_n) + sizeof(m_first_byte_compressed_size) + sizeof(m_second_byte_compressed_size);
    }

    uint64_t number_of_samples() const noexcept
    {
        return m_n;
    }
    uint64_t first_bytes_data_compressed_size() const noexcept { return m_first_byte_compressed_size; }
    uint64_t second_bytes_data_compressed_size() const noexcept { return m_second_byte_compressed_size; }

private:
    const uint64_t m_n;
    const uint64_t m_first_byte_compressed_size;
    const uint64_t m_second_byte_compressed_size;
};

//class Unconditional286Buffer
//{
//public:
//
//    Unconditional286Compressor(size_t n)
//    {
//
//    }
//private:
//    uint8_t * noise_data = nullptr;
//    uint8_t * first_byte = nullptr;
//    uint8_t *second_byte = nullptr;
//};

class Unconditional286Compressor
{
public:
    template<bool AllowReorder = true,
    bool DeltaEncodeInVb = false,
    bool RiceEncodeInVb = false,
    bool AssumeMemoryAlignment = false,
    size_t NoiseBits = 2,
    int ZSTDCompressionLevel = 1,
    SIMDAcceleration accelerator
    >
    static uint64_t encode(
        const int16_t parallel_nanopore_restrict *samples,
        const size_t number_of_samples,
        uint8_t parallel_nanopore_restrict *output);

    template<bool AllowReorder = true,
    bool DeltaEncodeInVb = false,
    bool RiceEncodeInVb = false,
    bool AssumeMemoryAlignment = false,
    size_t NoiseBits = 2,
    int ZSTDCompressionLevel = 1,
    SIMDAcceleration accelerator
    >
    static void decode(
        const uint8_t parallel_nanopore_restrict *bytes,
        int16_t parallel_nanopore_restrict *output);

    static size_t encode_bound(size_t number_of_samples);
    static Unconditional286CompressorHeader get_header(uint8_t * ptr)
    {
        return Unconditional286CompressorHeader::decode(ptr);
    }

private:
    static void unconditional286compressor_cleanup_routine(
        uint8_t* const noise_data_ptr,
        uint8_t* const first_byte_data_ptr,
        uint8_t* const second_byte_data_ptr)
    {
        padded_aligned_free(noise_data_ptr);
        padded_aligned_free(first_byte_data_ptr);
        padded_aligned_free(second_byte_data_ptr);
    }
};

//TODO: FIXME: use NoiseBits

template <
    bool AllowReorder,
    bool DeltaEncodeInVb,
    bool RiceEncodeInVb,
    bool AssumeMemoryAlignment,
    size_t NoiseBits,
    int ZSTDCompressionLevel,
    SIMDAcceleration accelerator
>
inline uint64_t Unconditional286Compressor::encode(
    const int16_t parallel_nanopore_restrict *samples,
    const size_t number_of_samples,
    uint8_t parallel_nanopore_restrict *output)
{
    uint8_t *const noise_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples), 32);
    uint8_t *const first_byte_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::medium_data_buffer_size_bytes(number_of_samples), 32);
    uint8_t *const second_byte_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::large_data_buffer_size_bytes(number_of_samples), 32);

    const auto vb_input_samples = padded_aligned_malloc_t<uint16_t>(number_of_samples, 32);

    uint_fast16_t previous_sample = 0;
    for (size_t i = 0; i < number_of_samples; i++)
    {
        const auto current_sample = samples[i];
        const auto delta = current_sample - previous_sample;
        vb_input_samples[i] = delta;
        previous_sample = current_sample;
    }

    Unconditional286Vb::encode<
        AllowReorder,
        AssumeMemoryAlignment,
        DeltaEncodeInVb,
        RiceEncodeInVb,
        accelerator>(
        reinterpret_cast<const uint16_t *>(vb_input_samples),
        number_of_samples,
        noise_data_ptr,
        first_byte_data_ptr,
        second_byte_data_ptr);

    padded_aligned_free(vb_input_samples);

    //TODO: we assume that the buffer is big enough..
    const auto compression_bound_bytes = encode_bound(number_of_samples);

    //TODO: we can avoid this copy if we write the vb directly to output (might want to experiment with memory alignment)
    memcpy(output + Unconditional286CompressorHeader::size(), noise_data_ptr, Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples));

    const auto first_byte_compression_result = ZSTD_compress(
        output + Unconditional286CompressorHeader::size() + Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples),
        compression_bound_bytes - Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples),
        first_byte_data_ptr,
        Unconditional286VbSizes::medium_data_buffer_size_bytes(number_of_samples),
        ZSTDCompressionLevel);


    // TODO: USE ERROR MONAD
    //TODO: different ZSTD Compression levels for different parts

    if (ZSTD_isError(first_byte_compression_result))
    {
        unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
        return -1;
    }

    const auto second_byte_compression_result = ZSTD_compress(
        output + Unconditional286CompressorHeader::size() + Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples) + first_byte_compression_result,
        compression_bound_bytes - Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples) - first_byte_compression_result,
        second_byte_data_ptr,
        Unconditional286VbSizes::large_data_buffer_size_bytes(number_of_samples),
        ZSTDCompressionLevel);

    if (ZSTD_isError(second_byte_compression_result))
    {
        unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
        return -1;
    }

    Unconditional286CompressorHeader hdr(number_of_samples, first_byte_compression_result, second_byte_compression_result);
    hdr.encode(output);

    unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
    return
        second_byte_compression_result +
        first_byte_compression_result +
        Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples) +
        Unconditional286CompressorHeader::size();
}

template<bool AllowReorder,
    bool DeltaEncodeInVb,
    bool RiceEncodeInVb,
    bool AssumeMemoryAlignment,
    size_t NoiseBits,
    int ZSTDCompressionLevel,
    SIMDAcceleration accelerator
    >
inline void Unconditional286Compressor::decode(
    const uint8_t parallel_nanopore_restrict *bytes,
    int16_t parallel_nanopore_restrict *output)
{
    const auto hdr = Unconditional286CompressorHeader::decode(bytes);
    const auto number_of_samples = hdr.number_of_samples();

    uint8_t parallel_nanopore_restrict * noise_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples), 32);
    uint8_t parallel_nanopore_restrict * first_byte_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::medium_data_buffer_size_bytes(number_of_samples), 32);
    uint8_t parallel_nanopore_restrict * second_byte_data_ptr = padded_aligned_malloc(Unconditional286VbSizes::large_data_buffer_size_bytes(number_of_samples), 32);
//FIXME: delta encode and rice encode!!!
    //TODO: we can also avoid this memcpy

    memcpy(
        noise_data_ptr,
        bytes + Unconditional286CompressorHeader::size(),
        Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples)
    );

    //TODO: reading the ZSTD frame content size, (and rearranging it so the first buffer is a ZSTD compressed buffer), we can avoid storing the number of samples in the header and infer in from the decompressed size
    const auto first_byte_decompression_result = ZSTD_decompress(
        first_byte_data_ptr,
        Unconditional286VbSizes::medium_data_buffer_size_bytes(number_of_samples),
        bytes + Unconditional286CompressorHeader::size() + Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples),
        hdr.first_bytes_data_compressed_size());
    
    if (ZSTD_isError(first_byte_decompression_result))
    {
        unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
        return;
    }

    const auto second_byte_decompression_result = ZSTD_decompress(
        second_byte_data_ptr,
        Unconditional286VbSizes::large_data_buffer_size_bytes(number_of_samples),
        bytes + Unconditional286CompressorHeader::size() + Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples) + hdr.first_bytes_data_compressed_size(),
        hdr.second_bytes_data_compressed_size());
    
    if (ZSTD_isError(second_byte_decompression_result))
    {
        unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
        return;
    }

    const auto vb_output_samples = padded_aligned_malloc_t<uint16_t>(number_of_samples, 32);

    Unconditional286Vb::decode
    <AllowReorder,
    AssumeMemoryAlignment,
    DeltaEncodeInVb,
    RiceEncodeInVb,
    accelerator>(
        noise_data_ptr,
        first_byte_data_ptr,
        second_byte_data_ptr,
        hdr.number_of_samples(),
        vb_output_samples);

    uint_fast16_t previous_sample = 0;
    for (size_t i = 0; i < number_of_samples; i++)
    {
        const auto current_delta = vb_output_samples[i];
        const auto decoded_sample = current_delta + previous_sample;
        output[i] = decoded_sample;
        previous_sample = decoded_sample;
    }

    padded_aligned_free(vb_output_samples);

    unconditional286compressor_cleanup_routine(noise_data_ptr, first_byte_data_ptr, second_byte_data_ptr);
    return;
}

size_t Unconditional286Compressor::encode_bound(size_t number_of_samples)
{
    const auto small_data_bytes = Unconditional286VbSizes::small_data_buffer_size_bytes(number_of_samples);
    const auto uncompressed_medium_data_bytes = Unconditional286VbSizes::medium_data_buffer_size_bytes(number_of_samples);
    const auto uncompressed_large_data_bytes = Unconditional286VbSizes::large_data_buffer_size_bytes(number_of_samples);
    const auto medium_data_bytes = ZSTD_compressBound(uncompressed_medium_data_bytes);
    const auto large_data_bytes = ZSTD_compressBound(uncompressed_large_data_bytes);
    return Unconditional286CompressorHeader::size() + small_data_bytes + medium_data_bytes + large_data_bytes;
}
