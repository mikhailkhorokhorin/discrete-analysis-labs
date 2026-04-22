#include "main.hpp"

#include <iostream>
#include <string>

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

    RadixSort(data);

    std::cout << data;

    return 0;
}
