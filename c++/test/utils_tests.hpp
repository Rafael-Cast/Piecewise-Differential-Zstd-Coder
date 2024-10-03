#pragma once

#include <gtest/gtest.h>
#include "../src/definitions/utils.hpp"

TEST(PackedKeysToSTLVectorTest, CorrectUnpack1)
{
    const uint64_t keys = 0xF;
    const auto size = 2;
    const auto res = key_buffer_to_unpacked_stl_vector<uint16_t>(reinterpret_cast<const uint8_t *>(&keys), 2);
    const std::vector<uint16_t> expected{3, 3};

    ASSERT_EQ(res, expected);
}

TEST(PackedKeysToSTLVectorTest, CorrectUnpack2)
{
    const uint64_t keys = 0x0;
    const auto size = 2;
    const auto res = key_buffer_to_unpacked_stl_vector<uint16_t>(reinterpret_cast<const uint8_t *>(&keys), 2);
    const std::vector<uint16_t> expected{0, 0};

    ASSERT_EQ(res, expected);
}

TEST(PackedKeysToSTLVectorTest, CorrectUnpack3)
{
    const uint64_t keys = 0x5;
    const auto size = 2;
    const auto res = key_buffer_to_unpacked_stl_vector<uint16_t>(reinterpret_cast<const uint8_t *>(&keys), 2);
    const std::vector<uint16_t> expected{1, 1};

    ASSERT_EQ(res, expected);
}

TEST(PackedKeysToSTLVectorTest, CorrectUnpack4)
{
    const uint64_t keys = 0xA;
    const auto size = 2;
    const auto res = key_buffer_to_unpacked_stl_vector<uint16_t>(reinterpret_cast<const uint8_t *>(&keys), 2);
    const std::vector<uint16_t> expected{2, 2};

    ASSERT_EQ(res, expected);
}