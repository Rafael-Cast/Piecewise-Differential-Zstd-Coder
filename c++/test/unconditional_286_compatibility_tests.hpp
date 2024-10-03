#pragma once

#include "gtest/gtest.h"

#include <vector>
#include <cstddef>
#include <cstdint>
#include "../src/definitions/unconditional286/unconditional286_api.hpp"
#include <functional>

class Unconditional286CompatibilityTestsDataInput :
public testing::TestWithParam<std::vector<uint16_t>>
{
protected:
    void run_serial_encoding_is_parallel_decodable_test(
        std::function<void(
            const uint16_t *parallel_nanopore_restrict in_ptr,
            const size_t n,
            uint8_t *parallel_nanopore_restrict small_data_ptr,
            uint8_t *parallel_nanopore_restrict medium_data_ptr,
            uint8_t *parallel_nanopore_restrict large_data_ptr
        )> encode_fn,
        std::function<void(
            const uint8_t *parallel_nanopore_restrict small_data_ptr,
            const uint8_t *parallel_nanopore_restrict medium_data_ptr,
            const uint8_t *parallel_nanopore_restrict large_data_ptr,
            size_t n,
            uint16_t *out_ptr
        )> decode_fn,
        std::function<uint16_t(uint16_t)> project_fn)
    {
        const auto data = GetParam();
        const auto n = data.size();
        const Unconditional286VbSizes sizes(n);
        auto const serial_encoded_small_data = padded_aligned_malloc(sizes.small_data_buffer_size_bytes(n), 32);
        auto const serial_encoded_medium_data = padded_aligned_malloc(sizes.medium_data_buffer_size_bytes(n), 32);
        auto const serial_encoded_large_data = padded_aligned_malloc(sizes.large_data_buffer_size_bytes(n), 32);
        auto const sse_decoded_data = padded_aligned_malloc_t<uint16_t>(n, 32);

        encode_fn(
            data.data(),
            n,
            serial_encoded_small_data,
            serial_encoded_medium_data,
            serial_encoded_large_data);

        decode_fn(
            serial_encoded_small_data,
            serial_encoded_medium_data,
            serial_encoded_large_data,
            n,
            sse_decoded_data);

        for (size_t i = 0; i < n; ++i)
        {
            const auto projected_real_data = project_fn(data[i]);
            const auto projected_decoded_data = project_fn(sse_decoded_data[i]);
            EXPECT_EQ(projected_real_data, projected_decoded_data)
                << "Mismatch at index " << i << ": expected " << projected_real_data << ", got " << projected_decoded_data;
            if (testing::Test::HasFailure())
            {
                break;
            }
        }
        padded_aligned_free(serial_encoded_small_data);
        padded_aligned_free(serial_encoded_medium_data);
        padded_aligned_free(serial_encoded_large_data);
        padded_aligned_free(sse_decoded_data);
    }
};

constexpr size_t block_size = 8 * 16;

const auto default_testing_vectors = testing::Values(
    std::vector<uint16_t>({0, 0, 0, 0}),
    std::vector<uint16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}),
    std::vector<uint16_t>({3,1,10,4}),
    std::vector<uint16_t>({1, 1, 1, 1}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    repeat_vect<uint16_t>(block_size * 1, 1),
    repeat_vect<uint16_t>(block_size * 1, 1 << 3),
    repeat_vect<uint16_t>(block_size * 1, 1 << 10),
    build_random_vector_u_vect(0),
    build_random_vector_u_vect(1)
);

TEST_P(Unconditional286CompatibilityTestsDataInput, SerialEncodedSmallDataIsSSEDecodable)
{
    run_serial_encoding_is_parallel_decodable_test(Unconditional286VbSerial::encode, Unconditional286VbSSE::decode, [](auto const& data) { return data & 0x3; });
};

TEST_P(Unconditional286CompatibilityTestsDataInput, SerialEncodedMediumDataIsSSEDecodable)
{
    run_serial_encoding_is_parallel_decodable_test(Unconditional286VbSerial::encode, Unconditional286VbSSE::decode, [](auto const& data) { return ((data & (0xFF << 2)) >> 2); });
}

TEST_P(Unconditional286CompatibilityTestsDataInput, SerialEncodedLargeDataIsSSEDecodable)
{
    run_serial_encoding_is_parallel_decodable_test(Unconditional286VbSerial::encode, Unconditional286VbSSE::decode, [](auto const& data) { return data & (0xFC00 >> 10); });
}

TEST_P(Unconditional286CompatibilityTestsDataInput, SerialEncodedFullDataIsSSEDecodable)
{
    run_serial_encoding_is_parallel_decodable_test(Unconditional286VbSerial::encode, Unconditional286VbSSE::decode, [](auto const& data) { return data; });
}

TEST_P(Unconditional286CompatibilityTestsDataInput, SSEEncodedFullDataIsSeriallyDecodable)
{
    run_serial_encoding_is_parallel_decodable_test(Unconditional286VbSSE::encode, Unconditional286VbSerial::decode, [](auto const& data) { return data; });
}

INSTANTIATE_TEST_SUITE_P(
    SerialEncodedSmallDataIsSSEDecodable,
    Unconditional286CompatibilityTestsDataInput,
    default_testing_vectors
);

INSTANTIATE_TEST_SUITE_P(
    SerialEncodedMediumDataIsSSEDecodable,
    Unconditional286CompatibilityTestsDataInput,
    default_testing_vectors);

INSTANTIATE_TEST_SUITE_P(
    SerialEncodedLargeDataIsSSEDecodable,
    Unconditional286CompatibilityTestsDataInput,
    default_testing_vectors
);

INSTANTIATE_TEST_SUITE_P(
    SerialEncodedFullDataIsSSEDecodable,
    Unconditional286CompatibilityTestsDataInput,
    default_testing_vectors);

//INSTANTIATE_TEST_SUITE_P(
//    SSEEncodedFullDataIsSeriallyDecodable,
//    Unconditional286CompatibilityTestsDataInput,
//    default_testing_vectors);