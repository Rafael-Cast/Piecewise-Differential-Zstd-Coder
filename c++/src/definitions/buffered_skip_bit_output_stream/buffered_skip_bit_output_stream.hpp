#pragma once

#include <cstddef>
#include <cstdint>
#include "stddef.h"
#include "../attributes.hpp"

/*
* We assume there is no need to flash the stream ever
*/

template<
size_t BitsPerElement,
size_t BytesSkip>
class BufferedSkipOutputBitStream
{
public:
    BufferedSkipOutputBitStream(uint16_t * parallel_nanopore_restrict _out) : out(_out), buffer(0), bits_written(0) {}
    inline void write(const uint16_t x) noexcept;
private:
    uint16_t *parallel_nanopore_restrict out;
    uint16_t buffer;
    size_t bits_written;
};

template<
size_t BitsPerElement,
size_t BytesSkip>
inline void BufferedSkipOutputBitStream<BitsPerElement, BytesSkip>::write(uint16_t x) noexcept
{
    FAIL_COMPILE_IF(BitsPerElement != 2 && BitsPerElement != 1 && BitsPerElement != 4 && BitsPerElement != 8 && BitsPerElement != 16);
    //constexpr uint16_t mask = (1 << BitsPerElement) - 1; We assume masking is already done by the user
    buffer |= x << bits_written;
    bits_written += BitsPerElement;
    if (bits_written >= 16)
    {
        *out = buffer;
        out = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(out) + BytesSkip);
        bits_written = 0;
        buffer = 0;
    }
}