#include "lcs.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "suffix_tree.hpp"

namespace {

LcsResult bruteForce(const std::string& first, const std::string& second) {
    std::size_t best = 0;
    std::set<std::string> found;
    for (std::size_t i = 0; i < first.size(); ++i) {
        for (std::size_t length = 1; i + length <= first.size(); ++length) {
            const std::string candidate = first.substr(i, length);
            if (second.find(candidate) == std::string::npos) {
                break;
            }
            if (length > best) {
                best = length;
                found.clear();
            }
            if (length == best) {
                found.insert(candidate);
            }
        }
    }
    return LcsResult{.length = best,
                     .substrings = std::vector<std::string>(found.begin(), found.end())};
}

std::string run(const std::string& input) {
    std::istringstream stream(input);
    std::ostringstream output;
    runLcs(stream, output);
    return output.str();
}

TEST(LcsTest, HandlesEmptyStrings) {
    EXPECT_EQ(longestCommonSubstrings("", "abc"), LcsResult{});
    EXPECT_EQ(longestCommonSubstrings("abc", ""), LcsResult{});
    EXPECT_EQ(longestCommonSubstrings("", ""), LcsResult{});
}

TEST(LcsTest, FindsAllSubstringsOfMaximalLength) {
    const LcsResult result = longestCommonSubstrings("abcxyz", "xyzabc");
    EXPECT_EQ(result.length, 3U);
    EXPECT_EQ(result.substrings, (std::vector<std::string>{"abc", "xyz"}));
    EXPECT_EQ(longestCommonSubstrings("same", "same").substrings, std::vector<std::string>{"same"});
}

TEST(LcsTest, HandlesLongRepetitiveInputWithoutRecursion) {
    const std::string first(200000, 'a');
    const std::string second(150000, 'a');
    const LcsResult result = longestCommonSubstrings(first, second);
    EXPECT_EQ(result.length, second.size());
    ASSERT_EQ(result.substrings.size(), 1U);
    EXPECT_EQ(result.substrings.front(), second);
}

TEST(LcsTest, MatchesBruteForceOnRandomStrings) {
    std::mt19937 generator(5);
    std::uniform_int_distribution<int> letter(0, 2);
    std::uniform_int_distribution<std::size_t> length(0, 30);
    for (int round = 0; round < 300; ++round) {
        std::string first(length(generator), 'a');
        std::string second(length(generator), 'a');
        for (char& symbol : first) {
            symbol = static_cast<char>('a' + letter(generator));
        }
        for (char& symbol : second) {
            symbol = static_cast<char>('a' + letter(generator));
        }
        SCOPED_TRACE(first + "|" + second);
        ASSERT_EQ(longestCommonSubstrings(first, second), bruteForce(first, second));
    }
}

TEST(LcsTest, RunLcsPrintsLengthAndSubstrings) {
    EXPECT_EQ(run("xabay\nxabcbay\n"), "3\nbay\nxab\n");
    EXPECT_EQ(run("abc\r\nzbcz\r\n"), "2\nbc\n");
    EXPECT_EQ(run("abc\n"), "0\n");
    EXPECT_EQ(run("abc"), "0\n");
    EXPECT_EQ(run(""), "0\n");
    EXPECT_EQ(run("abc\nxyz\n"), "0\n");
}

}
