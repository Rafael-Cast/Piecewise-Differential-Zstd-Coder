#pragma once

#include "../src/definitions/delta.hpp"

#include "utils.hpp"

#include <gtest/gtest.h>

#include <vector>
#include <memory.h>

TEST(DeltaMap, EncodesAndDecodesCorrectly)
{
    auto random_seeds = {0, 1, 2, 3, 4};

    for (auto seed : random_seeds)
    {
        auto input_data = build_random_vector(seed);
        auto copy_of_input_data = std::make_unique<std::vector<int16_t>>();
        copy_of_input_data->reserve(input_data->size());
        for (auto data : *input_data)
        {
            copy_of_input_data->push_back(data);
        }
        auto coded = Delta::encode<true>(copy_of_input_data->data(), copy_of_input_data->size());
        auto decoded_raw = Delta::decode<true>(coded, input_data->size());

        auto decoded = copy_to_vector(decoded_raw, input_data->size());

        ASSERT_EQ(*input_data, *decoded);
    }
}