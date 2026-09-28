#include "dictionary.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "patricia_trie.hpp"
#include "test_support/process.hpp"

namespace {

TEST(DictionaryTest, IgnoresBlankLines) {
    Dictionary dictionary;
    EXPECT_FALSE(dictionary.execute("").has_value());
    EXPECT_FALSE(dictionary.execute(" \t\r").has_value());
}

TEST(DictionaryTest, AddsFindsAndRemovesCaseInsensitively) {
    Dictionary dictionary;
    EXPECT_EQ(dictionary.execute("+ Word 34"), "OK");
    EXPECT_EQ(dictionary.execute("+ wORD 1"), "Exist");
    EXPECT_EQ(dictionary.execute("WORD"), "OK: 34");
    EXPECT_EQ(dictionary.execute("  word\t\r"), "OK: 34");
    EXPECT_EQ(dictionary.execute("- word"), "OK");
    EXPECT_EQ(dictionary.execute("- word"), "NoSuchWord");
    EXPECT_EQ(dictionary.execute("word"), "NoSuchWord");
    EXPECT_EQ(dictionary.execute("+ max 18446744073709551615"), "OK");
    EXPECT_EQ(dictionary.execute("max"), "OK: 18446744073709551615");
}

TEST(DictionaryTest, ReportsInvalidCommands) {
    Dictionary dictionary;
    EXPECT_EQ(dictionary.execute("+ word"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("+ word 1 2"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("-"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("- a b"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("! Save"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("! Load a b"), "ERROR: invalid command");
    EXPECT_EQ(dictionary.execute("!"), "ERROR: unknown command");
    EXPECT_EQ(dictionary.execute("! Print file"), "ERROR: unknown command");
    EXPECT_EQ(dictionary.execute("? word"), "ERROR: unknown command");
    EXPECT_EQ(dictionary.execute("word extra"), "ERROR: unknown command");
}

TEST(DictionaryTest, ValidatesKeysAndValues) {
    Dictionary dictionary;
    EXPECT_EQ(dictionary.execute("+ w0rd 1"), "ERROR: invalid key");
    EXPECT_EQ(dictionary.execute("- w0rd"), "ERROR: invalid key");
    EXPECT_EQ(dictionary.execute("w0rd"), "ERROR: invalid key");
    EXPECT_EQ(dictionary.execute("+ " + std::string(MAX_KEY_LENGTH + 1, 'a') + " 1"),
              "ERROR: invalid key");
    EXPECT_EQ(dictionary.execute("+ " + std::string(MAX_KEY_LENGTH, 'a') + " 1"), "OK");
    EXPECT_EQ(dictionary.execute("+ word -1"), "ERROR: invalid value");
    EXPECT_EQ(dictionary.execute("+ word +1"), "ERROR: invalid value");
    EXPECT_EQ(dictionary.execute("+ word 1x"), "ERROR: invalid value");
    EXPECT_EQ(dictionary.execute("+ word 18446744073709551616"), "ERROR: invalid value");
    EXPECT_EQ(dictionary.execute("word"), "NoSuchWord");
}

TEST(DictionaryTest, SavesAndLoadsFiles) {
    const test_support::TempDir dir;
    const std::string path = (dir.path() / "dict.bin").string();
    Dictionary dictionary;
    EXPECT_EQ(dictionary.execute("+ word 5"), "OK");
    EXPECT_EQ(dictionary.execute("! Save " + path), "OK");
    EXPECT_EQ(dictionary.execute("- word"), "OK");
    EXPECT_EQ(dictionary.execute("! Load " + path), "OK");
    EXPECT_EQ(dictionary.execute("word"), "OK: 5");
    EXPECT_EQ(dictionary.execute("! Load " + path + ".missing"),
              "ERROR: cannot open file for reading");
    EXPECT_EQ(dictionary.execute("! Save " + dir.path().string()),
              "ERROR: cannot open file for writing");
    EXPECT_EQ(dictionary.execute("word"), "OK: 5");
}

TEST(DictionaryTest, RunDictionaryProcessesStream) {
    std::istringstream input("+ a 1\r\n\n   \nA\n- a\na\n");
    std::ostringstream output;
    runDictionary(input, output);
    EXPECT_EQ(output.str(), "OK\nOK: 1\nOK\nNoSuchWord\n");
}

}
