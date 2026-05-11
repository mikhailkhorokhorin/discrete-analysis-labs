#include "kmp.hpp"

std::vector<int> BuildPrefixFunction(const std::vector<TToken> &pattern) {
    int patternLen = static_cast<int>(pattern.size());
    std::vector<int> prefixFunc(patternLen, 0);

    for (int i = 1; i < patternLen; ++i) {
        int k = prefixFunc[i - 1];
        while (k > 0 && pattern[i] != pattern[k]) {
            k = prefixFunc[k - 1];
        }
        if (pattern[i] == pattern[k]) {
            ++k;
        }
        prefixFunc[i] = k;
    }

    return prefixFunc;
}

std::vector<TPosition> KmpSearch(
    const std::vector<TToken> &pattern,
    const std::vector<TToken> &text,
    const std::vector<TPosition> &positions
) {
    int patternLen = static_cast<int>(pattern.size());
    int textLen = static_cast<int>(text.size());
    std::vector<TPosition> results;

    if (patternLen == 0 || textLen == 0) {
        return results;
    }

    std::vector<int> prefixFunc = BuildPrefixFunction(pattern);
    int matched = 0;

    for (int i = 0; i < textLen; ++i) {
        while (matched > 0 && text[i] != pattern[matched]) {
            matched = prefixFunc[matched - 1];
        }
        if (text[i] == pattern[matched]) {
            ++matched;
        }
        if (matched == patternLen) {
            int startIndex = i - patternLen + 1;
            results.push_back(positions[startIndex]);
            matched = prefixFunc[matched - 1];
        }
    }

    return results;
}
