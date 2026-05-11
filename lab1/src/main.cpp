#include "main.hpp"

#include <cstdint>

const int BYTE_BASE = 256;
const int BYTE_MASK = 0xFF;
const int UINT32_BYTES = 4;

void CountingPass(TVector<TPair> &input, TVector<TPair> &output, int byteIndex) {
    int count[BYTE_BASE];
    for (int i = 0; i < BYTE_BASE; ++i) {
        count[i] = 0;
    }
    for (size_t i = 0; i < input.Size(); ++i) {
        uint8_t byteVal = static_cast<uint8_t>((input[i].key >> (byteIndex * 8)) & BYTE_MASK);
        count[byteVal]++;
    }
    for (int i = 1; i < BYTE_BASE; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = static_cast<int>(input.Size()) - 1; i >= 0; --i) {
        uint8_t byteVal = static_cast<uint8_t>((input[i].key >> (byteIndex * 8)) & BYTE_MASK);
        int pos = --count[byteVal];
        output[pos] = std::move(input[i]);
    }
}

void RadixSort(TVector<TPair> &data) {
    if (data.Size() < 2) {
        return;
    }
    TVector<TPair> buffer(data.Size());
    for (int byte = 0; byte < UINT32_BYTES; ++byte) {
        CountingPass(data, buffer, byte);
        for (size_t i = 0; i < data.Size(); ++i) {
            data[i] = std::move(buffer[i]);
        }
    }
}
