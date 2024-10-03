#pragma once

#include <memory>
#include <cstddef>
#include <cstdlib>

uint8_t* padded_aligned_malloc(size_t n, size_t alignment_bytes)
{
    if (n % alignment_bytes != 0)
    {
        n = (n / alignment_bytes) * alignment_bytes + alignment_bytes;
    }
    return static_cast<uint8_t*>(aligned_alloc(alignment_bytes, n));
}

void padded_aligned_free(void * x)
{
    free(x);
}

template<typename T = uint8_t>
T* padded_aligned_malloc_t(size_t n, size_t alignment_bytes)
{
    return static_cast<T *>(static_cast < void *>(padded_aligned_malloc(n * sizeof(T), alignment_bytes)));
}