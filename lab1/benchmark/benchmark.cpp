#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>

#include "pair.hpp"
#include "radix_sort.hpp"
#include "vector.hpp"

namespace {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::microseconds;

constexpr std::size_t RUNS = 5;

using Timings = std::array<long long, RUNS>;

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
}

long long median(Timings timings) {
    std::ranges::sort(timings);
    return timings[RUNS / 2];
}

bool sameOrder(const Vector<Pair>& lhs, const Vector<Pair>& rhs) {
    return std::ranges::equal(
        lhs, rhs, [](const Pair& left, const Pair& right) { return left.raw == right.raw; });
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const Vector<Pair> data = readPairs(std::cin);

    Timings radixTimes{};
    Timings stlTimes{};
    bool equal = true;
    for (std::size_t run = 0; run < RUNS; ++run) {
        Vector<Pair> radixData = data;
        auto start = Clock::now();
        radixSort(radixData);
        radixTimes[run] = elapsedSince(start);

        Vector<Pair> stlData = data;
        start = Clock::now();
        std::ranges::stable_sort(
            stlData, [](const Pair& lhs, const Pair& rhs) { return lhs.key < rhs.key; });
        stlTimes[run] = elapsedSince(start);

        equal = equal && sameOrder(radixData, stlData);
    }

    std::cout << "Radix sort time: " << median(radixTimes) << "us\n";
    std::cout << "STL stable sort time: " << median(stlTimes) << "us\n";
    std::cout << "Results equal: " << (equal ? "yes" : "no") << "\n";
    return equal ? 0 : 1;
}
