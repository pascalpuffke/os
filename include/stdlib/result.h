#pragma once

// This is NOT thoroughly tested and there is a lot of error potential in this class.
// Use with caution, might behave in unexpected ways.

#include <kernel/util/kassert.h>
#include <stdlib/optional.h>

class Error final {
public:
    constexpr Error()
        : m_message(nullptr)
    {
    }
    constexpr Error(const char* message)
        : m_message(message)
    {
    }

    constexpr Error(const Error&) = default;
    constexpr Error(Error&&) = default;

    constexpr Error& operator=(const Error& other) = default;
    constexpr Error& operator=(Error&& other) = default;

    constexpr bool operator==(const Error& other) const { return m_message == other.m_message; }
    constexpr bool operator!=(const Error& other) const { return !(*this == other); }

    constexpr const char* message() const { return m_message; }

private:
    const char* m_message;
};

template <typename T, typename E = Error>
class Result final {
public:
    constexpr Result(const T& value)
        : m_value(value)
    {
    }
    constexpr Result(const E& error)
        : m_error(error)
    {
    }

    constexpr Result(const Result& other) = default;
    constexpr Result(Result&& other) = default;

    constexpr Result& operator=(const Result& other) = default;
    constexpr Result& operator=(Result&& other) = default;

    constexpr bool operator==(const Result& other) const
    {
        if (m_value.has_value() && other.m_value.has_value())
            return m_value.value() == other.m_value.value();
        return m_error.value() == other.m_error.value();
    }
    constexpr bool operator!=(const Result& other) const { return !(*this == other); }

    [[nodiscard]] constexpr bool has_value() const { return m_value.has_value(); }
    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }
    [[nodiscard]] constexpr operator bool() const { return has_value(); }

    /// @brief Returns the value type if it is present, otherwise panics.
    [[nodiscard]] constexpr T& value()
    {
        kassert_msg(has_value(), "Result::value() called on empty Result");
        return m_value.value();
    }
    /// @brief Returns the value type if it is present, otherwise panics.
    [[nodiscard]] constexpr const T& value() const
    {
        kassert_msg(has_value(), "Result::value() called on empty Result");
        return m_value.value();
    }

    /// @brief Returns the error type if it is present, otherwise panics.
    [[nodiscard]] constexpr E& error()
    {
        kassert_msg(has_error(), "Result::error() called on empty Result");
        return m_error.value();
    }
    /// @brief Returns the error type if it is present, otherwise panics.
    [[nodiscard]] constexpr const E& error() const
    {
        kassert_msg(has_error(), "Result::error() called on empty Result");
        return m_error.value();
    }

    /// @brief Returns the value type if it is present. Falls back to the default value otherwise.
    [[nodiscard]] constexpr T& value_or(T& default_value) { return has_value() ? value() : default_value; }
    /// @brief Returns the value type if it is present. Falls back to the default value otherwise.
    [[nodiscard]] constexpr const T& value_or(const T& default_value) const { return has_value() ? value() : default_value; }

    /// @brief Returns the value type if it is present. Otherwise, it returns lazily evaluated default value.
    template <typename F>
    [[nodiscard]] constexpr T value_or_else(F&& f) { return has_value() ? value() : f(); }

    /// @brief Transforms the value type if it is present. Otherwise, it returns the Result with the error.
    template <typename F>
    [[nodiscard]] constexpr Result<T, E> map(F&& f) const
    {
        if (has_value())
            return Result<T, E>(f(m_value.value()));
        return Result<T, E>(m_error.value());
    }

    /// @brief If the Result holds a value type, it calls given function with the value. Otherwise, returns the Result with the error type.
    template <typename F>
    [[nodiscard]] constexpr Result<T, E> and_then(F&& f) const
    {
        if (has_value())
            return f(m_value.value());
        return Result<T, E>(m_error.value());
    }

    /// @brief If the Result does not hold a value type, it calls given function with the error. Otherwise, returns the Result with the value type.
    template <typename F>
    [[nodiscard]] constexpr Result<T, E> or_else(F&& f) const
    {
        if (has_value())
            return Result<T, E>(m_value.value());
        return f(m_error.value());
    }

private:
    Optional<T> m_value {};
    Optional<E> m_error {};
};

template <typename E>
class Result<void, E> final {
public:
    constexpr Result(const E& error)
        : m_error(error)
    {
    }

    constexpr Result(const Result& other) = default;
    constexpr Result(Result&& other) = default;

    constexpr Result& operator=(const Result& other) = default;
    constexpr Result& operator=(Result&& other) = default;

    constexpr bool operator==(const Result& other) const
    {
        if (other.has_value())
            return false;
        return error() == other.error();
    }
    constexpr bool operator!=(const Result& other) const { return !(*this == other); }

    [[nodiscard]] constexpr bool has_value() const { return false; }
    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }
    [[nodiscard]] constexpr operator bool() const { return false; }

    constexpr void value() { }

    /// @brief Returns the error type if it is present, otherwise panics.
    [[nodiscard]] constexpr E& error()
    {
        kassert_msg(has_error(), "Result::error() called on empty Result");
        return m_error.value();
    }
    /// @brief Returns the error type if it is present, otherwise panics.
    [[nodiscard]] constexpr const E& error() const
    {
        kassert_msg(has_error(), "Result::error() called on empty Result");
        return m_error.value();
    }

private:
    Optional<E> m_error {};
};