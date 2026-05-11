#pragma once

#include <utility>

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

    TVector(TVector &&other) noexcept : data(other.data), size(other.size), capacity(other.capacity) {
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
