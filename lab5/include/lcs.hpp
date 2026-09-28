#pragma once

#include <iosfwd>
#include <string>

#include "suffix_tree.hpp"

LcsResult longestCommonSubstrings(const std::string& first, const std::string& second);
void runLcs(std::istream& input, std::ostream& output);
