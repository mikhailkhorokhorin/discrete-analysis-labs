#include "kmp.hpp"

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

std::vector<std::size_t> buildPrefixFunction(const std::vector<Token>& pattern) {
    std::vector<std::size_t> prefix(pattern.size(), 0);
    for (std::size_t i = 1; i < pattern.size(); ++i) {
        std::size_t length = prefix[i - 1];
        while (length > 0 && pattern[i] != pattern[length]) {
            length = prefix[length - 1];
        }
        if (pattern[i] == pattern[length]) {
            ++length;
        }
        prefix[i] = length;
    }
    return prefix;
}

KmpMatcher::KmpMatcher(std::vector<Token> pattern)
    : pattern_(std::move(pattern)),
      prefix_(buildPrefixFunction(pattern_)),
      window_(pattern_.size()) {
}

std::optional<Position> KmpMatcher::feed(Token token, Position position) {
    if (pattern_.empty()) {
        return std::nullopt;
    }
    window_[fed_ % pattern_.size()] = position;
    ++fed_;

    while (matched_ > 0 && token != pattern_[matched_]) {
        matched_ = prefix_[matched_ - 1];
    }
    if (token == pattern_[matched_]) {
        ++matched_;
    }
    if (matched_ < pattern_.size()) {
        return std::nullopt;
    }
    matched_ = prefix_[matched_ - 1];
    return window_[fed_ % pattern_.size()];
}

void KmpMatcher::reset() {
    fed_ = 0;
    matched_ = 0;
}

std::vector<Position> kmpSearch(const std::vector<Token>& pattern, const std::vector<Token>& text,
                                const std::vector<Position>& positions) {
    std::vector<Position> matches;
    KmpMatcher matcher(pattern);
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (const auto match = matcher.feed(text[i], positions[i])) {
            matches.push_back(*match);
        }
    }
    return matches;
}
