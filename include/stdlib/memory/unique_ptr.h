#pragma once

#include <stdlib/assert.h>
#include <stdlib/move.h>
#include <stdlib/types.h>

/**
 * Smart pointer for unique ownership. Resources are automatically freed when the pointer goes out of scope.
 * @code {.cpp}
 * // Example usage:
 * void test_unique_ptr()
 * {
 *     auto ptr = UniquePtr<int>::make(41);
 *     (*ptr)++;
 *     assert(*ptr == 42);
 * }
 * @encode
 * @tparam T The type of the resource.
 */
template <typename T>
class UniquePtr final {
public:
    constexpr UniquePtr() = default;
    constexpr UniquePtr(T* ptr)
        : m_ptr(ptr) {
    }

    template <typename U>
    constexpr UniquePtr(U* ptr)
        : m_ptr(ptr) {
    }

    constexpr UniquePtr(UniquePtr&& other)
        : m_ptr(other.release()) {
    }

    template <typename... Args>
    [[nodiscard]] static constexpr UniquePtr make(Args&&... args) {
        return UniquePtr(new T(forward<Args>(args)...));
    }

    constexpr UniquePtr(const UniquePtr&) = delete;
    constexpr UniquePtr& operator=(const UniquePtr&) = delete;

    constexpr ~UniquePtr() {
        reset();
    }

    constexpr UniquePtr& operator=(UniquePtr&& other) {
        reset(other.release());
        return *this;
    }

    constexpr T* release() {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    constexpr void reset(T* ptr = nullptr) {
        if (m_ptr)
            delete m_ptr;
        m_ptr = ptr;
    }

    constexpr void swap(UniquePtr& other) {
        T* ptr = m_ptr;
        m_ptr = other.m_ptr;
        other.m_ptr = ptr;
    }

    constexpr T* get() const {
        return m_ptr;
    }

    constexpr operator bool() const {
        return m_ptr != nullptr;
    }

    constexpr T& operator*() const {
        return *m_ptr;
    }

    constexpr T* operator->() const {
        return m_ptr;
    }

    // clang-format off
private:
    T* m_ptr = nullptr;
    // clang-format on
};

// Let's not bother with implementing an array specialization for UniquePtr. Use Array<T, N> instead.
