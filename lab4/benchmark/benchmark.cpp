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

long long elapsedSince(Clock::time_point start) {
    return std::chrono::duration_cast<Duration>(Clock::now() - start).count();
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

    auto start = Clock::now();
    const std::vector<Position> kmpResults = kmpSearch(pattern, text, positions);
    const long long kmpTime = elapsedSince(start);

    start = Clock::now();
    const std::vector<Position> naiveResults = naiveSearch(pattern, text, positions);
    const long long naiveTime = elapsedSince(start);

    const bool equal = kmpResults == naiveResults;
    std::cout << "KMP time: " << kmpTime << "us\n";
    std::cout << "Naive search time: " << naiveTime << "us\n";
    std::cout << "KMP matches: " << kmpResults.size() << "\n";
    std::cout << "Naive matches: " << naiveResults.size() << "\n";
    std::cout << "Results equal: " << (equal ? "yes" : "no") << "\n";
    return equal ? 0 : 1;
}
