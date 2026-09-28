#include "dictionary.hpp"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <new>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "patricia_trie.hpp"

namespace {

bool isSpace(char symbol) {
    return symbol == ' ' || symbol == '\t' || symbol == '\r' || symbol == '\n' || symbol == '\v' ||
           symbol == '\f';
}

std::vector<std::string_view> splitTokens(std::string_view line) {
    std::vector<std::string_view> tokens;
    std::size_t position = 0;
    while (position < line.size()) {
        while (position < line.size() && isSpace(line[position])) {
            ++position;
        }
        const std::size_t start = position;
        while (position < line.size() && !isSpace(line[position])) {
            ++position;
        }
        if (position > start) {
            tokens.push_back(line.substr(start, position - start));
        }
    }
    return tokens;
}

std::optional<std::uint64_t> parseValue(std::string_view text) {
    const bool allDigits =
        std::ranges::all_of(text, [](char symbol) { return symbol >= '0' && symbol <= '9'; });
    if (text.empty() || !allDigits) {
        return std::nullopt;
    }
    std::uint64_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        return std::nullopt;
    }
    return value;
}

constexpr std::string_view INVALID_COMMAND = "ERROR: invalid command";
constexpr std::string_view UNKNOWN_COMMAND = "ERROR: unknown command";
constexpr std::string_view INVALID_KEY = "ERROR: invalid key";
constexpr std::string_view INVALID_VALUE = "ERROR: invalid value";

}

std::optional<std::string> Dictionary::execute(std::string_view line) {
    const std::vector<std::string_view> tokens = splitTokens(line);
    if (tokens.empty()) {
        return std::nullopt;
    }
    const std::string_view command = tokens.front();
    if (command == "+") {
        if (tokens.size() != 3) {
            return std::string(INVALID_COMMAND);
        }
        return add(tokens[1], tokens[2]);
    }
    if (command == "-") {
        if (tokens.size() != 2) {
            return std::string(INVALID_COMMAND);
        }
        return erase(tokens[1]);
    }
    if (command == "!") {
        if (tokens.size() < 2 || (tokens[1] != "Save" && tokens[1] != "Load")) {
            return std::string(UNKNOWN_COMMAND);
        }
        if (tokens.size() != 3) {
            return std::string(INVALID_COMMAND);
        }
        return runFileCommand(tokens[1], std::string(tokens[2]));
    }
    if (tokens.size() != 1) {
        return std::string(UNKNOWN_COMMAND);
    }
    return lookup(command);
}

std::string Dictionary::add(std::string_view key, std::string_view value) {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    const std::optional<std::uint64_t> number = parseValue(value);
    if (!number) {
        return std::string(INVALID_VALUE);
    }
    return trie_.insert(normalizeKey(key), *number) ? "OK" : "Exist";
}

std::string Dictionary::erase(std::string_view key) {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    return trie_.remove(normalizeKey(key)) ? "OK" : "NoSuchWord";
}

std::string Dictionary::lookup(std::string_view key) const {
    if (!isValidKey(key)) {
        return std::string(INVALID_KEY);
    }
    const std::optional<std::uint64_t> value = trie_.find(normalizeKey(key));
    if (!value) {
        return "NoSuchWord";
    }
    return "OK: " + std::to_string(*value);
}

std::string Dictionary::runFileCommand(std::string_view command, const std::string& path) {
    try {
        if (command == "Save") {
            trie_.saveToFile(path);
        } else {
            trie_.loadFromFile(path);
        }
    } catch (const PatriciaError& error) {
        return std::string("ERROR: ") + error.what();
    }
    return "OK";
}

void runDictionary(std::istream& input, std::ostream& output) {
    Dictionary dictionary;
    std::string line;
    while (std::getline(input, line)) {
        std::optional<std::string> response;
        try {
            response = dictionary.execute(line);
        } catch (const std::bad_alloc&) {
            response = "ERROR: out of memory";
        }
        if (response) {
            output << *response << '\n';
        }
    }
}
