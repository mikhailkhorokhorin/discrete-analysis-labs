#include "kmp.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using TDuration = std::chrono::microseconds;
const std::string DURATION_SUFFIX = "us";

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<TToken> pattern;
    std::string patternLine;

    if (!std::getline(std::cin, patternLine)) {
        return 0;
    }

    if (!patternLine.empty() && patternLine.back() == '\r') {
        patternLine.pop_back();
    }

    {
        std::istringstream ss(patternLine);
        TToken token;
        while (ss >> token) {
            pattern.push_back(token);
        }
    }

    std::vector<TToken> text;
    std::vector<TPosition> positions;

    std::string line;
    int lineNumber = 1;

    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::istringstream ss(line);
        TToken token;
        int wordNumber = 1;

        while (ss >> token) {
            text.push_back(token);
            positions.emplace_back(lineNumber, wordNumber);
            ++wordNumber;
        }

        ++lineNumber;
    }

    std::vector<TPosition> kmpResults;

    auto start = std::chrono::high_resolution_clock::now();

    kmpResults = KmpSearch(pattern, text, positions);

    auto end = std::chrono::high_resolution_clock::now();
    long long kmpTime = std::chrono::duration_cast<TDuration>(end - start).count();

    std::vector<TPosition> naiveResults;

    start = std::chrono::high_resolution_clock::now();

    int patternLen = static_cast<int>(pattern.size());
    int textLen = static_cast<int>(text.size());

    if (patternLen > 0) {
        for (int i = 0; i <= textLen - patternLen; ++i) {
            bool found = true;
            for (int j = 0; j < patternLen && found; ++j) {
                if (text[i + j] != pattern[j]) {
                    found = false;
                }
            }
            if (found) {
                naiveResults.push_back(positions[i]);
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    long long naiveTime = std::chrono::duration_cast<TDuration>(end - start).count();

    std::cout << "KMP time: " << kmpTime << DURATION_SUFFIX << "\n";
    std::cout << "Naive search time: " << naiveTime << DURATION_SUFFIX << "\n";
    std::cout << "KMP matches: " << kmpResults.size() << "\n";
    std::cout << "Naive matches: " << naiveResults.size() << "\n";
    std::cout << "Results equal: " << (kmpResults == naiveResults ? "yes" : "no") << "\n";

    return kmpResults == naiveResults ? 0 : 1;
}
