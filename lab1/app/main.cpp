#include "main.hpp"

#include <iostream>
#include <string>

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

    RadixSort(data);

    for (size_t i = 0; i < data.Size(); ++i) {
        std::cout << data[i].raw << "\n";
    }

    return 0;
}
