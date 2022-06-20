#pragma once

#include <stdlib/types.h>

template <typename T, usize N>
class Array final {
public:
    constexpr Array(const T (&values)[N])
    {
        for (usize i = 0; i < N; i++) {
            m_storage[i] = values[i];
        }
    }
    constexpr Array(const T value)
    {
        for (usize i = 0; i < N; i++) {
            m_storage[i] = value;
        }
    }

    [[nodiscard]] constexpr usize size() const { return N; }
    [[nodiscard]] constexpr bool empty() const { return N == 0; }

    [[nodiscard]] constexpr T& at(usize index) { return m_storage[index]; }
    [[nodiscard]] constexpr const T& at(usize index) const { return m_storage[index]; }
    [[nodiscard]] constexpr T& operator[](usize index) { return m_storage[index]; }
    [[nodiscard]] constexpr const T& operator[](usize index) const { return m_storage[index]; }

    [[nodiscard]] constexpr T& first() { return m_storage[0]; }
    [[nodiscard]] constexpr const T& first() const { return m_storage[0]; }
    [[nodiscard]] constexpr T& last() { return m_storage[N]; }
    [[nodiscard]] constexpr const T& last() const { return m_storage[N]; }

    [[nodiscard]] constexpr bool operator==(const Array<T, N>& other) const
    {
        for (usize i = 0; i < N; i++) {
            if (m_storage[i] != other.m_storage[i])
                return false;
        }
        return true;
    }

private:
    T m_storage[N];
};
