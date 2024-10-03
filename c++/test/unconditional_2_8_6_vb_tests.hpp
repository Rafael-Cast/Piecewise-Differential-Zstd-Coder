#pragma once

#include "gtest/gtest.h"

#include "../src/definitions/unconditional_2_8_6_vb.hpp"
#include "../src/definitions/memory_utils.hpp"
#include "../src/definitions/simd_acceleration.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <utils.hpp>
#include "immintrin.h"

#define RAW_VB_TESTS_TEMPLATE_PARAMETERS true, false, false, false

template<SIMDAcceleration accelerator>
void run_buffer_analysis_test(const size_t n, const uint16_t x)
{
    const auto input_vector = repeat_vect<uint16_t>(n, x);

    //TODO: write an allocator and an overload for a struct of ptrs
    const auto input_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);
    const auto small_data_buffer = padded_aligned_malloc_t<uint8_t>(round_up_division<size_t>(n, 4), 128);
    const auto medium_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto large_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto decoded_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);

    memcpy(input_buffer, input_vector.data(), n * sizeof(uint16_t));

    Unconditional286Vb vb;

    vb.encode<RAW_VB_TESTS_TEMPLATE_PARAMETERS,
               accelerator>(
            input_buffer,
            n,
            small_data_buffer,
            medium_data_buffer,
            large_data_buffer);

    vb.decode<RAW_VB_TESTS_TEMPLATE_PARAMETERS,
            accelerator>(
        small_data_buffer,
        medium_data_buffer,
        large_data_buffer,
        n,
        decoded_buffer);

    const auto parallel_samples_end_i = nearest_multiple_of_rounded_down<size_t>(n, 128);

    std::string sb = "For test case with x = " + std::to_string(x) + " and n = " + std::to_string(n) + '\n';

    for (size_t i = 0; i < parallel_samples_end_i; i++)
    {
        EXPECT_EQ(input_buffer[i], decoded_buffer[i]);
        if (testing::Test::HasFailure())
        {
            sb = sb + "First decoding failure at index " + std::to_string(i) + " with value: " + std::to_string(decoded_buffer[i]) + " (expected: " + std::to_string(input_buffer[i]) + ")\n";
            break;
        }
    }

    if (testing::Test::HasFailure())
    {
        for (size_t i = 0; i < parallel_samples_end_i; i++)
        {
            const auto small_data_value = ((small_data_buffer[i / 4] >> (2 * (i % 4))) & 0x3);
            if ((input_buffer[i] & 0x3) != small_data_value)
            {
                sb = sb + "Small data failed at index: " + std::to_string(i) + " with value: " + std::to_string(small_data_value) + " (expected: " + std::to_string(input_buffer[i] & 0x3) + ")\n";
                break;
            }
        }

        // FIXME: the indexes are not quite correct but as the tests are for repeat n x it will works

        for (size_t i = 0; i < parallel_samples_end_i; i++)
        {
            const auto medium_data_value = medium_data_buffer[i];
            const auto medium_data_expected = ((input_buffer[i] & (0xFF << 2)) >> 2);
            if (medium_data_expected != medium_data_value)
            {
                sb = sb + "Medium data failed at index: " + std::to_string(i) + " with value: " + std::to_string(medium_data_value) + " (expected: " + std::to_string(medium_data_expected) + ")\n";
                break;
            }
        }

        for (size_t i = 0; i < parallel_samples_end_i; i++)
        {
            const auto large_data_value = large_data_buffer[i];
            //FIXME: expected value is incorrect => Or what actually is incorrect is the case of -1 (ie. something greater than the digitization range)?
            const auto large_data_expected = ((input_buffer[i] & (0xFF << 10)) >> 8);
            if (large_data_expected != large_data_value)
            {
                sb = sb + "Large data failed at index: " + std::to_string(i) + " with value: " + std::to_string(large_data_value) + " (expected: " + std::to_string(large_data_expected) + ")\n";
                break;
            }
        }
    }

    padded_aligned_free(input_buffer);
    padded_aligned_free(small_data_buffer);
    padded_aligned_free(medium_data_buffer);
    padded_aligned_free(large_data_buffer);
    padded_aligned_free(decoded_buffer);

    if (testing::Test::HasFailure())
    {
        FAIL() << sb;
    }
}

template<SIMDAcceleration accelerator>
void run_simple_equality_tests(const std::vector<uint16_t> &input_vector)
{
    const auto n = input_vector.size();
    const auto input_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);
    const auto small_data_buffer = padded_aligned_malloc_t<uint8_t>(round_up_division<size_t>(n, 4), 128);
    const auto medium_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto large_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto decoded_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);

    memcpy(input_buffer, input_vector.data(), n * sizeof(uint16_t));

    Unconditional286Vb vb;

    vb.encode<RAW_VB_TESTS_TEMPLATE_PARAMETERS,
           accelerator>(
            input_buffer,
            n,
            small_data_buffer,
            medium_data_buffer,
            large_data_buffer);

    vb.decode<RAW_VB_TESTS_TEMPLATE_PARAMETERS,
        accelerator>(
        small_data_buffer,
        medium_data_buffer,
        large_data_buffer,
        n,
        decoded_buffer);
    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(input_buffer[i], decoded_buffer[i]) << i;
        if (testing::Test::HasFailure())
        {
            break;
        }
    }
}

