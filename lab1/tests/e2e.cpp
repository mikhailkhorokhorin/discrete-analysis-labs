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
    const auto result = run("2.1.2000\tb\r\n   \r\n\t\n1.1.2000\ta\r\n \n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "1.1.2000\ta\n2.1.2000\tb\n");
}

TEST_P(E2eTest, ReportsInvalidLine) {
    const auto result = run("1.1.2000\ta\n40.1.2000\tb\n");
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_EQ(result.out, "");
    EXPECT_EQ(result.err, "ERROR: line 2: invalid date\n");
}

TEST_P(E2eTest, RejectsTooLongValue) {
    const auto result = run("1.1.2000\t" + std::string(65, 'v') + "\n");
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_EQ(result.err, "ERROR: line 1: value is longer than 64 characters\n");
}

INSTANTIATE_TEST_SUITE_P(Binaries, E2eTest,
                         ::testing::Values(std::string(LAB1_MAIN_PATH),
                                           std::string(LAB1_SOLUTION_PATH)),
                         [](const ::testing::TestParamInfo<std::string>& paramInfo) {
                             return paramInfo.index == 0 ? std::string("Main")
                                                         : std::string("Solution");
                         });

}
