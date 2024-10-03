#pragma once

#include "../src/definitions/delta_zig_zag.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

struct OutputExamples
{
public:
    std::vector<int16_t> input;
    std::vector<uint16_t> expected_output;
    std::vector<uint16_t> output;

    OutputExamples(std::vector<int16_t> &&input, std::vector<uint16_t> &&expected_output)
    {
        this->input = input;
        this->expected_output = expected_output;
        assert(this->input.size() == this->expected_output.size());
        for (size_t i = 0; i < input.size(); i++)
        {
            this->output.push_back(0);
        }
    }
};

// {0, -1, 1, -2, 2}
// {0, 0, -1, 1, -2}
// {0, -1, 2, -3, 4}
// {0, 1, 4, 5, 8}
OutputExamples encoding_example1({0, -1, 1, -2, 2}, {0, 1, 4, 5, 8});

class DeltaZigZagFixture : public testing::TestWithParam<size_t>
{
};

class DeltaZigZagRandomFixture : public testing::TestWithParam<int>
{
};

TEST(DeltaZigZagTests, SerialEncodingWorksCorrectly)
{
    auto output_ptr = DeltaZigZag::encode<false, false>(encoding_example1.input.data(), encoding_example1.input.size());

    for (size_t i = 0; i < encoding_example1.input.size(); i++)
    {
        encoding_example1.output[i] = output_ptr[i];
    }

    ASSERT_EQ(encoding_example1.expected_output, encoding_example1.output);
}

TEST(DeltaZigZag, AvxEncodingWorksCorrectly)
{
    auto output_ptr = DeltaZigZag::encode<false, true>(encoding_example1.input.data(), encoding_example1.input.size());

    for (size_t i = 0; i < encoding_example1.input.size(); i++)
    {
        encoding_example1.output[i] = output_ptr[i];
    }

    ASSERT_EQ(encoding_example1.expected_output, encoding_example1.output);
}

TEST_P(DeltaZigZagFixture, EncodesCorrectlyInVariousLenghts)
{
    size_t n = GetParam();
    auto xs = repeat(n, int16_t(1));
    auto xs_copy = copy_to_vector(xs->data(), xs->size());

    auto encoded = DeltaZigZag::encode<false>(xs_copy->data(), n);

    auto decoded_raw = DeltaZigZag::decode<false>(encoded, n);

    auto decoded = copy_to_vector(decoded_raw, n);

    ASSERT_EQ(*xs, *decoded);
}

TEST_P(DeltaZigZagRandomFixture, DeltaZigZagEncodingAndDecodingMatches)
{
    auto seed = GetParam();

    auto random_input = build_random_vector(seed);
    auto random_input_size = random_input->size();

    auto encoded = DeltaZigZag::encode<false, true>(random_input->data(), random_input_size);

    auto decoded_raw = DeltaZigZag::decode<false, true>(encoded, random_input_size);

    auto decoded = copy_to_vector(decoded_raw, random_input_size);

    ASSERT_EQ(*random_input, *decoded);

    delete[] encoded;
}

INSTANTIATE_TEST_SUITE_P(EncodesCorrectlyInVariousLenghts,
                        DeltaZigZagFixture,
                        testing::Values(1,2,8,15,16,17,31,32,33));

INSTANTIATE_TEST_SUITE_P(DeltaZigZagEncodingAndDecodingMatches,
                         DeltaZigZagRandomFixture,
                         testing::Values(0));