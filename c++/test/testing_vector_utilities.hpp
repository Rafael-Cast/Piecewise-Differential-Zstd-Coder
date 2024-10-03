#pragma once

#include <gtest/gtest.h>

#define EXPECT_EQ_M256_EPI16(expected_output, actual_output)\
    auto x_0 = _mm256_extract_epi16(expected_output, 0);\
    auto x_1 = _mm256_extract_epi16(expected_output, 1);\
    auto x_2 = _mm256_extract_epi16(expected_output, 2);\
    auto x_3 = _mm256_extract_epi16(expected_output, 3);\
    auto x_4 = _mm256_extract_epi16(expected_output, 4);\
    auto x_5 = _mm256_extract_epi16(expected_output, 5);\
    auto x_6 = _mm256_extract_epi16(expected_output, 6);\
    auto x_7 = _mm256_extract_epi16(expected_output, 7);\
    auto x_8 = _mm256_extract_epi16(expected_output, 8);\
    auto x_9 = _mm256_extract_epi16(expected_output, 9);\
    auto x_10 = _mm256_extract_epi16(expected_output, 10);\
    auto x_11 = _mm256_extract_epi16(expected_output, 11);\
    auto x_12 = _mm256_extract_epi16(expected_output, 12);\
    auto x_13 = _mm256_extract_epi16(expected_output, 13);\
    auto x_14 = _mm256_extract_epi16(expected_output, 14);\
    auto x_15 = _mm256_extract_epi16(expected_output, 15);\
\
    auto y_0 = _mm256_extract_epi16(actual_output, 0);\
    auto y_1 = _mm256_extract_epi16(actual_output, 1);\
    auto y_2 = _mm256_extract_epi16(actual_output, 2);\
    auto y_3 = _mm256_extract_epi16(actual_output, 3);\
    auto y_4 = _mm256_extract_epi16(actual_output, 4);\
    auto y_5 = _mm256_extract_epi16(actual_output, 5);\
    auto y_6 = _mm256_extract_epi16(actual_output, 6);\
    auto y_7 = _mm256_extract_epi16(actual_output, 7);\
    auto y_8 = _mm256_extract_epi16(actual_output, 8);\
    auto y_9 = _mm256_extract_epi16(actual_output, 9);\
    auto y_10 = _mm256_extract_epi16(actual_output, 10);\
    auto y_11 = _mm256_extract_epi16(actual_output, 11);\
    auto y_12 = _mm256_extract_epi16(actual_output, 12);\
    auto y_13 = _mm256_extract_epi16(actual_output, 13);\
    auto y_14 = _mm256_extract_epi16(actual_output, 14);\
    auto y_15 = _mm256_extract_epi16(actual_output, 15);\
\
    EXPECT_EQ(x_0, y_0);\
    EXPECT_EQ(x_1, y_1);\
    EXPECT_EQ(x_2, y_2);\
    EXPECT_EQ(x_3, y_3);\
    EXPECT_EQ(x_4, y_4);\
    EXPECT_EQ(x_5, y_5);\
    EXPECT_EQ(x_6, y_6);\
    EXPECT_EQ(x_7, y_7);\
    EXPECT_EQ(x_8, y_8);\
    EXPECT_EQ(x_9, y_9);\
    EXPECT_EQ(x_10, y_10);\
    EXPECT_EQ(x_11, y_11);\
    EXPECT_EQ(x_12, y_12);\
    EXPECT_EQ(x_13, y_13);\
    EXPECT_EQ(x_14, y_14);\
    EXPECT_EQ(x_15, y_15);
