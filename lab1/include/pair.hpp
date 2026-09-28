#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

#include "vector.hpp"

inline constexpr std::size_t VALUE_LENGTH = 64;

struct Pair {
    std::string raw;
    std::uint32_t key = 0;
};

bool isBlank(std::string_view line);
std::uint32_t parseDateKey(std::string_view date);
Pair parsePair(std::string line);
Vector<Pair> readPairs(std::istream& input);
void writePairs(std::ostream& output, const Vector<Pair>& pairs);
