#pragma once

#include "../attributes.hpp"

#include <cstdint>
#include <cstddef>

// We assume we never have to flush the stream

class Unconditional286MediumLargeDataOutputStream
{
public:
    inline Unconditional286MediumLargeDataOutputStream(uint8_t * parallel_nanopore_restrict) noexcept;
    inline void write(
        uint8_t x_0,
        uint8_t x_1,
        uint8_t x_2,
        uint8_t x_3,
        uint8_t x_4,
        uint8_t x_5,
        uint8_t x_6,
        uint8_t x_7) noexcept;

private:
    uint8_t * parallel_nanopore_restrict ptr;
    size_t cycle = 0;
};

Unconditional286MediumLargeDataOutputStream::Unconditional286MediumLargeDataOutputStream(
    uint8_t * parallel_nanopore_restrict base) noexcept : ptr(base) {}


inline void Unconditional286MediumLargeDataOutputStream::write(
    uint8_t x_0,
    uint8_t x_1,
    uint8_t x_2,
    uint8_t x_3,
    uint8_t x_4,
    uint8_t x_5,
    uint8_t x_6,
    uint8_t x_7) noexcept
{
    ptr[cycle] = x_0;
    ptr[cycle + 2] = x_1;
    ptr[cycle + 4] = x_2;
    ptr[cycle + 6] = x_3;
    ptr[cycle + 8] = x_4;
    ptr[cycle + 10] = x_5;
    ptr[cycle + 12] = x_6;
    ptr[cycle + 14] = x_7;

    ++cycle;

    if (cycle == 2)
    {
        cycle = 0;
        ptr += 16;
    }
}