#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>

template <typename T>
class Vector {
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
    Vector() : data(nullptr), size(0), capacity(0) {
    }

    Vector(size_t n) : size(n), capacity(n) {
        data = new T[n];
    }

    ~Vector() {
        delete[] data;
    }

    size_t Size() const {
        return size;
    }

    void PushBack(T &&value) {
        if (size == capacity) {
            size_t newCap = capacity == 0 ? 1 : capacity * 2;
            Reallocate(newCap);
        }
        data[size++] = std::move(value);
    }

    T &operator[](size_t i) {
        if (i >= size) {
            throw std::out_of_range("index");
        }
        return data[i];
    }

    const T &operator[](size_t index) const {
        if (index >= size) {
            throw std::out_of_range("index");
        }
        return data[index];
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector<T> &v) {
        for (size_t i = 0; i < v.size; i++) {
            os << v.data[i] << "\n";
        }
        return os;
    }
};

struct Pair {
    std::string raw;
    uint32_t key;

    Pair() : key(0) {
    }

    explicit Pair(const std::string &line) : raw(line), key(0) {
        size_t dot1 = raw.find('.');
        size_t dot2 = raw.find('.', dot1 + 1);
        size_t tab = raw.find('\t');

        int day = std::stoi(raw.substr(0, dot1));
        int month = std::stoi(raw.substr(dot1 + 1, dot2 - dot1 - 1));
        int year = std::stoi(raw.substr(dot2 + 1, tab - dot2 - 1));

        key = year * 10000 + month * 100 + day;
    }

    friend std::ostream &operator<<(std::ostream &os, const Pair &p) {
        os << p.raw;
        return os;
    }
};

void CountingPass(Vector<Pair> &input, Vector<Pair> &output, int byteIndex) {
    const int BASE = 256;
    int count[BASE];

    for (int i = 0; i < BASE; ++i) {
        count[i] = 0;
    }

    for (size_t i = 0; i < input.Size(); ++i) {
        uint8_t byte = (input[i].key >> (byteIndex * 8)) & 0xFF;
        count[byte]++;
    }

    for (int i = 1; i < BASE; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = (int)input.Size() - 1; i >= 0; --i) {
        uint8_t byte = (input[i].key >> (byteIndex * 8)) & 0xFF;
        int pos = --count[byte];
        output[pos] = std::move(input[i]);
    }
}

void RadixSort(Vector<Pair> &data) {
    if (data.Size() < 2) {
        return;
    }

    Vector<Pair> buffer(data.Size());

    for (int byte = 0; byte < 4; ++byte) {
        CountingPass(data, buffer, byte);

        for (size_t i = 0; i < data.Size(); ++i) {
            data[i] = std::move(buffer[i]);
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Vector<Pair> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            data.PushBack(Pair(line));
        }
    }

    RadixSort(data);

    std::cout << data;

    return 0;
}