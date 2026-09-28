#include "pair.hpp"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace {

constexpr std::uint32_t MAX_DAY = 31;
constexpr std::uint32_t MAX_MONTH = 12;
constexpr std::uint32_t MAX_YEAR = 9999;
constexpr std::uint32_t YEAR_WEIGHT = 10000;
constexpr std::uint32_t MONTH_WEIGHT = 100;

std::uint32_t parseNumber(std::string_view text) {
    const bool allDigits =
        std::ranges::all_of(text, [](char symbol) { return symbol >= '0' && symbol <= '9'; });
    if (text.empty() || !allDigits) {
        throw std::invalid_argument("invalid date");
    }
    std::uint32_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::invalid_argument("invalid date");
    }
    return value;
}

}

bool isBlank(std::string_view line) {
    return std::ranges::all_of(line, [](char symbol) {
        return symbol == ' ' || symbol == '\t' || symbol == '\r' || symbol == '\n' ||
               symbol == '\v' || symbol == '\f';
    });
}

std::uint32_t parseDateKey(std::string_view date) {
    const std::size_t firstDot = date.find('.');
    if (firstDot == std::string_view::npos) {
        throw std::invalid_argument("invalid date");
    }
    const std::size_t secondDot = date.find('.', firstDot + 1);
    if (secondDot == std::string_view::npos) {
        throw std::invalid_argument("invalid date");
    }
    const std::uint32_t day = parseNumber(date.substr(0, firstDot));
    const std::uint32_t month = parseNumber(date.substr(firstDot + 1, secondDot - firstDot - 1));
    const std::uint32_t year = parseNumber(date.substr(secondDot + 1));
    if (day < 1 || day > MAX_DAY || month < 1 || month > MAX_MONTH || year > MAX_YEAR) {
        throw std::invalid_argument("invalid date");
    }
    return year * YEAR_WEIGHT + month * MONTH_WEIGHT + day;
}

Pair parsePair(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    const std::size_t tab = line.find('\t');
    if (tab == std::string::npos) {
        throw std::invalid_argument("missing tab separator");
    }
    if (line.size() - tab - 1 > VALUE_LENGTH) {
        throw std::invalid_argument("value is longer than 64 characters");
    }
    const std::uint32_t key = parseDateKey(std::string_view(line).substr(0, tab));
    return Pair{.raw = std::move(line), .key = key};
}

Vector<Pair> readPairs(std::istream& input) {
    Vector<Pair> pairs;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (isBlank(line)) {
            continue;
        }
        try {
            pairs.pushBack(parsePair(std::exchange(line, {})));
        } catch (const std::invalid_argument& error) {
            throw std::invalid_argument("line " + std::to_string(lineNumber) + ": " + error.what());
        }
    }
    return pairs;
}

void writePairs(std::ostream& output, const Vector<Pair>& pairs) {
    for (const Pair& pair : pairs) {
        output << pair.raw << '\n';
    }
}
