#pragma once

#include <cstdint>
#include <cstddef>
#include "../attributes.hpp"

#include "unconditional286_read_stream_types.hpp"

class Unconditional286SmallDataStream
{
public:
	Unconditional286SmallDataStream(const uint8_t * parallel_nanopore_restrict base) : ptr(reinterpret_cast<const uint16_t*>(base)) {};

    Unconditional286SmallDataStreamBlock next(void);

private:
    uint_fast8_t cycle = 0;
    const uint16_t *parallel_nanopore_restrict ptr;
    uint16_t buff[8];
};

Unconditional286SmallDataStreamBlock Unconditional286SmallDataStream::next()
{
    const auto res = Unconditional286SmallDataStreamBlock(ptr, cycle);
    cycle += 2;
    if (cycle == 16)
    {
        cycle = 0;
        ptr += 8;
    }
    return res;
}


class Unconditional286MediumLargeDataStream
{
public:
    Unconditional286MediumLargeDataStream(const uint8_t * parallel_nanopore_restrict base) : ptr(base) {};
    uint16_t next()
    {
        const auto res = ptr[cycle + in_cycle_iteration * 2];
        ++in_cycle_iteration;
        if (in_cycle_iteration == 8)
        {
            in_cycle_iteration = 0;
            ++cycle;
            if (cycle == 2)
            {
                cycle = 0;
                ptr += 16;
            }
        }
        return res;
    }
private:
    uint_fast8_t cycle = 0;
    uint_fast8_t in_cycle_iteration = 0;
    const uint8_t *parallel_nanopore_restrict ptr;
};