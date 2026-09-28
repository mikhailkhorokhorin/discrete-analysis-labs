#include <exception>
#include <iostream>

#include "search.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        runSearch(std::cin, std::cout);
    } catch (const std::exception& error) {
        std::cout.flush();
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
