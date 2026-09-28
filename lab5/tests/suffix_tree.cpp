#include "suffix_tree.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {

TEST(SuffixTreeTest, BuildsCombinedTextWithSeparators) {
    EXPECT_EQ(buildCombinedText("ab", "c"),
              (std::vector<int>{'a', 'b', FIRST_SEPARATOR, 'c', SECOND_SEPARATOR}));
    EXPECT_EQ(buildCombinedText("", ""), (std::vector<int>{FIRST_SEPARATOR, SECOND_SEPARATOR}));
    EXPECT_EQ(buildCombinedText("\xff", "")[0], 255);
}

TEST(SuffixTreeTest, HasLinearNumberOfNodes) {
    const std::string first(1000, 'a');
    const SuffixTree tree(buildCombinedText(first, first));
    EXPECT_LE(tree.nodeCount(), 2 * (2 * first.size() + 2) + 1);
}

TEST(SuffixTreeTest, FindsStatementExample) {
    const SuffixTree tree(buildCombinedText("xabay", "xabcbay"));
    const LcsResult result = tree.findLcs(5);
    EXPECT_EQ(result.length, 3U);
    EXPECT_EQ(result.substrings, (std::vector<std::string>{"bay", "xab"}));
}

TEST(SuffixTreeTest, ReturnsEmptyResultWithoutCommonSymbols) {
    const SuffixTree tree(buildCombinedText("abc", "xyz"));
    EXPECT_EQ(tree.findLcs(3), LcsResult{});
}

}
