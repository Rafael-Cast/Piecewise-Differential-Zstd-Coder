#pragma once

/*
* SVB is used in some of NTI's implementations
* In order to avoid bringing the full library into our namespace
* We copy only needed parts here
* All credits go to their original authors
*/

#include <gsl-lite.hpp>

#ifdef _MSC_VER
#define SVB_RESTRICT __restrict
#else
#define SVB_RESTRICT __restrict__
#endif

#if __cplusplus >= 201703L
#define SVB16_IF_CONSTEXPR if constexpr
#else
#define SVB16_IF_CONSTEXPR if
#endif

namespace svb16NTI {
namespace detail {
inline uint16_t zigzag_encode(uint16_t val)
{
    return (val + val) ^ static_cast<uint16_t>(static_cast<int16_t>(val) >> 15);
}
}  // namespace detail

template <typename Int16T, bool UseDelta, bool UseZigzag>
uint8_t * encode_scalar(
    Int16T const * in,
    uint8_t * SVB_RESTRICT keys,
    uint8_t * SVB_RESTRICT data,
    uint32_t count,
    Int16T prev = 0)
{
    if (count == 0) {
        return data;
    }

    uint8_t shift = 0;  // cycles 0 through 7 then resets
    uint8_t key_byte = 0;
    for (uint32_t c = 0; c < count; c++) {
        if (shift == 8) {
            shift = 0;
            *keys++ = key_byte;
            key_byte = 0;
        }
        uint16_t value;
        SVB16_IF_CONSTEXPR(UseDelta)
        {
            // need to do the arithmetic in unsigned space so it wraps
            value = static_cast<uint16_t>(in[c]) - static_cast<uint16_t>(prev);
            SVB16_IF_CONSTEXPR(UseZigzag) { value = detail::zigzag_encode(value); }
            prev = in[c];
        }
        else SVB16_IF_CONSTEXPR(UseZigzag) {
            value = detail::zigzag_encode(static_cast<uint16_t>(in[c]));
        }
        else {
            value = static_cast<uint16_t>(in[c]);
        }

        if (value < (1 << 8)) {  // 1 byte
            *data = static_cast<uint8_t>(value);
            ++data;
        } else {                           // 2 bytes
            std::memcpy(data, &value, 2);  // assumes little endian
            data += 2;
            key_byte |= 1 << shift;
        }

        shift += 1;
    }

    *keys = key_byte;  // write last key (no increment needed)
    return data;
}

}  // namespace svb16

namespace svb16NTI {
namespace detail {
inline uint16_t zigzag_decode(uint16_t val)
{
    return (val >> 1) ^ static_cast<uint16_t>(0 - (val & 1));
}

inline uint16_t decode_data(gsl::span<uint8_t const>::iterator & dataPtr, uint8_t code)
{
    uint16_t val;

    if (code == 0) {  // 1 byte
        val = (uint16_t)*dataPtr;
        dataPtr += 1;
    } else {  // 2 bytes
        val = 0;
        memcpy(&val, dataPtr, 2);  // assumes little endian
        dataPtr += 2;
    }

    return val;
}
}  // namespace detail

template <typename Int16T, bool UseDelta, bool UseZigzag>
uint8_t const * decode_scalar(
    gsl::span<Int16T> out_span,
    gsl::span<uint8_t const> keys_span,
    gsl::span<uint8_t const> data_span,
    Int16T prev = 0)
{
    auto const count = out_span.size();
    if (count == 0) {
        return data_span.begin();
    }

    auto out = out_span.begin();
    auto keys = keys_span.begin();
    auto data = data_span.begin();

    uint8_t shift = 0;  // cycles 0 through 7 then resets
    uint8_t key_byte = *keys++;
    // need to do the arithmetic in unsigned space so it wraps
    auto u_prev = static_cast<uint16_t>(prev);
    for (uint32_t c = 0; c < count; c++, shift++) {
        if (shift == 8) {
            shift = 0;
            key_byte = *keys++;
        }
        uint16_t value = detail::decode_data(data, (key_byte >> shift) & 0x01);
        SVB16_IF_CONSTEXPR(UseZigzag) { value = detail::zigzag_decode(value); }
        SVB16_IF_CONSTEXPR(UseDelta)
        {
            value += u_prev;
            u_prev = value;
        }
        *out++ = static_cast<Int16T>(value);
    }

    assert(out == out_span.end());
    assert(keys == keys_span.end());
    assert(data <= data_span.end());
    return data;
}

}  // namespace svb16

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

/// Get the number of key bytes required to encode a given number of 16-bit integers.
inline uint32_t svb16_key_length_2bit(uint32_t count)
{
    // ceil(count / 4.0), without overflowing or using fp arithmetic
    return (count >> 2) + (((count & 3) + 3) >> 2);
}

/// Get the maximum number of bytes required to encode a given number of 16-bit integers.
inline uint32_t svb16_max_encoded_length_2bit(uint32_t count)
{
    return svb16_key_length_2bit(count) + (2 * count);
}

#if defined(__cplusplus)
};
#endif


namespace svb16NTI {

// Required extra space after readable buffers passed in.
//
// Require 1 128 bit buffer beyond the end of all input readable buffers.
inline std::size_t decode_input_buffer_padding_byte_count()
{
//#ifdef SVB16_X64
//  return sizeof(__m128i);
//#else
    return 0;
//#endif
}

}  // namespace svb16