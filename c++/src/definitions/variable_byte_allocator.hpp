#pragma once

#include "memory_utils.hpp"

#include <cstddef>

///*
//    TODO: use compile time polimorfism using boost::variant + visitor + wrapper
//*/
//class VariableByteAllocator
//{
//public:
//    template <typename T>
//    virtual T *allocate(size_t) = 0;
//    virtual void deallocate(void *) = 0;
//};

/*
 *  TODO: refactor so there exists a MemoryRequirements class
 * and all algorithms have an instance of this MemoryRequirements class (possibly as a type trait)
 * that can be injected into an allocator to configure it to satisfy those needs.
 * It might be useful for this class to also have operators such that given two
 * algorithms A and B such that A requires memory aligned to 32 bytes and 64 byte padding
 * while B requires 64 byte alignment and no padding, the sum of their requirements is
 * an instance of the MemoryRequirements class that specifies
 * 64 byte alignment and 64 byte padding.
 * This will allow the compressor to quickly determine how the 
 * memory for it's internal buffers needs to be allocated
 */
class C2VariableByteAllocator
{
public:
    template<typename T>
    inline T* allocate(size_t n) const
    {
        return padded_aligned_malloc_t<T>(n, 32);
    }

    inline uint8_t *allocate_keys(size_t n) const
    {
        return padded_aligned_malloc_t<uint8_t>(bytes_needed_for_keys(n), 32);
    }

    inline uint8_t *allocate_high_bytes(size_t n) const
    {
        return padded_aligned_malloc_t<uint8_t>(bytes_needed_for_high_bytes(n), 32);
    }

    inline uint8_t *allocate_low_bytes(size_t n) const
    {
        return padded_aligned_malloc_t<uint8_t>(bytes_needed_for_low_bytes(n), 32);
    }

    inline uint16_t *allocate_samples(size_t n) const
    {
        return padded_aligned_malloc_t<uint16_t>(n, 32);
    }

    inline void deallocate(void * ptr) const
    {
        padded_aligned_free(ptr);
    }

    constexpr size_t bytes_needed_for_keys(size_t n) const noexcept
    {
        return (n / 8) + (((n % 8) == 0) ? 0 : 1);
    }

    constexpr size_t bytes_needed_for_low_bytes(size_t n) const noexcept
    {
        return n;
    }

    constexpr size_t bytes_needed_for_high_bytes(size_t n) const noexcept
    {
        return n;
    }
};