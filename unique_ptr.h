#pragma once

#include <cstddef>
#include <memory>
#include <utility>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator*();
    const T& operator*() const;

    T*       operator->();
    const T* operator->() const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T*      ptr_ = nullptr;
    Deleter deleter_;
};

// =====================================================================
//  Partial specialization for arrays: UniquePtr<T[]>
//  Uses operator[] instead of operator*/operator->.
//  Default deleter is std::default_delete<T[]> (calls delete[]).
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T*      ptr_ = nullptr;
    Deleter deleter_;
};

// =================== Free function ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args);

// ======================== TODO ========================

// ======================== Constructors ========================

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr() : ptr_(nullptr), deleter_() {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr) : ptr_(ptr), deleter_() {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : ptr_(ptr), deleter_(deleter) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : ptr_(ptr), deleter_(std::move(deleter)) {}

// =================== Move ====================================

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(UniquePtr&& other)
    : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        deleter_ = std::move(other.deleter_);
        other.ptr_ = nullptr;
    }
    return *this;
}

// =================== Destructor ==============================

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::~UniquePtr() {
    reset();
}

// =================== Observers ===============================

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::get() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T, Deleter>::get() const {
    return ptr_;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::operator bool() const {
    return ptr_ != nullptr;
}

template <typename T, typename Deleter>
T& UniquePtr<T, Deleter>::operator*() {
    return *ptr_;
}

template <typename T, typename Deleter>
const T& UniquePtr<T, Deleter>::operator*() const {
    return *ptr_;
}

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::operator->() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T, Deleter>::operator->() const {
    return ptr_;
}

template <typename T, typename Deleter>
Deleter& UniquePtr<T, Deleter>::get_deleter() {
    return deleter_;
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T, Deleter>::get_deleter() const {
    return deleter_;
}

// =================== Modifiers ===============================

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::release() {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::reset(T* ptr) {
    if (ptr_ != ptr) {
        if (ptr_) {
            deleter_(ptr_);
        }
        ptr_ = ptr;
    }
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
}

// =====================================================================
// Array specialization
// =====================================================================

// ======================== Constructors ========================

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr() : ptr_(nullptr), deleter_() {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr) : ptr_(ptr), deleter_() {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : ptr_(ptr), deleter_(deleter) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : ptr_(ptr), deleter_(std::move(deleter)) {}

// =================== Move ====================================

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(UniquePtr&& other)
    : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>& UniquePtr<T[], Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        deleter_ = std::move(other.deleter_);
        other.ptr_ = nullptr;
    }
    return *this;
}

// =================== Destructor ==============================

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::~UniquePtr() {
    reset();
}

// =================== Observers ===============================

template <typename T, typename Deleter>
T* UniquePtr<T[], Deleter>::get() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T[], Deleter>::get() const {
    return ptr_;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::operator bool() const {
    return ptr_ != nullptr;
}

template <typename T, typename Deleter>
T& UniquePtr<T[], Deleter>::operator[](size_t index) {
    return ptr_[index];
}

template <typename T, typename Deleter>
const T& UniquePtr<T[], Deleter>::operator[](size_t index) const {
    return ptr_[index];
}

template <typename T, typename Deleter>
Deleter& UniquePtr<T[], Deleter>::get_deleter() {
    return deleter_;
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T[], Deleter>::get_deleter() const {
    return deleter_;
}

// =================== Modifiers ===============================

template <typename T, typename Deleter>
T* UniquePtr<T[], Deleter>::release() {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
}

template <typename T, typename Deleter>
void UniquePtr<T[], Deleter>::reset(T* ptr) {
    if (ptr_ != ptr) {
        if (ptr_) {
            deleter_(ptr_);
        }
        ptr_ = ptr;
    }
}

template <typename T, typename Deleter>
void UniquePtr<T[], Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
}

// =================== make_unique ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}