#pragma once

#include "custom_algorithms.hpp"

#include <cstdint>

class Delta
{
public:
    //template <typename T>
    //static constexpr T encode_single_sample(const T &current, const T &previous)
    //{
    //    return current - previous;
    //}
    //template <typename T>
    //static constexpr T decode_single_sample(const T &current, const T &previous)
    //{
    //    return current + previous;
    //}
    template <typename T>
    static constexpr T encode_single_sample(const T current, const T previous)
    {
        return current - previous;
    }
    template <typename T>
    static constexpr T decode_single_sample(const T current, const T previous)
    {
        return current + previous;
    }

    template <bool InPlace, typename T>
    static constexpr T *encode(T *buffer, const size_t count)
    {
        return run_inplace_algorithm<InPlace, T, T, Delta::encode<T>>(buffer, count);
    };

    template <bool InPlace, typename T>
    static constexpr T*decode(T* buffer, const size_t count)
    {
        return run_inplace_algorithm < InPlace, T, T, Delta::decode<T>>(buffer, count);
    }

    template <typename T>
    static constexpr void encode_no_inplace(const T * input_buffer, T* output_buffer, const size_t count)
    {
        encode(const_cast<T*>(input_buffer), output_buffer, count); //FIXME: This is not good code (const_cast)
    }

    template <typename T>
    static constexpr void decode_no_inplace(const T * input_buffer, T* output_buffer, const size_t count)
    {
        decode(const_cast<T *>(input_buffer), output_buffer, count); //FIXME: this is not good code (const_cast)
    }

private:
    template <typename T>
    static constexpr void encode(T *input_buffer, T *output_buffer, const size_t count)
    {
        auto current_input_ptr = input_buffer;
        auto end_input_ptr = input_buffer + count;
        auto current_output_ptr = output_buffer;
        T previous = T(0);
        T current;
        for (; current_input_ptr != end_input_ptr; current_input_ptr++, current_output_ptr++)
        {
            current = *current_input_ptr;
            *current_output_ptr = encode_single_sample(current, previous);
            previous = current;
        }
    }

    template <typename T>
    static constexpr void decode(T * input_buffer, T* output_buffer, const size_t count)
    {
        auto current_input_ptr = input_buffer;
        auto end_input_ptr = input_buffer + count;
        auto current_output_ptr = output_buffer;
        T previous = T(0);
        T decoded_sample;
        for (; current_input_ptr != end_input_ptr; current_input_ptr++, current_output_ptr++)
        {
            decoded_sample = decode_single_sample(*current_input_ptr, previous);
            *current_output_ptr = decoded_sample;
            previous = decoded_sample;
        }
    }
};