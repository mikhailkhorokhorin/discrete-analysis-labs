#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
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
