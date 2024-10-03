#pragma once

class RiceMapSSE
{
public:
    static inline __m128i encode_single_sample(const __m128i &x);
    static inline __m128i decode_single_sample(const __m128i &x);
    static constexpr inline uint16_t encode_single_sample(const int16_t x);
    static constexpr inline int16_t decode_single_sample(const uint16_t x);

    static void encode_buffer_serially(const int16_t *input, uint16_t *output, size_t length)
    {
        for (size_t i = 0; i < length; ++i)
        {
            output[i] = encode_single_sample(input[i]);
        }
    }

    static void decode_buffer_serially(const uint16_t *input, int16_t *output, size_t length)
    {
        for (size_t i = 0; i < length; ++i)
        {
            output[i] = decode_single_sample(input[i]);
        }
    }

    static void encode_buffer_parallel(const int16_t *input, uint16_t *output, size_t length)
    {
        for (size_t i = 0; i < length / 8; ++i)
        {
            const __m128i x = _mm_loadu_si128((const __m128i *)(input + i * 8));
            const __m128i y = encode_single_sample(x);
            _mm_storeu_si128((__m128i *)(output + i * 8), y);
        }

        for (size_t i = 0; i < length % 8; ++i)
        {
            output[i + 8 * (length / 8)] = encode_single_sample(input[i + 8 * (length / 8)]);
        }
    }

    static void decode_buffer_parallel(const uint16_t *input, int16_t *output, size_t length)
    {
        for (size_t i = 0; i < length / 8; ++i)
        {
            const __m128i x = _mm_loadu_si128((const __m128i  *)(input + i * 8));
            const __m128i y = decode_single_sample(x);
            _mm_storeu_si128((__m128i  *)(output + i * 8), y);
        }

        for (size_t i = 0; i < length % 8; ++i)
        {
            output[i + 8 * (length / 8)] = decode_single_sample(input[i + 8 * (length / 8)]);
        }
    }
};

inline __m128i RiceMapSSE::encode_single_sample(const __m128i &x)
{
    return _mm_xor_si128(_mm_add_epi16(x, x), _mm_srai_epi16(x, 16));
}
const __m128i lowest_bit_mask_128_ = _mm_set1_epi16(0x01);

inline __m128i RiceMapSSE::decode_single_sample(const __m128i &x)
{
    return _mm_xor_si128(_mm_srli_epi16(x, 1),
                         _mm_sub_epi16(_mm_setzero_si128(),
                                       _mm_and_si128(x, lowest_bit_mask_128_)));
}

constexpr uint16_t RiceMapSSE::encode_single_sample(const int16_t x)
{
    return (x + x) ^ static_cast<uint16_t>(static_cast<int16_t>(x) >> 15);
} // TODO: can val + val be replaced by val << 1?

constexpr int16_t RiceMapSSE::decode_single_sample(const uint16_t x)
{
    return (x >> 1) ^ static_cast<uint16_t>(0 - (x & 1)); // TODO: Once again this assumes 2's compliment, right?
}