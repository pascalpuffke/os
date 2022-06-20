#pragma once

#include <kernel/util/kassert.h>

template <typename T>
class Optional final {
public:
    constexpr Optional()
        : m_has_value(false)
    {
    }
    constexpr Optional(const T& value)
        : m_has_value(true)
        , m_value(value)
    {
    }
    constexpr Optional(const Optional& other)
        : m_has_value(other.m_has_value)
        , m_value(other.m_value)
    {
    }
    constexpr Optional(Optional&& other)
        : m_has_value(other.m_has_value)
        , m_value(other.m_value)
    {
    }

    constexpr Optional& operator=(const Optional& other) = default;
    constexpr Optional& operator=(Optional&& other) = default;

    constexpr bool operator==(const Optional& other) const
    {
        if (m_has_value && other.m_has_value)
            return m_value == other.m_value;
        return m_has_value == other.m_has_value;
    }
    constexpr bool operator!=(const Optional& other) const { return !(*this == other); }

    [[nodiscard]] constexpr bool has_value() const { return m_has_value; }
    [[nodiscard]] constexpr operator bool() const { return has_value(); }

    /// @brief Returns the value if it is present, otherwise panics.
    [[nodiscard]] constexpr T& value()
    {
        // TODO: This should not use kernel assertions.
        kassert_msg(has_value(), "Optional::value() called on empty Optional");
        return m_value;
    }
    /// @brief Returns the value if it is present, otherwise panics.
    [[nodiscard]] constexpr const T& value() const
    {
        kassert_msg(has_value(), "Optional::value() called on empty Optional");
        return m_value;
    }

    /// @brief Returns the value type if it is present. Falls back to the default value otherwise.
    [[nodiscard]] constexpr T& value_or(T& default_value) { return has_value() ? m_value : default_value; }
    /// @brief Returns the value type if it is present. Falls back to the default value otherwise.
    [[nodiscard]] constexpr const T& value_or(const T& default_value) const { return has_value() ? m_value : default_value; }

    /// @brief Returns the value type if it is present. Returns lazily evaluated default value otherwise.
    template <typename F>
    [[nodiscard]] constexpr T value_or_else(F&& func) const
    {
        return has_value() ? m_value : func();
    }

    /// @brief Transforms the value type if it is present. Otherwise, it returns an empty Optional.
    template <typename F>
    [[nodiscard]] constexpr Optional<T> map(F&& func) const
    {
        if (has_value())
            return Optional<T>(func(m_value));
        return Optional<T>();
    }

    [[nodiscard]] constexpr T& operator*() { return value(); }
    [[nodiscard]] constexpr const T& operator*() const { return value(); }

private:
    bool m_has_value;
    T m_value;
};
