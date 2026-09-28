#include <algorithm>
#include <chrono>
#include <iostream>

#include "pair.hpp"
#include "radix_sort.hpp"
#include "vector.hpp"

namespace {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::microseconds;

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Vector<Pair> data = readPairs(std::cin);
    Vector<Pair> dataStl = data;

    auto start = Clock::now();
    radixSort(data);
    const long long radixTime = elapsedSince(start);

    start = Clock::now();
    if (!dataStl.empty()) {
        std::stable_sort(dataStl.data(), dataStl.data() + dataStl.size(),
                         [](const Pair& lhs, const Pair& rhs) { return lhs.key < rhs.key; });
    }
    const long long stlTime = elapsedSince(start);

    std::cout << "Radix sort time: " << radixTime << "us\n";
    std::cout << "STL stable sort time: " << stlTime << "us\n";
    return 0;
}
