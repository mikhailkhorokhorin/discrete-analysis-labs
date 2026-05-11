#include "main.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

using TDuration = std::chrono::microseconds;
const std::string DURATION_PREFIX = "us";

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TVector<TPair> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            data.PushBack(TPair(line));
        }
    }

    TVector<TPair> dataSTL = data;

    auto start = std::chrono::high_resolution_clock::now();

    RadixSort(data);

    auto end = std::chrono::high_resolution_clock::now();
    long long radixTime = std::chrono::duration_cast<TDuration>(end - start).count();

    start = std::chrono::high_resolution_clock::now();

    std::stable_sort(&dataSTL[0], &dataSTL[0] + dataSTL.Size(),
                     [](const TPair &a, const TPair &b) { return a.key < b.key; });

    end = std::chrono::high_resolution_clock::now();
    long long stlTime = std::chrono::duration_cast<TDuration>(end - start).count();

    std::cout << "Radix sort time: " << radixTime << DURATION_PREFIX << "\n";
    std::cout << "STL stable sort time: " << stlTime << DURATION_PREFIX << "\n";

    return 0;
}
