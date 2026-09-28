#include "radix_sort.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace {

constexpr std::size_t BYTE_BASE = 256;
constexpr std::uint32_t BYTE_MASK = 0xFF;
constexpr int KEY_BYTES = 4;
constexpr int BITS_PER_BYTE = 8;

std::size_t byteAt(std::uint32_t key, int byteIndex) {
    return (key >> (byteIndex * BITS_PER_BYTE)) & BYTE_MASK;
}

}

void countingPass(Vector<Pair>& input, Vector<Pair>& output, int byteIndex) {
    std::array<std::size_t, BYTE_BASE> count{};
    for (const Pair& pair : input) {
        ++count[byteAt(pair.key, byteIndex)];
    }
    for (std::size_t i = 1; i < BYTE_BASE; ++i) {
        count[i] += count[i - 1];
    }
    for (std::size_t i = input.size(); i > 0; --i) {
        Pair& pair = input[i - 1];
        const std::size_t position = --count[byteAt(pair.key, byteIndex)];
        output[position] = std::move(pair);
    }
}

void radixSort(Vector<Pair>& data) {
    if (data.size() < 2) {
        return;
    }
    Vector<Pair> buffer(data.size());
    for (int byteIndex = 0; byteIndex < KEY_BYTES; ++byteIndex) {
        countingPass(data, buffer, byteIndex);
        data.swap(buffer);
    }
}
