#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

template <typename T>
class Vector {
public:
    Vector() = default;

    explicit Vector(std::size_t count)
        : data_(count > 0 ? new T[count] : nullptr), size_(count), capacity_(count) {}

    Vector(const Vector& other)
        : data_(other.capacity_ > 0 ? new T[other.capacity_] : nullptr),
          size_(other.size_),
          capacity_(other.capacity_) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector copy(other);
            swap(copy);
        }
        return *this;
    }

    Vector(Vector&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)),
          size_(std::exchange(other.size_, 0)),
          capacity_(std::exchange(other.capacity_, 0)) {}

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
            capacity_ = std::exchange(other.capacity_, 0);
        }
        return *this;
    }

    ~Vector() { delete[] data_; }

    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] std::size_t capacity() const { return capacity_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    T* data() { return data_; }
    const T* data() const { return data_; }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

    void pushBack(T value) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_++] = std::move(value);
    }

    T& operator[](std::size_t index) { return data_[index]; }
    const T& operator[](std::size_t index) const { return data_[index]; }

private:
    void reallocate(std::size_t newCapacity) {
        T* newData = new T[newCapacity];
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }

    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

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

void countingPass(Vector<Pair>& input, Vector<Pair>& output, int byteIndex);
void radixSort(Vector<Pair>& data);

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

namespace {

constexpr std::size_t BYTE_BASE = 256;
constexpr std::uint32_t BYTE_MASK = 0xFF;
constexpr int KEY_BYTES = 4;
constexpr int BITS_PER_BYTE = 8;

std::size_t byteAt(std::uint32_t key, int byteIndex) {
    return (key >> (byteIndex * BITS_PER_BYTE)) & BYTE_MASK;
}

}

void countingPass(Vector<Pair>& input, Vector<Pair>& output, int byteIndex) {
    std::array<std::size_t, BYTE_BASE> count{};
    for (const Pair& pair : input) {
        ++count[byteAt(pair.key, byteIndex)];
    }
    for (std::size_t i = 1; i < BYTE_BASE; ++i) {
        count[i] += count[i - 1];
    }
    for (std::size_t i = input.size(); i > 0; --i) {
        Pair& pair = input[i - 1];
        const std::size_t position = --count[byteAt(pair.key, byteIndex)];
        output[position] = std::move(pair);
    }
}

void radixSort(Vector<Pair>& data) {
    if (data.size() < 2) {
        return;
    }
    Vector<Pair> buffer(data.size());
    for (int byteIndex = 0; byteIndex < KEY_BYTES; ++byteIndex) {
        countingPass(data, buffer, byteIndex);
        data.swap(buffer);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        Vector<Pair> pairs = readPairs(std::cin);
        radixSort(pairs);
        writePairs(std::cout, pairs);
    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
