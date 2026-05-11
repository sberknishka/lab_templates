#pragma once

#include <cstddef>
#include <algorithm>
template <typename T>
class Array {
public:
    // ======================== Constructors ========================

    Array();                                // empty array, size 0
    explicit Array(size_t size);            // default-constructed elements
    Array(size_t size, const T& value);     // all elements = value

    // =================== Copy and Move ===========================

    Array(const Array& other);
    Array(Array&& other);
    Array& operator=(const Array& other);
    Array& operator=(Array&& other);

    // =================== Destructor ==============================

    ~Array();

    // =================== Element access ==========================

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

    T&       front();
    const T& front() const;

    T&       back();
    const T& back() const;

    T*       data();
    const T* data() const;

    // =================== Capacity ================================

    size_t size() const;
    bool   empty() const;

    // =================== Operations ==============================

    void fill(const T& value);
    void swap(Array& other);

    // =================== Comparison ==============================

    bool operator==(const Array& rhs) const;
    bool operator!=(const Array& rhs) const;

private:
    T*     data_ = nullptr;
    size_t size_ = 0;
};

// ======================== TODO ========================

template <typename T>
Array<T>::Array()
    : data_(nullptr), size_(0) {}

template <typename T>
Array<T>::Array(size_t size)
    : data_(size ? new T[size] : nullptr),
      size_(size) {}

template <typename T>
Array<T>::Array(size_t size, const T& value)
    : data_(size ? new T[size] : nullptr),
      size_(size) {
    fill(value);
}

// =================== Copy and Move ===========================

template <typename T>
Array<T>::Array(const Array& other)
    : data_(other.size_ ? new T[other.size_] : nullptr),
      size_(other.size_) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}

template <typename T>
Array<T>::Array(Array&& other) noexcept
    : data_(other.data_),
      size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
}

template <typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this != &other) {
        delete[] data_;

        size_ = other.size_;
        data_ = size_ ? new T[size_] : nullptr;

        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(Array&& other) noexcept {
    if (this != &other) {
        delete[] data_;

        data_ = other.data_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.size_ = 0;
    }

    return *this;
}

// =================== Destructor ==============================

template <typename T>
Array<T>::~Array() {
    delete[] data_;
}

// =================== Element access ==========================

template <typename T>
T& Array<T>::operator[](size_t index) {
    return data_[index];
}

template <typename T>
const T& Array<T>::operator[](size_t index) const {
    return data_[index];
}

template <typename T>
T& Array<T>::front() {
    return data_[0];
}

template <typename T>
const T& Array<T>::front() const {
    return data_[0];
}

template <typename T>
T& Array<T>::back() {
    return data_[size_ - 1];
}

template <typename T>
const T& Array<T>::back() const {
    return data_[size_ - 1];
}

template <typename T>
T* Array<T>::data() {
    return data_;
}

template <typename T>
const T* Array<T>::data() const {
    return data_;
}

// =================== Capacity ================================

template <typename T>
size_t Array<T>::size() const {
    return size_;
}

template <typename T>
bool Array<T>::empty() const {
    return size_ == 0;
}

// =================== Operations ==============================

template <typename T>
void Array<T>::fill(const T& value) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = value;
    }
}

template <typename T>
void Array<T>::swap(Array& other) {
    T* temp_data = data_;
    data_ = other.data_;
    other.data_ = temp_data;

    size_t temp_size = size_;
    size_ = other.size_;
    other.size_ = temp_size;
}

// =================== Comparison ==============================

template <typename T>
bool Array<T>::operator==(const Array& rhs) const {
    if (size_ != rhs.size_) {
        return false;
    }

    for (size_t i = 0; i < size_; ++i) {
        if (!(data_[i] == rhs.data_[i])) {
            return false;
        }
    }

    return true;
}

template <typename T>
bool Array<T>::operator!=(const Array& rhs) const {
    return !(*this == rhs);
}