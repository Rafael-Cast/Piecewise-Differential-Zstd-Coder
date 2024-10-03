#pragma once

#include <cstdint>

uint8_t *zstd_encode(uint8_t *buffer)
{
    // TODO: implement
    return buffer;
}

uint8_t *zstd_encode_mock(uint8_t *buffer)
{
    return buffer;
}

uint8_t *zstd_decode_mock(uint8_t* buffer)
{
    return buffer;
}

constexpr void encode_uint64(uint64_t x, uint8_t *buffer)
{
    uint8_t current_byte = 0;
    // TODO: rewrite to mirror the code of the decode function
    for (int i = 0; i < 8; i++)
    {
        const unsigned shift_expresion = i * 8;
        current_byte = static_cast<uint8_t>((x & (static_cast<uint64_t>(0xFF) << shift_expresion)) >> shift_expresion);
        *buffer = current_byte;
        buffer++;
    }
};

constexpr uint64_t decode_uint64(uint8_t const *buffer)
{
    return static_cast<uint64_t>(*buffer) |
           static_cast<uint64_t>(*(buffer + 1)) << 8 |
           static_cast<uint64_t>(*(buffer + 2)) << 16 |
           static_cast<uint64_t>(*(buffer + 3)) << 24 |
           static_cast<uint64_t>(*(buffer + 4)) << 32 |
           static_cast<uint64_t>(*(buffer + 5)) << 40 |
           static_cast<uint64_t>(*(buffer + 6)) << 48 |
           static_cast<uint64_t>(*(buffer + 7)) << 56;
}