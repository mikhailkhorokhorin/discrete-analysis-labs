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
    const auto result = run("+ Word 7\r\n   \r\n\t\nWORD\r\n- word\r\n \nword\r\n");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_EQ(result.out, "OK\nOK: 7\nOK\nNoSuchWord\n");
}

TEST_P(E2eTest, SharesFileFormatAcrossRuns) {
    const test_support::TempDir workDir;
    const auto save = test_support::runProcess(GetParam(), {}, "+ persisted 9\n! Save dict.bin\n",
                                               workDir.path());
    EXPECT_EQ(save.out, "OK\nOK\n");
    const auto load =
        test_support::runProcess(GetParam(), {}, "! Load dict.bin\npersisted\n", workDir.path());
    EXPECT_EQ(load.out, "OK\nOK: 9\n");
}

INSTANTIATE_TEST_SUITE_P(Binaries, E2eTest,
                         ::testing::Values(std::string(LAB2_MAIN_PATH),
                                           std::string(LAB2_SOLUTION_PATH)),
                         [](const ::testing::TestParamInfo<std::string>& paramInfo) {
                             return paramInfo.index == 0 ? std::string("Main")
                                                         : std::string("Solution");
                         });

}
