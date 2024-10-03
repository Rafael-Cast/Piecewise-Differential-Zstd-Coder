#pragma once

#include "gtest/gtest.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include "../src/definitions/unconditional286/unconditional286_read_streams.hpp"
#include <utility>

//class Unconditional286MediumLargeDataReadStreamTestFixture : testing::TestWithParam<std::pair<const std::vector<uint8_t> &, const std::vector<uint8_t> &>>
//{
//protected:
//    Unconditional286MediumLargeDataReadStreamTestFixture() {}
//
//    virtual void SetUp()
//    {
//        const auto param = GetParam();
//        const auto input = param.first;
//        const auto expected = param.second;
//        n = input.size();
//        this->expected = expected.data();
//        assert(input.size() == expected.size());
//        stream = std::make_unique<Unconditional286MediumLargeDataStream>(input.data());
//    }
//
//    virtual void TearDown()
//    {
//        stream.release();
//    }
//
//public:
//    void run()
//    {
//        for (size_t i = 0; i < n; i++)
//        {
//            const auto data = stream->next();
//            ASSERT_EQ(expected, data) << i;
//        }
//    }
//
//    size_t n;
//    const uint8_t * expected;
//    std::unique_ptr<Unconditional286MediumLargeDataStream> stream;
//};
//
//TEST_P(Unconditional286MediumLargeDataReadStreamTestFixture, StreamIsCorrectlyRead)
//{
//    run();
//};
//
//const std::vector<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> default_testing_cases
//{
//    {{0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
//};

//INSTANTIATE_TEST_SUITE_P(
//    StreamIsCorrectlyRead,
//    Unconditional286MediumLargeDataReadStreamTestFixture,
//    testing::ValuesIn<std::vector<uint8_t>, std::vector<uint8_t>>(default_testing_cases));

TEST(Unconditional286MediumLargeDataReadStreamTestFixtureDummy, StreamIsCorrectlyReadDummy)
{
    const std::pair<std::vector<uint8_t>, std::vector<uint8_t>> param = {{0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    const auto input = param.first;
    const auto expected = param.second;
    auto n = input.size();
    assert(input.size() == expected.size());
    auto stream = std::make_unique<Unconditional286MediumLargeDataStream>(input.data());

    for (size_t i = 0; i < n; i++)
    {
        const auto data = stream->next();
        ASSERT_EQ(expected[i], data) << i;
    }

    stream.release();

}