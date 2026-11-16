#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "kmp.hpp"
#include "search.hpp"

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

std::vector<Position> naiveSearch(const std::vector<Token>& pattern, const std::vector<Token>& text,
                                  const std::vector<Position>& positions) {
    std::vector<Position> matches;
    if (pattern.empty() || text.size() < pattern.size()) {
        return matches;
    }
    for (std::size_t i = 0; i + pattern.size() <= text.size(); ++i) {
        bool found = true;
        for (std::size_t j = 0; j < pattern.size() && found; ++j) {
            found = text[i + j] == pattern[j];
        }
        if (found) {
            matches.push_back(positions[i]);
        }
    }
    return matches;
}

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    if (!std::getline(std::cin, line)) {
        return 0;
    }
    const std::vector<Token> pattern = parseTokens(line);

    std::vector<Token> text;
    std::vector<Position> positions;
    std::size_t lineNumber = 0;
    while (std::getline(std::cin, line)) {
        ++lineNumber;
        std::size_t wordNumber = 0;
        for (const Token token : parseTokens(line)) {
            text.push_back(token);
            positions.push_back(Position{.line = lineNumber, .word = ++wordNumber});
        }
    }

    Timings kmpTimes{};
    Timings naiveTimes{};
    std::vector<Position> kmpResults;
    std::vector<Position> naiveResults;
    for (std::size_t run = 0; run < RUNS; ++run) {
        auto start = Clock::now();
        kmpResults = kmpSearch(pattern, text, positions);
        kmpTimes[run] = elapsedSince(start);

        start = Clock::now();
        naiveResults = naiveSearch(pattern, text, positions);
        naiveTimes[run] = elapsedSince(start);
    }

    const bool equal = kmpResults == naiveResults;
    std::cout << "KMP time: " << median(kmpTimes) << "us\n";
    std::cout << "Naive search time: " << median(naiveTimes) << "us\n";
    std::cout << "KMP matches: " << kmpResults.size() << "\n";
    std::cout << "Naive matches: " << naiveResults.size() << "\n";
    std::cout << "Results equal: " << (equal ? "yes" : "no") << "\n";
    return equal ? 0 : 1;
}
