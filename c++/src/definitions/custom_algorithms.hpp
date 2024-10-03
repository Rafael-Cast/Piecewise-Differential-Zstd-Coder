#pragma once

template <bool InPlace, typename TIn, typename TOut, void(f)(TIn *, TOut *, size_t)>
inline TOut *run_inplace_algorithm(TIn *input_buffer, size_t count)
{
    TOut *output_buffer;
    if constexpr (InPlace)
    {
        output_buffer = reinterpret_cast<TOut *>(input_buffer);
    }
    else
    {
        output_buffer = new TOut[count];
    }
    f(input_buffer, output_buffer, count);
    return output_buffer;
}