#include <charconv>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using Token = std::uint32_t;

struct Position {
    std::size_t line = 0;
    std::size_t word = 0;

    bool operator==(const Position& other) const = default;
};

std::vector<std::size_t> buildPrefixFunction(const std::vector<Token>& pattern);

class KmpMatcher {
public:
    explicit KmpMatcher(std::vector<Token> pattern);

    std::optional<Position> feed(Token token, Position position);
    void reset();

    [[nodiscard]] const std::vector<Token>& pattern() const { return pattern_; }

private:
    std::vector<Token> pattern_;
    std::vector<std::size_t> prefix_;
    std::vector<Position> window_;
    std::size_t fed_ = 0;
    std::size_t matched_ = 0;
};

std::vector<Position> kmpSearch(const std::vector<Token>& pattern, const std::vector<Token>& text,
                                const std::vector<Position>& positions);

std::vector<Token> parseTokens(std::string_view line);
void runSearch(std::istream& input, std::ostream& output);

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

namespace {

bool isSpace(char symbol) {
    return symbol == ' ' || symbol == '\t' || symbol == '\r' || symbol == '\n' || symbol == '\v' ||
           symbol == '\f';
}

Token parseToken(std::string_view text) {
    Token value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::invalid_argument("invalid token '" + std::string(text) + "'");
    }
    return value;
}

template <typename Callback>
void forEachToken(std::string_view line, const Callback& callback) {
    std::size_t position = 0;
    while (position < line.size()) {
        while (position < line.size() && isSpace(line[position])) {
            ++position;
        }
        const std::size_t start = position;
        while (position < line.size() && !isSpace(line[position])) {
            ++position;
        }
        if (position > start) {
            callback(parseToken(line.substr(start, position - start)));
        }
    }
}

}

std::vector<Token> parseTokens(std::string_view line) {
    std::vector<Token> tokens;
    forEachToken(line, [&tokens](Token token) { tokens.push_back(token); });
    return tokens;
}

void runSearch(std::istream& input, std::ostream& output) {
    std::string line;
    if (!std::getline(input, line)) {
        return;
    }
    std::vector<Token> pattern;
    try {
        pattern = parseTokens(line);
    } catch (const std::invalid_argument& error) {
        throw std::invalid_argument(std::string("pattern: ") + error.what());
    }
    if (pattern.empty()) {
        return;
    }

    KmpMatcher matcher(std::move(pattern));
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        std::size_t wordNumber = 0;
        try {
            forEachToken(line, [&](Token token) {
                ++wordNumber;
                if (const std::optional<Position> match =
                        matcher.feed(token, Position{.line = lineNumber, .word = wordNumber})) {
                    output << match->line << ", " << match->word << '\n';
                }
            });
        } catch (const std::invalid_argument& error) {
            throw std::invalid_argument("line " + std::to_string(lineNumber) + ": " + error.what());
        }
    }
}

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
