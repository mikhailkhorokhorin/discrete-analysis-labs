#include <exception>
#include <iostream>

#include "pair.hpp"
#include "radix_sort.hpp"
#include "vector.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        Vector<Pair> pairs = readPairs(std::cin);
        radixSort(pairs);
        writePairs(std::cout, pairs);
    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
