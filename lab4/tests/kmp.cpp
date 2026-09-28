#include "kmp.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <optional>
#include <random>
#include <vector>

namespace {

std::vector<Position> positionsFor(std::size_t count) {
    std::vector<Position> positions;
    for (std::size_t i = 0; i < count; ++i) {
        positions.push_back(Position{.line = 1, .word = i + 1});
    }
    return positions;
}

std::vector<Position> naiveSearch(const std::vector<Token>& pattern,
                                  const std::vector<Token>& text) {
    std::vector<Position> matches;
    for (std::size_t i = 0; !pattern.empty() && i + pattern.size() <= text.size(); ++i) {
        bool found = true;
        for (std::size_t j = 0; j < pattern.size(); ++j) {
            found = found && text[i + j] == pattern[j];
        }
        if (found) {
            matches.push_back(Position{.line = 1, .word = i + 1});
        }
    }
    return matches;
}

TEST(PrefixFunctionTest, ComputesBorders) {
    EXPECT_EQ(buildPrefixFunction({}), std::vector<std::size_t>{});
    EXPECT_EQ(buildPrefixFunction({1, 2, 1, 2, 1, 3}),
              (std::vector<std::size_t>{0, 0, 1, 2, 3, 0}));
    EXPECT_EQ(buildPrefixFunction({7, 7, 7, 7}), (std::vector<std::size_t>{0, 1, 2, 3}));
    EXPECT_EQ(buildPrefixFunction({1, 1, 2, 1, 1, 1}),
              (std::vector<std::size_t>{0, 1, 0, 1, 2, 2}));
}

TEST(KmpMatcherTest, ReportsStartPositionOfEachMatch) {
    KmpMatcher matcher({11, 45, 11, 45, 90});
    const std::vector<Token> text = {11, 45, 11, 45, 11, 45, 90, 11, 45, 11, 45, 90};
    std::vector<Position> matches;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (const auto match = matcher.feed(text[i], Position{.line = 1, .word = i + 1})) {
            matches.push_back(*match);
        }
    }
    EXPECT_EQ(matches, (std::vector<Position>{{1, 3}, {1, 8}}));
}

TEST(KmpMatcherTest, FindsOverlappingMatches) {
    const std::vector<Token> text(6, 5);
    EXPECT_EQ(kmpSearch({5, 5}, text, positionsFor(text.size())).size(), 5U);
}

TEST(KmpMatcherTest, EmptyPatternNeverMatches) {
    KmpMatcher matcher({});
    EXPECT_FALSE(matcher.feed(1, Position{}).has_value());
    EXPECT_TRUE(kmpSearch({}, {1, 2}, positionsFor(2)).empty());
}

TEST(KmpMatcherTest, ResetForgetsPartialMatch) {
    KmpMatcher matcher({1, 2});
    EXPECT_EQ(matcher.pattern(), (std::vector<Token>{1, 2}));
    EXPECT_FALSE(matcher.feed(1, Position{.line = 1, .word = 1}).has_value());
    matcher.reset();
    EXPECT_FALSE(matcher.feed(2, Position{.line = 1, .word = 2}).has_value());
    EXPECT_FALSE(matcher.feed(1, Position{.line = 2, .word = 1}).has_value());
    const std::optional<Position> match = matcher.feed(2, Position{.line = 2, .word = 2});
    ASSERT_TRUE(match.has_value());
    EXPECT_EQ(*match, (Position{.line = 2, .word = 1}));
}

TEST(KmpMatcherTest, HandlesMaximalTokens) {
    const Token maxToken = 4294967295U;
    EXPECT_EQ(kmpSearch({maxToken, 0}, {0, maxToken, 0, maxToken}, positionsFor(4)),
              (std::vector<Position>{{1, 2}}));
}

TEST(KmpMatcherTest, MatchesNaiveSearchOnRandomData) {
    std::mt19937 generator(99);
    std::uniform_int_distribution<Token> tokenDistribution(0, 2);
    std::uniform_int_distribution<std::size_t> lengthDistribution(1, 5);
    for (int round = 0; round < 200; ++round) {
        std::vector<Token> pattern(lengthDistribution(generator));
        for (Token& token : pattern) {
            token = tokenDistribution(generator);
        }
        std::vector<Token> text(200);
        for (Token& token : text) {
            token = tokenDistribution(generator);
        }
        ASSERT_EQ(kmpSearch(pattern, text, positionsFor(text.size())), naiveSearch(pattern, text));
    }
}

}
