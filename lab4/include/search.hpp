#pragma once

#include <iosfwd>
#include <string_view>
#include <vector>

#include "kmp.hpp"

std::vector<Token> parseTokens(std::string_view line);
void runSearch(std::istream& input, std::ostream& output);