const std::vector<uint16_t> default_xs_values{0, 1, 0xFFFF, 0xFF, 0xFF00};
const std::vector<uint16_t> debugging_xs_values{38852, 4635};
const std::vector<size_t> debugging_ns_values{1, 128, 256, 575473};
const std::vector<SIMDAcceleration> default_accelerators{SIMDAcceleration::SSE, SIMDAcceleration::AVX2};

TEST(Unconditional286VbTests, RawVbParallelSectionBufferAnalysis)
{
    const std::vector<size_t> ns = {1, 128, 256};
    const std::vector<uint16_t> xs = default_xs_values;
    const auto cases = product_vect(ns, xs);


    for (const auto [n, x] : cases)
    {
        run_buffer_analysis_test<SIMDAcceleration::AVX2>(n, x);
        if (HasFailure())
        {
            break;
        }
        run_buffer_analysis_test<SIMDAcceleration::SSE>(n, x);
        if (HasFailure())
        {
            break;
        }
    }
};

TEST(Unconditional286VbTests, RawVbSerialSectionBufferAnalysis)
{
    const std::vector<size_t> ns = nats_vect<size_t>(128);
    const std::vector<uint16_t> xs = default_xs_values;
    const auto cases = product_vect(ns, xs);

    for (const auto [n, x] : cases)
    {
        run_buffer_analysis_test<SIMDAcceleration::AVX2>(n, x);
        run_buffer_analysis_test<SIMDAcceleration::SSE>(n, x);
    }
}

TEST(Unconditional286VbTests, RawVbCompoundBufferAnalysis)
{
    const std::vector<size_t> ns = {130, 260, 300, 700, 1000, 192};
    const std::vector<uint16_t> xs = default_xs_values;
    const auto cases = product_vect(ns, xs);
    for (const auto [n, x] : cases)
    {
        run_buffer_analysis_test<SIMDAcceleration::AVX2>(n, x);
        run_buffer_analysis_test<SIMDAcceleration::SSE>(n, x);
    }
}

TEST(Unconditional286VbTests, RawVbDebugBufferAnalysis)
{
    for (const auto [n, x] : product_vect(debugging_ns_values, debugging_xs_values))
    {
        run_buffer_analysis_test<SIMDAcceleration::AVX2>(n, x);
        run_buffer_analysis_test<SIMDAcceleration::SSE>(n, x);
    }
}

TEST(Unconditional286VbTests, RawVbDebugBufferAnalysis2)
{
    for (const auto n : debugging_ns_values)
    {
        std::vector<uint16_t> xs;
        for (size_t i = 0; i < n; i++)
        {
            if ((i % 2) == 0)
            {
                xs.push_back(0xFFFF);
            }
            else
            {
                xs.push_back(0);
            }
        }
    }
}

TEST(Unconditional286VbTests, RawAvxVbDebugEqualityTest)
{
    const auto reg_xs = _mm256_set_epi8(
        31 << 2, 30 << 2, 29 << 2, 28 << 2, 27 << 2, 26 << 2,
        25  << 2, 24  << 2, 23  << 2, 22  << 2, 21  << 2,
        20  << 2, 19  << 2, 18  << 2, 17  << 2, 16  << 2,
        15  << 2, 14  << 2, 13  << 2, 12  << 2, 11  << 2,
        10  << 2,  9  << 2,  8  << 2,  7  << 2,  6  << 2,
        5  << 2,  4  << 2,  3  << 2,  2  << 2,  1  << 2,
        0  << 2
    );

    std::vector<uint16_t> xs;
    xs.reserve(16);
    _mm256_storeu_si256((__m256i*)xs.data(), reg_xs);

    const auto input_vector = xs;
    const auto n = input_vector.size();
    const auto input_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);
    const auto small_data_buffer = padded_aligned_malloc_t<uint8_t>(round_up_division<size_t>(n, 4), 128);
    const auto medium_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto large_data_buffer = padded_aligned_malloc_t<uint8_t>(n, 128);
    const auto decoded_buffer = padded_aligned_malloc_t<uint16_t>(n, 128);

    memcpy(input_buffer, input_vector.data(), n * sizeof(uint16_t));

    Unconditional286Vb vb;

    vb.encode<RAW_VB_TESTS_TEMPLATE_PARAMETERS>(
            input_buffer,
            n,
            small_data_buffer,
            medium_data_buffer,
            large_data_buffer);

    vb.decode<RAW_VB_TESTS_TEMPLATE_PARAMETERS>(
        small_data_buffer,
        medium_data_buffer,
        large_data_buffer,
        n,
        decoded_buffer);

    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(small_data_buffer[i], 0);
        ASSERT_EQ(large_data_buffer[i], 0);
        ASSERT_EQ(medium_data_buffer[i], i);
    }

    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(input_buffer[i], decoded_buffer[i]) << i;
        if (testing::Test::HasFailure())
        {
            break;
        }
    }

}

TEST(Unconditional286VbTests, RawVbRandomEqualityTest)
{
    const std::vector<int> seeds{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (const auto seed : seeds)
    {
        const auto _input_vector = build_random_vector_u(seed);
        const auto input_vector = *_input_vector;
        std::vector<uint16_t> masked_input_vector;
        masked_input_vector.reserve(input_vector.size());
        for (const auto x : input_vector)
        {
            masked_input_vector.push_back(x);
        }

        run_simple_equality_tests<SIMDAcceleration::AVX2>(masked_input_vector);
        if (HasFailure())
        {
            FAIL() << seed;
            break;
        }
        run_simple_equality_tests<SIMDAcceleration::SSE>(masked_input_vector);
        if (HasFailure())
        {
            FAIL() << seed;
            break;
        }
    }
}