#pragma once

class Unconditional286VbSizes
{
public:
    constexpr Unconditional286VbSizes(const size_t _n) : n(_n) {}

    static constexpr size_t parallel_small_data_buffer_size_bytes(const size_t n)
    {
        return round_up_division<size_t>(parallel_number_of_processed_samples(n), 4);
    }

    static constexpr size_t parallel_medium_data_buffer_size_bytes(const size_t n)
    {
        return parallel_number_of_processed_samples(n);
    }

    static constexpr size_t parallel_large_data_buffer_size_bytes(const size_t n)
    {
        return parallel_number_of_processed_samples(n);
    }

    static constexpr size_t parallel_number_of_processed_samples(const size_t n)
    {
        return (n / (8 * 16)) * (8 * 16);
    }

    static constexpr size_t serial_number_of_processed_samples(const size_t n)
    {
        return n % (8 * 16);
    }

    static constexpr size_t serial_small_data_buffer_size_bytes(const size_t n)
    {
        return round_up_division<size_t>(serial_number_of_processed_samples(n), 4);
    }

    static constexpr size_t serial_medium_data_buffer_size_bytes(const size_t n)
    {
        return serial_number_of_processed_samples(n);
    }

    static constexpr size_t serial_large_data_buffer_size_bytes(const size_t n)
    {
        return serial_number_of_processed_samples(n);
    }

    static constexpr size_t small_data_buffer_size_bytes(const size_t n)
    {
        return parallel_small_data_buffer_size_bytes(n) + serial_small_data_buffer_size_bytes(n);
    }

    static constexpr size_t medium_data_buffer_size_bytes(const size_t n)
    {
        return parallel_medium_data_buffer_size_bytes(n) + serial_medium_data_buffer_size_bytes(n);
    }

    static constexpr size_t large_data_buffer_size_bytes(const size_t n)
    {
        return parallel_large_data_buffer_size_bytes(n) + serial_large_data_buffer_size_bytes(n);
    }

private:
    const size_t n;
};