#include "pair.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>

namespace {

TEST(PairTest, IsBlankDetectsWhitespaceOnlyLines) {
    EXPECT_TRUE(isBlank(""));
    EXPECT_TRUE(isBlank(" \t\r\v\f\n"));
    EXPECT_FALSE(isBlank(" 1"));
}

TEST(PairTest, ParseDateKeyEncodesDate) {
    EXPECT_EQ(parseDateKey("1.1.1"), 10101U);
    EXPECT_EQ(parseDateKey("01.09.2009"), 20090901U);
    EXPECT_EQ(parseDateKey("31.12.9999"), 99991231U);
    EXPECT_EQ(parseDateKey("1.1.0"), 101U);
    EXPECT_EQ(parseDateKey("1.9.2009"), parseDateKey("01.09.2009"));
}

TEST(PairTest, ParseDateKeyRejectsInvalidDates) {
    for (const char* date :
         {"", "1", "1.1", "0.1.1", "32.1.1", "1.0.1", "1.13.1", "1.1.10000", "a.1.1", "1..1",
          "1.1.", " 1.1.1", "-1.1.1", "+1.1.1", "1.1.99999999999999999999"}) {
        SCOPED_TRACE(date);
        EXPECT_THROW(parseDateKey(date), std::invalid_argument);
    }
}

TEST(PairTest, ParsePairKeepsRawLineWithoutCarriageReturn) {
    const Pair pair = parsePair("01.02.2008\tvalue\r");
    EXPECT_EQ(pair.raw, "01.02.2008\tvalue");
    EXPECT_EQ(pair.key, 20080201U);
}

TEST(PairTest, ParsePairAcceptsEmptyAndMaximalValues) {
    EXPECT_EQ(parsePair("1.1.1\t").raw, "1.1.1\t");
    const std::string value(VALUE_LENGTH, 'x');
    EXPECT_EQ(parsePair("1.1.1\t" + value).raw, "1.1.1\t" + value);
}

TEST(PairTest, ParsePairRejectsMalformedLines) {
    EXPECT_THROW(parsePair("1.1.1 value"), std::invalid_argument);
    EXPECT_THROW(parsePair("1.1.1\t" + std::string(VALUE_LENGTH + 1, 'x')), std::invalid_argument);
    EXPECT_THROW(parsePair("x\tvalue"), std::invalid_argument);
}

TEST(PairTest, ReadPairsSkipsBlankLinesAndStripsCarriageReturns) {
    std::istringstream input("1.1.1\ta\r\n\r\n   \n\t\n2.1.1\tb\n");
    const Vector<Pair> pairs = readPairs(input);
    ASSERT_EQ(pairs.size(), 2U);
    EXPECT_EQ(pairs[0].raw, "1.1.1\ta");
    EXPECT_EQ(pairs[1].raw, "2.1.1\tb");
}

TEST(PairTest, ReadPairsReportsLineNumber) {
    std::istringstream input("1.1.1\ta\n\nbad line\n");
    try {
        readPairs(input);
        FAIL() << "exception expected";
    } catch (const std::invalid_argument& error) {
        EXPECT_EQ(std::string(error.what()), "line 3: missing tab separator");
    }
}

TEST(PairTest, WritePairsPrintsRawLines) {
    Vector<Pair> pairs;
    pairs.pushBack(Pair{.raw = "1.1.1\ta", .key = 1});
    pairs.pushBack(Pair{.raw = "2.1.1\tb", .key = 2});
    std::ostringstream output;
    writePairs(output, pairs);
    EXPECT_EQ(output.str(), "1.1.1\ta\n2.1.1\tb\n");
}

}
