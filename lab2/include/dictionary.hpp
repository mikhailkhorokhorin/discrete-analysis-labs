#pragma once

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

#include "patricia_trie.hpp"

class Dictionary {
public:
    std::optional<std::string> execute(std::string_view line);

private:
    std::string add(std::string_view key, std::string_view value);
    std::string erase(std::string_view key);
    std::string lookup(std::string_view key) const;
    std::string runFileCommand(std::string_view command, const std::string& path);

    PatriciaTrie trie_;
};

void runDictionary(std::istream& input, std::ostream& output);
