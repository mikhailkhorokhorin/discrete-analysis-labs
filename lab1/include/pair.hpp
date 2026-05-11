#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

struct TPair {
    std::string raw;
    uint32_t key;

    TPair() : raw(), key(0) {
    }

    explicit TPair(const std::string &line) : raw(line), key(0) {
        size_t dot1 = raw.find('.');
        size_t dot2 = raw.find('.', dot1 + 1);
        size_t tab = raw.find('\t');

        if (dot1 == std::string::npos || dot2 == std::string::npos || tab == std::string::npos) {
            throw std::invalid_argument("Invalid input format");
        }

        int day = std::stoi(raw.substr(0, dot1));
        int month = std::stoi(raw.substr(dot1 + 1, dot2 - dot1 - 1));
        int year = std::stoi(raw.substr(dot2 + 1, tab - dot2 - 1));

        if (day < 1 || day > 31 || month < 1 || month > 12) {
            throw std::invalid_argument("Invalid date");
        }

        key = static_cast<uint32_t>(year) * 10000
            + static_cast<uint32_t>(month) * 100
            + static_cast<uint32_t>(day);
    }

    TPair(const TPair &other) : raw(other.raw), key(other.key) {
    }

    TPair &operator=(const TPair &other) {
        if (this != &other) {
            raw = other.raw;
            key = other.key;
        }
        return *this;
    }

    TPair(TPair &&other) noexcept : raw(std::move(other.raw)), key(other.key) {
    }

    TPair &operator=(TPair &&other) noexcept {
        if (this != &other) {
            raw = std::move(other.raw);
            key = other.key;
        }
        return *this;
    }
};
