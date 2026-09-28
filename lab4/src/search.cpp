#include "search.hpp"

#include <charconv>
#include <cstddef>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "kmp.hpp"

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
