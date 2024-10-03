#pragma once

#include <gtest/gtest.h>
#include "../src/definitions/unconditional286_compressor.hpp"
#include "../src/definitions/memory_utils.hpp"

template<SIMDAcceleration accelerator>
void run_compression_is_reversible_test(const std::vector<int16_t>& xs)
{
    ASSERT_NE(xs.data(), nullptr);

    const auto compressed_buffer = padded_aligned_malloc(Unconditional286Compressor::encode_bound(xs.size()), 32);
    const auto decompressed_buffer = padded_aligned_malloc_t<int16_t>(xs.size(), 32);
    
    ASSERT_NE(compressed_buffer, nullptr);
    ASSERT_NE(decompressed_buffer, nullptr);

    const auto number_of_compressed_bytes = Unconditional286Compressor::encode<true, false, false, false, 2, 1, accelerator>(xs.data(), xs.size(), compressed_buffer);
    const auto hdr = Unconditional286Compressor::get_header(compressed_buffer);
    Unconditional286Compressor::decode<true, false, false, false, 2, 1, accelerator>(compressed_buffer, decompressed_buffer);

    ASSERT_NE(number_of_compressed_bytes, -1);
    EXPECT_EQ(hdr.number_of_samples(), xs.size());
    for (size_t i = 0; i < xs.size(); i++)
    {
        ASSERT_EQ(xs[i], decompressed_buffer[i]);
        if (testing::Test::HasFailure())
        {
            break;
        }
    }
}

TEST(Unconditional286CompressorTests, CompressionIsReversibleForHomogeneousInput)
{
    const std::vector<int16_t> xs{0, 1, 0xFF, -1, static_cast<int16_t>(0xFF00)};
    const std::vector<size_t> ns{1, 10, 128, 256, 300};
    const auto pairs = product_vect(xs, ns);
    for (const auto [x, n] : pairs)
    {
        const auto input_data = repeat_vect<int16_t>(n, x);
        run_compression_is_reversible_test<SIMDAcceleration::AVX2>(input_data);
        if (testing::Test::HasFailure())
        {
            break;
        }
        run_compression_is_reversible_test<SIMDAcceleration::SSE>(input_data);
        if (testing::Test::HasFailure())
        {
            break;
        }
    }
}

TEST(Unconditional286CompressorTests, CompressionIsReversibleForRandomData)
{
    const std::vector<int> seeds{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    for (const auto seed : seeds)
    {
        const auto input_data_ptr = build_random_vector(seed);
        const auto input_data = *input_data_ptr;
        run_compression_is_reversible_test<SIMDAcceleration::AVX2>(input_data);
        if (testing::Test::HasFailure())
        {
            break;
        }
        run_compression_is_reversible_test<SIMDAcceleration::SSE>(input_data);
        if (testing::Test::HasFailure())
        {
            break;
        }
    }
}

TEST(Unconditional286CompressorTests, CompressionAndDecompressionShouldSucceedForEmptyInput)
{
    //FIXME: not actually supported right now
    run_compression_is_reversible_test<SIMDAcceleration::AVX2>(repeat_vect<int16_t>(0, 0xFFFF));
    run_compression_is_reversible_test<SIMDAcceleration::SSE>(repeat_vect<int16_t>(0, 0xFFFF));
}