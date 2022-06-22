#pragma once

#ifdef KERNEL
#include <kernel/util/kassert.h>
#endif

// Optional<T> class

/**
 * Type that can be either empty or contain a value.
 */
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

    constexpr Optional(const Optional& other) = default;
    constexpr Optional(Optional&& other) = default;

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
    /**
     * Returns whether this Optional holds a value.
     * @see has_value()
     * @return `true` if this Result holds a value, `false` otherwise.
     */
    [[nodiscard]] constexpr operator bool() const { return has_value(); }

    /**
     * Returns the value of this Optional or panics if it is empty.
     * @note As this function returns a reference, it is not safe to call it on an Optional that is empty.
     *       Use `has_value()` to check if this Optional holds a value before calling this function, as otherwise
     *       it will panic.
     * @return The value of this Optional.
     */
    [[nodiscard]] constexpr T& value()
    {
// TODO: This should not use kernel assertions.
#ifdef KERNEL
        kassert_msg(has_value(), "Optional::value() called on empty Optional");
#endif
        return m_value;
    }
    /**
     * Returns the value of this Optional or panics if it is empty.
     * @note As this function returns a reference, it is not safe to call it on an Optional that is empty.
     *       Use `has_value()` to check if this Optional holds a value before calling this function, as otherwise
     *       it will panic.
     * @return The value of this Optional.
     */
    [[nodiscard]] constexpr const T& value() const
    {
#ifdef KERNEL
        kassert_msg(has_value(), "Optional::value() called on empty Optional");
#endif
        return m_value;
    }

    /**
     * Returns the value of this Optional or the given default value if it is empty.
     * @param default_value The default value to return if this Optional is empty.
     * @return The value type if it is present, falling back to the default value otherwise.
     */
    [[nodiscard]] constexpr T& value_or(T& default_value) { return has_value() ? m_value : default_value; }
    /**
     * Returns the value of this Optional or the given default value if it is empty.
     * @param default_value The default value to return if this Optional is empty.
     * @return The value type if it is present, falling back to the default value otherwise.
     */
    [[nodiscard]] constexpr const T& value_or(const T& default_value) const { return has_value() ? m_value : default_value; }

    /**
     * Returns the value of this Optional or the given default value if it is empty.
     * @param default_value The default value to return if this Optional is empty.
     * @return The value type if it is present, falling back to the default value otherwise.
     */
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
