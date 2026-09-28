#include "search.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "kmp.hpp"

namespace {

std::string search(const std::string& input) {
    std::istringstream stream(input);
    std::ostringstream output;
    runSearch(stream, output);
    return output.str();
}

std::string errorOf(const std::string& input) {
    try {
        search(input);
    } catch (const std::invalid_argument& error) {
        return error.what();
    }
    return "";
}

TEST(ParseTokensTest, ParsesNumbersSeparatedByWhitespace) {
    EXPECT_EQ(parseTokens(""), std::vector<Token>{});
    EXPECT_EQ(parseTokens(" \t \r"), std::vector<Token>{});
    EXPECT_EQ(parseTokens("0011 45\t011  4294967295\r"),
              (std::vector<Token>{11, 45, 11, 4294967295U}));
}

TEST(ParseTokensTest, RejectsGarbage) {
    for (const char* line : {"12a", "abc", "-1", "+1", "4294967296", "1,2"}) {
        SCOPED_TRACE(line);
        EXPECT_THROW(parseTokens(line), std::invalid_argument);
    }
}

TEST(RunSearchTest, FindsStatementExample) {
    EXPECT_EQ(search("11 45 11 45 90\n0011 45 011 0045 11 45 90    11\n45 11 45 90\n"),
              "1, 3\n1, 8\n");
}

TEST(RunSearchTest, HandlesEmptyInputsAndBlankLines) {
    EXPECT_EQ(search(""), "");
    EXPECT_EQ(search("\n1 2 3\n"), "");
    EXPECT_EQ(search("1 2\n\n  \n1\n\n2\n1 2\r\n"), "3, 1\n6, 1\n");
}

TEST(RunSearchTest, ReportsInvalidTokens) {
    EXPECT_EQ(errorOf("1 x\n1\n"), "pattern: invalid token 'x'");
    EXPECT_EQ(errorOf("1\n1 2\n3 4a 5\n"), "line 2: invalid token '4a'");
    EXPECT_EQ(errorOf("1\n1\n"), "");
}

}
