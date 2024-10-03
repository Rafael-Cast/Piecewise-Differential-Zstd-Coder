#include "../src/definitions/memory_utils.hpp"

#include "../src/definitions/utils.hpp"

#include "../src/definitions/unconditional286/unconditional286_api.hpp"

#include <vector>
#include <memory>
#include <cstring>

#include <zstd.h>


void test_Unconditional286()
{
    constexpr size_t block_size = 8 * 16;

    const std::vector<std::vector<uint16_t>> test_cases{
        std::vector<uint16_t>({0, 0, 0, 0}),
        std::vector<uint16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}),
        std::vector<uint16_t>({3, 1, 10, 4}),
        std::vector<uint16_t>({1, 1, 1, 1}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        repeat_vect<uint16_t>(block_size * 1, 1),
        repeat_vect<uint16_t>(block_size * 1, 1 << 3),
        repeat_vect<uint16_t>(block_size * 1, 1 << 10),
        build_random_vector_u_vect(0),
        build_random_vector_u_vect(1)};
    
    for (const auto & test_case : test_cases)
    {
        const auto & data_vector = test_case;
        const auto n = data_vector.size();
        const Unconditional286VbSizes sizes(n);
        const auto encoded_small_data = padded_aligned_malloc(sizes.small_data_buffer_size_bytes(n), 32);
        const auto encoded_medium_data = padded_aligned_malloc(sizes.medium_data_buffer_size_bytes(n), 32);
        const auto encoded_large_data = padded_aligned_malloc(sizes.large_data_buffer_size_bytes(n), 32);
        const auto decoded_data = padded_aligned_malloc_t<uint16_t>(n, 32);
        const auto data = padded_aligned_malloc_t<uint16_t>(n, 32);
        memcpy(data, data_vector.data(), n * sizeof(uint16_t));

        Unconditional286VbSerial::encode(
            data,
            n,
            encoded_small_data,
            encoded_medium_data,
            encoded_large_data);

        Unconditional286VbSSE::decode(
            encoded_small_data,
            encoded_medium_data,
            encoded_large_data,
            n,
            decoded_data);

        padded_aligned_free(encoded_small_data);
        padded_aligned_free(encoded_medium_data);
        padded_aligned_free(encoded_large_data);
        padded_aligned_free(decoded_data);
        padded_aligned_free(data);
    }
}

void test_Unconditional286_2()
{
    constexpr size_t block_size = 8 * 16;

    const std::vector<std::vector<uint16_t>> test_cases{
        // std::vector<uint16_t>({0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}),
        // std::vector<uint16_t>({3, 1, 10, 4}),
        // std::vector<uint16_t>({1, 1, 1, 1}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        // std::vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
        repeat_vect<uint16_t>(block_size * 1, 1),
        // repeat_vect<uint16_t>(block_size * 1, 1 << 3),
        // repeat_vect<uint16_t>(block_size * 1, 1 << 10),
        // build_random_vector_u_vect(0),
        // build_random_vector_u_vect(1)
    };

    for (const auto & test_case : test_cases)
    {
        const auto & data_vector = test_case;
        const auto n = data_vector.size();
        const Unconditional286VbSizes sizes(n);
        const auto encoded_small_data = padded_aligned_malloc(sizes.small_data_buffer_size_bytes(n), 32);
        const auto encoded_medium_data = padded_aligned_malloc(sizes.medium_data_buffer_size_bytes(n), 32);
        const auto encoded_large_data = padded_aligned_malloc(sizes.large_data_buffer_size_bytes(n), 32);
        const auto decoded_data = padded_aligned_malloc_t<uint16_t>(n, 32);
        const auto data = padded_aligned_malloc_t<uint16_t>(n, 32);
        memcpy(data, data_vector.data(), n * sizeof(uint16_t));

        Unconditional286VbSSE::encode(
            data,
            n,
            encoded_small_data,
            encoded_medium_data,
            encoded_large_data);

        Unconditional286VbSerial::decode(
            encoded_small_data,
            encoded_medium_data,
            encoded_large_data,
            n,
            decoded_data);

        padded_aligned_free(encoded_small_data);
        padded_aligned_free(encoded_medium_data);
        padded_aligned_free(encoded_large_data);
        padded_aligned_free(decoded_data);
        padded_aligned_free(data);
    }

}

int main(void)
{
    test_Unconditional286();
    test_Unconditional286_2();
    return 0;
}