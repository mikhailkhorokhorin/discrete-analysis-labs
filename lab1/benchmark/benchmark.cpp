#include "main.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

using duration_t = std::chrono::microseconds;
const std::string DURATION_PREFIX = "us";

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Vector<Pair> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            data.PushBack(Pair(line));
        }
    }

    Vector<Pair> dataSTL = data;

    auto start = std::chrono::high_resolution_clock::now();

    RadixSort(data);

    auto end = std::chrono::high_resolution_clock::now();
    auto radixTime = std::chrono::duration_cast<duration_t>(end - start).count();

    start = std::chrono::high_resolution_clock::now();

    std::stable_sort(&dataSTL[0], &dataSTL[0] + dataSTL.Size(),
                     [](const Pair &a, const Pair &b) { return a.key < b.key; });

    end = std::chrono::high_resolution_clock::now();
    auto stlTime = std::chrono::duration_cast<duration_t>(end - start).count();

    std::cout << "Radix sort time: " << radixTime << DURATION_PREFIX << "\n";
    std::cout << "STL stable sort time: " << stlTime << DURATION_PREFIX << "\n";

    return 0;
}
