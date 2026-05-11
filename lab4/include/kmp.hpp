#pragma once

#include <cstdint>
#include <utility>
#include <vector>

using TToken = uint32_t;
using TPosition = std::pair<int, int>;

std::vector<int> BuildPrefixFunction(const std::vector<TToken> &pattern);

std::vector<TPosition> KmpSearch(
    const std::vector<TToken> &pattern,
    const std::vector<TToken> &text,
    const std::vector<TPosition> &positions
);
