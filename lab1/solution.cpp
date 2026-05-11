#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

const int BYTE_BASE = 256;
const int BYTE_MASK = 0xFF;
const int UINT32_BYTES = 4;

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

        key = static_cast<uint32_t>(year) * 10000 + static_cast<uint32_t>(month) * 100
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

template <typename T>
class TVector {
  private:
    T *data;
    size_t size;
    size_t capacity;

    void Reallocate(size_t newCapacity) {
        T *newData = new T[newCapacity];
        for (size_t i = 0; i < size; i++) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

  public:
    TVector() : data(nullptr), size(0), capacity(0) {
    }

    explicit TVector(size_t n) : data(nullptr), size(n), capacity(n) {
        if (n > 0) {
            data = new T[n];
        }
    }

    TVector(const TVector &other) : data(nullptr), size(other.size), capacity(other.capacity) {
        if (capacity > 0) {
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
    }

    TVector &operator=(const TVector &other) {
        if (this == &other) {
            return *this;
        }
        T *newData = nullptr;
        if (other.capacity > 0) {
            newData = new T[other.capacity];
            for (size_t i = 0; i < other.size; ++i) {
                newData[i] = other.data[i];
            }
        }
        delete[] data;
        data = newData;
        size = other.size;
        capacity = other.capacity;
        return *this;
    }

    TVector(TVector &&other) noexcept
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    TVector &operator=(TVector &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] data;
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
        return *this;
    }

    ~TVector() {
        delete[] data;
    }

    size_t Size() const {
        return size;
    }

    void PushBack(T &&value) {
        if (size == capacity) {
            Reallocate((capacity == 0) ? 1 : capacity * 2);
        }
        data[size++] = std::move(value);
    }

    T &operator[](size_t i) {
        return data[i];
    }

    const T &operator[](size_t i) const {
        return data[i];
    }
};

void CountingPass(TVector<TPair> &input, TVector<TPair> &output, int byteIndex) {
    int count[BYTE_BASE];
    for (int i = 0; i < BYTE_BASE; ++i) {
        count[i] = 0;
    }
    for (size_t i = 0; i < input.Size(); ++i) {
        uint8_t byteVal = static_cast<uint8_t>((input[i].key >> (byteIndex * 8)) & BYTE_MASK);
        count[byteVal]++;
    }
    for (int i = 1; i < BYTE_BASE; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = static_cast<int>(input.Size()) - 1; i >= 0; --i) {
        uint8_t byteVal = static_cast<uint8_t>((input[i].key >> (byteIndex * 8)) & BYTE_MASK);
        int pos = --count[byteVal];
        output[pos] = std::move(input[i]);
    }
}

void RadixSort(TVector<TPair> &data) {
    if (data.Size() < 2) {
        return;
    }
    TVector<TPair> buffer(data.Size());
    for (int byte = 0; byte < UINT32_BYTES; ++byte) {
        CountingPass(data, buffer, byte);
        for (size_t i = 0; i < data.Size(); ++i) {
            data[i] = std::move(buffer[i]);
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TVector<TPair> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            data.PushBack(TPair(line));
        }
    }

    RadixSort(data);

    for (size_t i = 0; i < data.Size(); ++i) {
        std::cout << data[i].raw << "\n";
    }

    return 0;
}
