#pragma once

#include <stdlib/constexpr_util.h>
#include <stdlib/move.h>

/**
 * Type that can be either empty or contain a value.
 */
template <typename T>
class Optional final {
public:
    static Optional empty() {
        return Optional {};
    }

    constexpr explicit(false) Optional(const T& value)
        : m_has_value(true)
        , m_value(value) {
    }
    constexpr explicit(false) Optional(T&& value)
        : m_has_value(true)
        , m_value(move(value)) {
    }

    constexpr Optional(const Optional& other) = default;
    constexpr Optional(Optional&& other) noexcept = default;

    constexpr Optional& operator=(const Optional& other) = default;
    constexpr Optional& operator=(Optional&& other) noexcept = default;

    constexpr bool operator==(const Optional& other) const {
        if (m_has_value && other.m_has_value)
            return m_value == other.m_value;
        return m_has_value == other.m_has_value;
    }
    constexpr bool operator!=(const Optional& other) const { return !(*this == other); }

    [[nodiscard]] constexpr bool has_value() const { return m_has_value; }
    [[nodiscard]] constexpr bool is_empty() const { return !m_has_value; }
    [[nodiscard]] constexpr operator bool() const { return m_has_value; }

    [[nodiscard]] constexpr T& value() & {
        CONSTEXPR_AWARE_ASSERT(has_value());
        return m_value;
    }
    [[nodiscard]] constexpr const T& value() const& {
        CONSTEXPR_AWARE_ASSERT(has_value());
        return m_value;
    }
    [[nodiscard]] constexpr T release() {
        CONSTEXPR_AWARE_ASSERT(has_value());
        auto released = move(m_value);
        value().~T();
        m_has_value = false;
        return released;
    }

    [[nodiscard]] constexpr T& value_or(T& default_value) {
        return has_value() ? value() : default_value;
    }
    [[nodiscard]] constexpr const T& value_or(const T& default_value) const {
        return has_value() ? value() : default_value;
    }

    template <typename F>
    [[nodiscard]] constexpr T value_or_else(F&& func) const {
        return has_value() ? m_value : func();
    }

    template <typename F>
    [[nodiscard]] constexpr Optional<T> map(F&& func) const {
        if (has_value())
            return Optional<T>(func(m_value));
        return Optional<T>();
    }

    [[nodiscard]] constexpr T& operator*() { return value(); }
    [[nodiscard]] constexpr const T& operator*() const { return value(); }

private:
    constexpr Optional()
        : m_has_value(false) {
    }

    bool m_has_value;
    T m_value;
};
