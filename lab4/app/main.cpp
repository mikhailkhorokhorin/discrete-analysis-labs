#include "kmp.hpp"

#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

    if (pattern.empty()) {
        return 0;
    }

    std::vector<int> prefixFunc = BuildPrefixFunction(pattern);
    std::deque<TPosition> positionWindow;
    int matched = 0;
    int patternLen = static_cast<int>(pattern.size());

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
            positionWindow.emplace_back(lineNumber, wordNumber);
            if (static_cast<int>(positionWindow.size()) > patternLen) {
                positionWindow.pop_front();
            }

            while (matched > 0 && token != pattern[matched]) {
                matched = prefixFunc[matched - 1];
            }
            if (token == pattern[matched]) {
                ++matched;
            }
            if (matched == patternLen) {
                const TPosition &match = positionWindow.front();
                std::cout << match.first << ", " << match.second << "\n";
                matched = prefixFunc[matched - 1];
            }

            ++wordNumber;
        }

        ++lineNumber;
    }

    return 0;
}
