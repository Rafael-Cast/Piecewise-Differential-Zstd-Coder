#pragma once

#include <cstddef>
#include <cstdint>

#include "../buffered_skip_bit_output_stream/buffered_skip_bit_output_stream.hpp"

class Unconditional286SmallDataOutputStream
{
public:
    Unconditional286SmallDataOutputStream(uint8_t * base_ptr) :
    small_data_output_stream_0(reinterpret_cast<uint16_t*>(base_ptr)),
    small_data_output_stream_1(reinterpret_cast<uint16_t*>(base_ptr) + 1),
    small_data_output_stream_2(reinterpret_cast<uint16_t*>(base_ptr) + 2),
    small_data_output_stream_3(reinterpret_cast<uint16_t*>(base_ptr) + 3),
    small_data_output_stream_4(reinterpret_cast<uint16_t*>(base_ptr) + 4),
    small_data_output_stream_5(reinterpret_cast<uint16_t*>(base_ptr) + 5),
    small_data_output_stream_6(reinterpret_cast<uint16_t*>(base_ptr) + 6),
    small_data_output_stream_7(reinterpret_cast<uint16_t*>(base_ptr) + 7)
    {}

    inline void write(
        uint16_t value_0,
        uint16_t value_1,
        uint16_t value_2,
        uint16_t value_3,
        uint16_t value_4,
        uint16_t value_5,
        uint16_t value_6,
        uint16_t value_7
    ) noexcept
    {
        small_data_output_stream_0.write(value_0);
        small_data_output_stream_1.write(value_1);
        small_data_output_stream_2.write(value_2);
        small_data_output_stream_3.write(value_3);
        small_data_output_stream_4.write(value_4);
        small_data_output_stream_5.write(value_5);
        small_data_output_stream_6.write(value_6);
        small_data_output_stream_7.write(value_7);
    }
private:
    static constexpr size_t SmallDataBitWidth = 2;
    static constexpr size_t BytesPerRegister = 16;
    static constexpr size_t ByteOffsetSize = BytesPerRegister;

    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_0;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_1;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_2;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_3;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_4;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_5;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_6;
    BufferedSkipOutputBitStream<2, 16> small_data_output_stream_7;
};