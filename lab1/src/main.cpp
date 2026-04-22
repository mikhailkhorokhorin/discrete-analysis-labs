#include "main.hpp"
#include <iostream>
#include <string>

void CountingPass(Vector<Pair> &input, Vector<Pair> &output, int byteIndex) {
    const int BASE = 256;
    int count[BASE];

    for (int i = 0; i < BASE; ++i) {
        count[i] = 0;
    }

    for (size_t i = 0; i < input.Size(); ++i) {
        uint8_t byte = (input[i].key >> (byteIndex * 8)) & 0xFF;
        count[byte]++;
    }

    for (int i = 1; i < BASE; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = (int)input.Size() - 1; i >= 0; --i) {
        uint8_t byte = (input[i].key >> (byteIndex * 8)) & 0xFF;
        int pos = --count[byte];
        output[pos] = std::move(input[i]);
    }
}

void RadixSort(Vector<Pair> &data) {
    if (data.Size() < 2) {
        return;
    }

    Vector<Pair> buffer(data.Size());

    for (int byte = 0; byte < 4; ++byte) {
        CountingPass(data, buffer, byte);

        for (size_t i = 0; i < data.Size(); ++i) {
            data[i] = std::move(buffer[i]);
        }
    }
}