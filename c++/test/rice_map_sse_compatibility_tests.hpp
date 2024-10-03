#pragma once

#include "gtest/gtest.h"
#include "../src/definitions/rice_map_sse.hpp"
#include "../src/definitions/utils.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <cstddef>

class RiceMapSSECompatibilityTest : public testing::TestWithParam<std::vector<int16_t>> { };

TEST_P(RiceMapSSECompatibilityTest, RiceMapSSESerialEncodingIsParallelDecodable)
{
    const auto data = GetParam();
    const auto n = data.size();
    const auto encoded_buffer = std::make_unique<uint16_t[]>(n);
    const auto decoded_buffer = std::make_unique<int16_t[]>(n);
    RiceMapSSE::encode_buffer_serially(data.data(), encoded_buffer.get(), n);
    RiceMapSSE::decode_buffer_parallel(encoded_buffer.get(), decoded_buffer.get(), n);

    for (size_t i = 0; i < n; ++i)
    {
        EXPECT_EQ(data[i], decoded_buffer[i]) << "Failure at index " << i << std::endl << "Expected: " << data[i] << std::endl << "Got: " << decoded_buffer[i] << std::endl;
    }
}


INSTANTIATE_TEST_CASE_P(
    RiceMapSSESerialEncodingIsParallelDecodable,
    RiceMapSSECompatibilityTest,
    testing::Values(
        build_random_vector_vect(0),
        build_random_vector_vect(1),
        build_random_vector_vect(2),
        build_random_vector_vect(3)
    ));