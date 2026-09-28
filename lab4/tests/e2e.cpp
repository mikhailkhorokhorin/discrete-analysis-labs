#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "test_support/process.hpp"

namespace {

class E2eTest : public ::testing::TestWithParam<std::string> {
protected:
    [[nodiscard]] test_support::ProcessResult run(const std::string& input) const {
        const test_support::TempDir workDir;
        return test_support::runProcess(GetParam(), {}, input, workDir.path());
    }
};

TEST_P(E2eTest, MatchesExpectedOutputs) {
    const auto inputs = test_support::inputFiles(TEST_DATA_DIR);
    ASSERT_FALSE(inputs.empty());
    for (const auto& inputPath : inputs) {
        SCOPED_TRACE(inputPath.filename().string());
        auto expectedPath = inputPath;
        expectedPath.replace_extension(".out");
        const auto result = run(test_support::readFile(inputPath));
        EXPECT_EQ(result.exitCode, 0);
        EXPECT_EQ(result.out, test_support::readFile(expectedPath));
    }
}

TEST_P(E2eTest, HandlesCrlfAndWhitespaceOnlyLines) {
    const auto result = run("7 8\r\n  \r\n7\t8 7\r\n \t\n8\r\n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "2, 1\n2, 3\n");
}

TEST_P(E2eTest, WhitespaceOnlyPatternProducesNothing) {
    const auto result = run("  \t\n1 2 3\n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "");
}

TEST_P(E2eTest, ReportsGarbageToken) {
    const auto result = run("1 2\n1 2 x 1 2\n");
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_EQ(result.out, "1, 1\n");
    EXPECT_EQ(result.err, "ERROR: line 1: invalid token 'x'\n");
}

TEST_P(E2eTest, ReportsTokenOverflow) {
    const auto result = run("4294967296\n1\n");
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_EQ(result.err, "ERROR: pattern: invalid token '4294967296'\n");
}

INSTANTIATE_TEST_SUITE_P(Binaries, E2eTest,
                         ::testing::Values(std::string(LAB4_MAIN_PATH),
                                           std::string(LAB4_SOLUTION_PATH)),
                         [](const ::testing::TestParamInfo<std::string>& paramInfo) {
                             return paramInfo.index == 0 ? std::string("Main")
                                                         : std::string("Solution");
                         });

}
