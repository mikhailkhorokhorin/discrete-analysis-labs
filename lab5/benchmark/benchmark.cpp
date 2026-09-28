#include <chrono>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "lcs.hpp"
#include "suffix_tree.hpp"

namespace {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::microseconds;

constexpr std::size_t DP_CELL_LIMIT = 25000000;

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
}

std::string readLine(std::istream& input) {
    std::string line;
    if (std::getline(input, line) && !line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

LcsResult dynamicProgrammingLcs(const std::string& first, const std::string& second) {
    std::size_t best = 0;
    std::set<std::string> found;
    std::vector<std::size_t> previous(second.size() + 1, 0);
    std::vector<std::size_t> current(second.size() + 1, 0);
    for (std::size_t i = 1; i <= first.size(); ++i) {
        for (std::size_t j = 1; j <= second.size(); ++j) {
            current[j] = first[i - 1] == second[j - 1] ? previous[j - 1] + 1 : 0;
            if (current[j] == 0 || current[j] < best) {
                continue;
            }
            if (current[j] > best) {
                best = current[j];
                found.clear();
            }
            found.insert(first.substr(i - best, best));
        }
        previous.swap(current);
    }
    return LcsResult{.length = best,
                     .substrings = std::vector<std::string>(found.begin(), found.end())};
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const std::string first = readLine(std::cin);
    const std::string second = readLine(std::cin);

    auto start = Clock::now();
    const LcsResult tree = longestCommonSubstrings(first, second);
    const long long treeTime = elapsedSince(start);

    std::cout << "Suffix tree LCS time: " << treeTime << "us\n";
    std::cout << "LCS length: " << tree.length << "\n";
    std::cout << "LCS count: " << tree.substrings.size() << "\n";

    if ((first.size() + 1) * (second.size() + 1) > DP_CELL_LIMIT) {
        std::cout << "DP LCS skipped: input is too large\n";
        return 0;
    }

    start = Clock::now();
    const LcsResult dynamic = dynamicProgrammingLcs(first, second);
    const long long dynamicTime = elapsedSince(start);

    const bool equal = tree == dynamic;
    std::cout << "DP LCS time: " << dynamicTime << "us\n";
    std::cout << "DP LCS length: " << dynamic.length << "\n";
    std::cout << "DP LCS count: " << dynamic.substrings.size() << "\n";
    std::cout << "Results equal: " << (equal ? "yes" : "no") << "\n";
    return equal ? 0 : 1;
}
