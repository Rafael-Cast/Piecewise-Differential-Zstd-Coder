#pragma once

#include <cstddef>
#include <cstdint>

#include "../attributes.hpp"

class Unconditional286SmallDataStream;

class Unconditional286DataStreamBlockSharedConstants
{
public:
    static constexpr size_t samples_per_block = 8;
};

class Unconditional286SmallDataStreamBlock
{
public:  
    inline uint16_t get(size_t i) const noexcept
    {
        return (ptr[i] >> cycle) & 0x3;
    }
private:
    Unconditional286SmallDataStreamBlock(
        const uint16_t parallel_nanopore_restrict *_ptr,
        uint16_t _cycle)
        : ptr(_ptr), cycle(_cycle) {};

    const uint16_t parallel_nanopore_restrict * ptr;
    const uint16_t cycle;

    friend class Unconditional286SmallDataStream;
};

//class Unconditional286LargeDataStreamBlock
//{
//public:
//    uint16_t get_data(size_t i);
//};
//class Unconditional286MediumDataStreamBlock
//{
//public:
//    uint16_t get_data(size_t i);
//};