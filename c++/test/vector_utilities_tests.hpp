#pragma once

#include <gtest/gtest.h>

#include "vector_utilities.hpp"
#include "testing_vector_utilities.hpp"

#include "immintrin.h"

class ShiftsFixture : public testing::TestWithParam<std::pair<__m256i, __m256i>>
{
};

TEST_P(ShiftsFixture, SRL1Epi16ElementFunctionalResultsAreOK)
{
    __m256i input;
    __m256i expected_output;
    std::tie(input, expected_output) = GetParam();

    __m256i actual_output = _mm256_slr_1_epi16_element(input);

    EXPECT_EQ_M256_EPI16(expected_output, actual_output);
};

INSTANTIATE_TEST_SUITE_P(SRL1Epi16ElementFunctionalResultsAreOK,
                         ShiftsFixture,
                         testing::Values(
                             std::pair(
                                _mm256_set_epi16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
                                _mm256_set_epi16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                            ),
                            std::pair(
                                _mm256_set_epi16(0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0, 0xEF),
                                _mm256_set_epi16(0, 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0)
                            )
                        ));

TEST(MM256Swap128Si256, FunctionalResultsAreOk)
{
    auto input = _mm256_set_epi32(0,1,2,3,4,5,6,7);
    auto swapped = _mm256_swap128_si256(input);
    auto expected = _mm256_set_epi32(4, 5, 6, 7, 0, 1, 2, 3);
    EXPECT_EQ_M256_EPI16(swapped, expected);
};

TEST(PackAndUnpackLowBytes, IsReversible)
{
    auto input = _mm256_set_epi64x(1, 2, 3, 4);
    auto packed = pack_low_bytes<true>(input);
    auto unpacked = unpack_low_bytes<true>(packed);
    EXPECT_EQ_M256_EPI16(input, unpacked);
}