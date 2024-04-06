#pragma once

#include <stdlib/types.h>

namespace std {

template <typename T>
class initializer_list {
public:
    using value_type = T;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = usize;
    using iterator = const T*;
    using const_iterator = const T*;

    constexpr initializer_list() noexcept
        : m_data(nullptr)
        , m_size(0) {
    }

    constexpr size_type size() const noexcept { return m_size; }

    constexpr const_iterator begin() const noexcept { return m_data; }
    constexpr const_iterator end() const noexcept { return m_data + m_size; }

private:
    constexpr initializer_list(const T* data, size_type size) noexcept
        : m_data(data)
        , m_size(size) {
    }

    const T* m_data { nullptr };
    size_type m_size { 0 };
};

template <typename T>
inline constexpr const T* begin(initializer_list<T> il) noexcept { return il.begin(); }

template <typename T>
inline constexpr const T* end(initializer_list<T> il) noexcept { return il.end(); }

}
