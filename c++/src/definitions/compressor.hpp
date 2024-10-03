#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include "attributes.hpp"

class CompressorV1
{
public:
    virtual uint64_t encode(int16_t *samples, size_t number_of_samples, uint8_t * output_pointer) = 0;
    virtual int16_t *decode(uint8_t *bytes, size_t number_of_samples) = 0;//TODO: fix decode function signature
    /*
    * The decompression shouldn't need to be passed the number of samples
    * Also, the decompression shouldn' allocate memory
    * To finalize, being able to query the number of samples from the header
    *   (in order to properly reserve the memory for the compressor)
    *   should be a compressor's interface method
    * 
    * We might also want to add (auto implemented using the current encode and decode methods)
    *   methods which automatically reserve the memory, for the client's convenience
    * 
    * We should also do the same for spans, but also providing default implementations converting
    *   between spans and raw ptrs + count
    */
    virtual uint64_t encode_bound(size_t number_of_sample) = 0;

    virtual ~CompressorV1() = default;
};

class CompressorV2
{
public:
    virtual uint64_t encode(const int16_t * parallel_nanopore_restrict samples, size_t number_of_samples, uint8_t * parallel_nanopore_restrict output_pointer) const = 0;
    virtual void decode(const uint8_t * parallel_nanopore_restrict bytes, int16_t * parallel_nanopore_restrict output_pointer, size_t input_size) const = 0;
    virtual uint64_t encode_bound(size_t number_of_samples) const noexcept = 0;
    virtual uint64_t get_number_of_samples(const uint8_t *bytes) const noexcept = 0;

    ///*
    //* This method should be overridden for compressors with specific alignment and padding constraints
    //*/
    //virtual std::unique_ptr<uint8_t[]> encode_and_alloc(const int16_t *samples, size_t number_of_samples)
    //{
    //    const auto max_compressed_size = encode_bound(number_of_samples);
    //    auto output = std::make_unique<uint8_t[]>(max_compressed_size);
    //    encode(samples, number_of_samples, output.get());
    //    return output;
    //}
    ////TODO: refactor this, so an allocator can be retrieved from the compressor and that allocator is used
    //// As such, specific compressors will need to implement the allocator-getter function
    ///*
    //* This method should be overridden for compressors with specific alignment and padding constraints
    //*/
    //virtual std::unique_ptr<int16_t[]> decode_and_alloc(const uint8_t * bytes)
    //{
    //    const auto number_of_samples = get_number_of_samples(bytes);
    //    auto output = std::make_unique<int16_t[]>(number_of_samples);
    //    decode(bytes, output.get());
    //    return output;
    //}
};

typedef CompressorV2 Compressor;