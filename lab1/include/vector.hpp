#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>

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

    Vector(size_t n, T value = T{}) : data(nullptr), size(n), capacity(n) {
        if (n > 0) {
            data = new T[n];
            for (size_t i = 0; i < n; i++) {
                data[i] = value;
            }
        }
    }

    Vector(const Vector &other) : data(nullptr), size(other.size), capacity(other.capacity) {
        if (capacity > 0) {
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
    }

    Vector &operator=(const Vector &other) {
        if (this == &other)
            return *this;

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

    Vector(Vector &&other) noexcept : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    Vector &operator=(Vector &&other) noexcept {
        if (this == &other)
            return *this;

        delete[] data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;

        return *this;
    }

    ~Vector() {
        delete[] data;
    }

    size_t Size() const {
        return size;
    }

    void PushBack(T &&value) {
        if (size == capacity) {
            size_t newCapacity;
            if (capacity == 0) {
                newCapacity = 1;
            } else {
                newCapacity = capacity * 2;
            }
            Reallocate(newCapacity);
        }
        data[size++] = std::move(value);
    }

    T &operator[](size_t i) {
        if (i >= size) {
            throw std::out_of_range("index");
        }
        return data[i];
    }

    const T &operator[](size_t i) const {
        if (i >= size) {
            throw std::out_of_range("index");
        }
        return data[i];
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
        for (size_t i = 0; i < v.size; i++) {
            os << v.data[i] << "\n";
        }
        return os;
    }
};
