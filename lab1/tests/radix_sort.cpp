#include "radix_sort.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "pair.hpp"
#include "vector.hpp"

namespace {

Vector<Pair> makePairs(const std::vector<std::uint32_t>& keys) {
    Vector<Pair> pairs;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        pairs.pushBack(Pair{.raw = std::to_string(i), .key = keys[i]});
    }
    return pairs;
}

std::vector<std::string> rawValues(const Vector<Pair>& pairs) {
    std::vector<std::string> values;
    for (const Pair& pair : pairs) {
        values.push_back(pair.raw);
    }
    return values;
}

TEST(RadixSortTest, HandlesEmptyAndSingleElement) {
    Vector<Pair> empty;
    radixSort(empty);
    EXPECT_TRUE(empty.empty());

    Vector<Pair> single = makePairs({42});
    radixSort(single);
    ASSERT_EQ(single.size(), 1U);
    EXPECT_EQ(single[0].key, 42U);
}

TEST(RadixSortTest, SortsByKeyAndKeepsEqualKeysStable) {
    Vector<Pair> pairs = makePairs({5, 1, 5, 0xFFFFFFFFU, 0, 1, 256});
    radixSort(pairs);
    EXPECT_EQ(rawValues(pairs), (std::vector<std::string>{"4", "1", "5", "0", "2", "6", "3"}));
}

TEST(RadixSortTest, CountingPassSortsBySingleByte) {
    Vector<Pair> input = makePairs({0x0201, 0x0102, 0x0301});
    Vector<Pair> output(input.size());
    countingPass(input, output, 0);
    EXPECT_EQ(rawValues(output), (std::vector<std::string>{"0", "2", "1"}));
    Vector<Pair> second(output.size());
    countingPass(output, second, 1);
    EXPECT_EQ(rawValues(second), (std::vector<std::string>{"1", "0", "2"}));
}

TEST(RadixSortTest, MatchesStableSortOnRandomData) {
    std::mt19937 generator(12345);
    std::uniform_int_distribution<std::uint32_t> distribution(0, 99991231);
    std::vector<std::uint32_t> keys(5000);
    for (std::uint32_t& key : keys) {
        key = distribution(generator) % 1000 * 99991;
    }
    Vector<Pair> pairs = makePairs(keys);
    radixSort(pairs);

    std::vector<std::size_t> order(keys.size());
    for (std::size_t i = 0; i < order.size(); ++i) {
        order[i] = i;
    }
    std::ranges::stable_sort(
        order, [&keys](std::size_t lhs, std::size_t rhs) { return keys[lhs] < keys[rhs]; });
    ASSERT_EQ(pairs.size(), order.size());
    for (std::size_t i = 0; i < order.size(); ++i) {
        EXPECT_EQ(pairs[i].raw, std::to_string(order[i]));
    }
}

}
