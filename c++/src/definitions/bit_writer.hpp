#pragma once

#include "attributes.hpp"

#include <cstdint>
#include <cstddef>

/*
* TODO: after testing the removal of unnecesary copies in the C5 implementation, test if this speeds the algorithm by avoiding stores
*/
template<size_t N>
class BitWriter
{
public:
    inline BitWriter(void *parallel_nanopore_restrict _ptr) noexcept : 
        ptr(static_cast<uint64_t *>(_ptr)),
        shift(0),
        buff(uint64_t(0)),
        bytes_written(0) {};

    void inline add(uint32_t x) noexcept
    {
        buff |= static_cast<uint64_t>(x) << shift;
        shift += N;
        if (shift >= 64)
        {
            *ptr = buff;
            ptr++;
            bytes_written += 8;
            shift = 0;
            buff = 0;
        }
    }

    //FIXME: assumes padded memory which is not an explicit constraint
    void inline flush() noexcept
    {
        *ptr = buff & ((static_cast<uint64_t>(1) << shift) - 1);
        bytes_written += round_up_division<size_t>(shift, 8);
    }

    size_t inline total_bytes_written() const noexcept
    {
        return bytes_written;
    }

private:
    uint64_t *parallel_nanopore_restrict ptr;
    uint64_t buff;
    size_t shift;
    size_t bytes_written;
};

template<>
class BitWriter<32>
{
public:
    inline BitWriter(void *parallel_nanopore_restrict _ptr) noexcept : 
        ptr(static_cast<uint32_t *>(_ptr)),
        bytes_written(0) {};

    void inline add(uint32_t x) noexcept
    {
        *ptr = x;
        ptr++;
        bytes_written += 4;
    }

    /*
    * Memory must be padded... => FIXME: this should be an explicit constraint
    */
    void inline add_last_bits(uint32_t x, size_t n_bits) noexcept
    {
        *ptr = x;
        bytes_written += round_up_division<size_t>(n_bits, 8);
    }

    void inline flush() noexcept {}

    size_t inline total_bytes_written() const noexcept
    {
        return bytes_written;
    }
private:
    uint32_t *parallel_nanopore_restrict ptr;
    size_t bytes_written;
};

template<>
class BitWriter<8>
{
public:
    inline BitWriter(void *parallel_nanopore_restrict _ptr) noexcept : 
        ptr(static_cast<uint8_t *>(_ptr)),
        bytes_written(0) {};

    void inline add(uint32_t x) noexcept
    {
        *ptr = static_cast<uint8_t>(x);
        ptr++;
        bytes_written++;
    }

    void inline flush() noexcept {}

    size_t inline total_bytes_written() const noexcept
    {
        return bytes_written;
    }
private:
    uint8_t *parallel_nanopore_restrict ptr;
    size_t bytes_written;
};