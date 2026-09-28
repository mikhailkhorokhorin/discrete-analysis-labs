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

TEST_P(E2eTest, HandlesCrlfLines) {
    const auto result = run("abc\r\nzbcz\r\n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "2\nbc\n");
}

TEST_P(E2eTest, TreatsWhitespaceAsRegularSymbols) {
    const auto result = run("a   b\n  \n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "2\n  \n");
}

TEST_P(E2eTest, TreatsMissingSecondLineAsEmpty) {
    const auto result = run("abc");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "0\n");
}

TEST_P(E2eTest, SurvivesLongRepetitiveInput) {
    const std::string first(100000, 'a');
    const std::string second(80000, 'a');
    const auto result = run(first + "\n" + second + "\n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "80000\n" + second + "\n");
}

INSTANTIATE_TEST_SUITE_P(Binaries, E2eTest,
                         ::testing::Values(std::string(LAB5_MAIN_PATH),
                                           std::string(LAB5_SOLUTION_PATH)),
                         [](const ::testing::TestParamInfo<std::string>& paramInfo) {
                             return paramInfo.index == 0 ? std::string("Main")
                                                         : std::string("Solution");
                         });

}
