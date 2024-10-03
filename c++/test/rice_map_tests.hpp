#pragma once

#include "../src/definitions/rice_map.hpp"

#include "utils.hpp"

#include "immintrin.h"

#include <gtest/gtest.h>

#include <vector>
#include <memory.h>
#include "testing_vector_utilities.hpp"

TEST(RiceMapTest, ScalarEncodesAndDecodesCorrectly)
{
    auto values = {0, 1, 2, 3, 4, 5};
    for (auto value : values)
    {
        auto mapped = RiceMap::encode_single_sample(value);
        auto decoded = RiceMap::decode_single_sample(mapped);
        ASSERT_EQ(value, decoded);
    }
}

TEST(RiceMapTest, ParallelEncodesAndDecodesCorrectly)
{
    auto values = {_mm256_set1_epi16(0), _mm256_set1_epi16(1), _mm256_set1_epi16(2), _mm256_set1_epi16(3), _mm256_set1_epi16(4), _mm256_set1_epi16(5)};
    for (auto value : values)
    {
        auto mapped = RiceMap::encode_single_sample(value);
        auto decoded = RiceMap::decode_single_sample(mapped);
        EXPECT_EQ_M256_EPI16(value, decoded);
    }
}