#include "patricia_trie.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <utility>

#include "test_support/process.hpp"

namespace {

std::string saved(const PatriciaTrie& trie) {
    std::ostringstream output;
    trie.save(output);
    return output.str();
}

PatriciaTrie loaded(const std::string& bytes) {
    std::istringstream input(bytes);
    return PatriciaTrie::load(input);
}

std::string header(std::uint32_t version, std::uint64_t count) {
    std::string bytes = "PATRICIA";
    for (int i = 0; i < 4; ++i) {
        bytes.push_back(static_cast<char>((version >> (8 * i)) & 0xFFU));
    }
    for (int i = 0; i < 8; ++i) {
        bytes.push_back(static_cast<char>((count >> (8 * i)) & 0xFFU));
    }
    return bytes;
}

std::string entry(const std::string& key, std::uint64_t value) {
    std::string bytes;
    bytes.push_back(static_cast<char>(key.size() & 0xFFU));
    bytes.push_back(static_cast<char>((key.size() >> 8) & 0xFFU));
    bytes += key;
    for (int i = 0; i < 8; ++i) {
        bytes.push_back(static_cast<char>((value >> (8 * i)) & 0xFFU));
    }
    return bytes;
}

void expectLoadError(const std::string& bytes, const std::string& message) {
    try {
        loaded(bytes);
        FAIL() << "exception expected";
    } catch (const PatriciaError& error) {
        EXPECT_EQ(std::string(error.what()), message);
    }
}

TEST(KeyTest, ValidatesAlphabetAndLength) {
    EXPECT_TRUE(isValidKey("a"));
    EXPECT_TRUE(isValidKey("AbZz"));
    EXPECT_TRUE(isValidKey(std::string(MAX_KEY_LENGTH, 'q')));
    EXPECT_FALSE(isValidKey(""));
    EXPECT_FALSE(isValidKey(std::string(MAX_KEY_LENGTH + 1, 'q')));
    EXPECT_FALSE(isValidKey("ab1"));
    EXPECT_FALSE(isValidKey("a-b"));
    EXPECT_FALSE(isValidKey("?"));
}

TEST(KeyTest, NormalizesToLowerCase) {
    EXPECT_EQ(normalizeKey("AbCxYz"), "abcxyz");
    EXPECT_EQ(normalizeKey("abc"), "abc");
}

TEST(PatriciaTrieTest, EmptyTrieFindsNothing) {
    PatriciaTrie trie;
    EXPECT_TRUE(trie.empty());
    EXPECT_FALSE(trie.find("a").has_value());
    EXPECT_FALSE(trie.remove("a"));
}

TEST(PatriciaTrieTest, InsertFindAndDuplicate) {
    PatriciaTrie trie;
    EXPECT_TRUE(trie.insert("a", 1));
    EXPECT_FALSE(trie.insert("a", 2));
    EXPECT_TRUE(trie.insert("ab", 3));
    EXPECT_TRUE(trie.insert("b", UINT64_MAX));
    EXPECT_EQ(trie.size(), 3U);
    EXPECT_EQ(trie.find("a"), 1U);
    EXPECT_EQ(trie.find("ab"), 3U);
    EXPECT_EQ(trie.find("b"), UINT64_MAX);
    EXPECT_FALSE(trie.find("abc").has_value());
    EXPECT_FALSE(trie.find("c").has_value());
}

TEST(PatriciaTrieTest, RemovesHeaderAndInnerNodes) {
    PatriciaTrie trie;
    for (const char* key : {"m", "c", "x", "a", "e", "z"}) {
        ASSERT_TRUE(trie.insert(key, key[0]));
    }
    EXPECT_TRUE(trie.remove("m"));
    EXPECT_FALSE(trie.remove("m"));
    EXPECT_FALSE(trie.find("m").has_value());
    for (const char* key : {"c", "x", "a", "e", "z"}) {
        EXPECT_EQ(trie.find(key), static_cast<std::uint64_t>(key[0]));
    }
    for (const char* key : {"c", "x", "a", "e", "z"}) {
        EXPECT_TRUE(trie.remove(key));
    }
    EXPECT_TRUE(trie.empty());
    EXPECT_TRUE(trie.insert("again", 7));
    EXPECT_EQ(trie.find("again"), 7U);
}

TEST(PatriciaTrieTest, MatchesStdMapOnRandomOperations) {
    std::mt19937 generator(2024);
    std::uniform_int_distribution<int> lengthDistribution(1, 6);
    std::uniform_int_distribution<int> letterDistribution(0, 2);
    std::uniform_int_distribution<int> operationDistribution(0, 2);
    std::uniform_int_distribution<std::uint64_t> valueDistribution;

    PatriciaTrie trie;
    std::map<std::string, std::uint64_t> model;
    for (int step = 0; step < 20000; ++step) {
        std::string key(static_cast<std::size_t>(lengthDistribution(generator)), 'a');
        for (char& symbol : key) {
            symbol = static_cast<char>('a' + letterDistribution(generator));
        }
        const int operation = operationDistribution(generator);
        if (operation == 0) {
            const std::uint64_t value = valueDistribution(generator);
            ASSERT_EQ(trie.insert(key, value), model.emplace(key, value).second);
        } else if (operation == 1) {
            ASSERT_EQ(trie.remove(key), model.erase(key) == 1);
        } else {
            const auto iterator = model.find(key);
            if (iterator == model.end()) {
                ASSERT_FALSE(trie.find(key).has_value());
            } else {
                ASSERT_EQ(trie.find(key), iterator->second);
            }
        }
        ASSERT_EQ(trie.size(), model.size());
        if (step % 500 == 0) {
            for (const auto& [storedKey, value] : model) {
                ASSERT_EQ(trie.find(storedKey), value);
            }
        }
    }
    for (const auto& [storedKey, value] : model) {
        ASSERT_EQ(trie.find(storedKey), value);
        ASSERT_TRUE(trie.remove(storedKey));
    }
    EXPECT_TRUE(trie.empty());
}

TEST(PatriciaTrieTest, HandlesMaximalKeys) {
    PatriciaTrie trie;
    const std::string longKey(MAX_KEY_LENGTH, 'a');
    std::string otherKey = longKey;
    otherKey.back() = 'b';
    EXPECT_TRUE(trie.insert(longKey, 1));
    EXPECT_TRUE(trie.insert(otherKey, 2));
    EXPECT_EQ(trie.find(longKey), 1U);
    EXPECT_EQ(trie.find(otherKey), 2U);
    PatriciaTrie copy = loaded(saved(trie));
    EXPECT_EQ(copy.find(otherKey), 2U);
}

TEST(PatriciaTrieTest, MoveTransfersContents) {
    PatriciaTrie source;
    source.insert("key", 5);
    PatriciaTrie moved(std::move(source));
    EXPECT_EQ(moved.find("key"), 5U);

    PatriciaTrie target;
    target.insert("other", 1);
    target = std::move(moved);
    EXPECT_EQ(target.find("key"), 5U);
    EXPECT_FALSE(target.find("other").has_value());

    PatriciaTrie& alias = target;
    target = std::move(alias);
    EXPECT_EQ(target.size(), 1U);
}

TEST(PatriciaTrieTest, SaveAndLoadRoundTrip) {
    PatriciaTrie trie;
    for (int i = 0; i < 200; ++i) {
        std::string key;
        for (int value = i + 1; value > 0; value /= 26) {
            key.push_back(static_cast<char>('a' + value % 26));
        }
        trie.insert(key, static_cast<std::uint64_t>(i) * 1000003U);
    }
    const PatriciaTrie copy = loaded(saved(trie));
    EXPECT_EQ(copy.size(), trie.size());
    EXPECT_EQ(saved(copy).size(), saved(trie).size());
    EXPECT_EQ(copy.find("b"), 0U);

    const PatriciaTrie empty = loaded(saved(PatriciaTrie()));
    EXPECT_TRUE(empty.empty());
}

TEST(PatriciaTrieTest, LoadRejectsMalformedFiles) {
    expectLoadError("", "invalid file format");
    expectLoadError("NOTATRIE" + header(1, 0).substr(8), "invalid file format");
    expectLoadError(header(2, 0), "unsupported file version");
    expectLoadError(header(1, 0).substr(0, 14), "invalid file format");
    expectLoadError(header(1, 1), "invalid file format");
    expectLoadError(header(1, 1) + entry("", 1), "invalid file format");
    expectLoadError(header(1, 1) + entry(std::string(MAX_KEY_LENGTH + 1, 'a'), 1),
                    "invalid file format");
    expectLoadError(header(1, 1) + entry("Upper", 1), "invalid file format");
    expectLoadError(header(1, 1) + entry("dig1t", 1), "invalid file format");
    expectLoadError(header(1, 1) + entry("word", 1).substr(0, 5), "invalid file format");
    expectLoadError(header(1, 1) + entry("word", 1).substr(0, 8), "invalid file format");
    expectLoadError(header(1, 2) + entry("word", 1) + entry("word", 2), "invalid file format");
    expectLoadError(header(1, 1) + entry("word", 1) + "x", "invalid file format");
    EXPECT_EQ(loaded(header(1, 1) + entry("word", 9)).find("word"), 9U);
}

TEST(PatriciaTrieTest, SaveReportsStreamFailure) {
    PatriciaTrie trie;
    trie.insert("word", 1);
    std::ostream broken(nullptr);
    EXPECT_THROW(trie.save(broken), PatriciaError);
}

TEST(PatriciaTrieTest, FileOperationsReportErrors) {
    const test_support::TempDir dir;
    PatriciaTrie trie;
    trie.insert("word", 42);

    try {
        trie.saveToFile(dir.path().string());
        FAIL() << "exception expected";
    } catch (const PatriciaError& error) {
        EXPECT_EQ(std::string(error.what()), "cannot open file for writing");
    }
    try {
        trie.loadFromFile((dir.path() / "missing.bin").string());
        FAIL() << "exception expected";
    } catch (const PatriciaError& error) {
        EXPECT_EQ(std::string(error.what()), "cannot open file for reading");
    }

    const std::string path = (dir.path() / "dict.bin").string();
    trie.saveToFile(path);
    PatriciaTrie other;
    other.insert("stale", 1);
    other.loadFromFile(path);
    EXPECT_EQ(other.find("word"), 42U);
    EXPECT_FALSE(other.find("stale").has_value());

    test_support::writeFile(path, "garbage");
    EXPECT_THROW(other.loadFromFile(path), PatriciaError);
    EXPECT_EQ(other.find("word"), 42U);
    EXPECT_EQ(other.size(), 1U);
}

}
