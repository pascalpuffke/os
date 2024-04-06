#pragma once

#include <stdlib/optional.h>

/**
 * Simple Error type used as the default error type for Result.
 * It is a wrapper around a `const char*`.
 * @see Result
 */
class Error final {
public:
    /**
     * Constructs an empty Error, setting its internal message to `nullptr`.
     */
    constexpr Error()
        : m_message() {
    }
    /**
     * Constructs an Error with the given message.
     * @param message The message to set.
     */
    constexpr explicit Error(const char* message)
        : m_message(message) {
    }

    constexpr Error(const Error&) = default;
    constexpr Error(Error&&) = default;

    constexpr Error& operator=(const Error& other) = default;
    constexpr Error& operator=(Error&& other) = default;

    constexpr bool operator==(const Error& other) const { return m_message == other.m_message; }
    constexpr bool operator!=(const Error& other) const { return !(*this == other); }

    /**
     * @brief Returns the possibly empty message of this Error.
     *
     * @return The message of this Error
     */
    [[nodiscard]] constexpr const char* message() const { return m_message; }

private:
    const char* m_message;
};

/**
 * Result type used for error handling.
 * Supports chaining similar to Rust's std::result<T, E> type.
 * @code {.cpp}
 * // Example usage:
 * Result<int> res = 10;
 * const auto number = res
 *                        .map([](auto x) { return x * 2; })
 *                        .map([](auto x) { return x + 49; })
 *                        .or_else([](auto e) { kprintln("Error: %s", e.message()); return 0; })
 *                        .value();
 * println("Result: %d", number); // Prints "Result: 69"
 * @endcode
 */
template <typename T, typename E = Error>
class Result final {
public:
    constexpr explicit(false) Result(const T& value)
        : m_value(value) {
    }
    constexpr explicit(false) Result(const E& error)
        : m_error(error) {
    }

    constexpr Result(const Result& other) = default;
    constexpr Result(Result&& other) noexcept = default;

    constexpr Result& operator=(const Result& other) = default;
    constexpr Result& operator=(Result&& other) noexcept = default;

    constexpr bool operator==(const Result& other) const {
        if (m_value.has_value() && other.m_value.has_value())
            return m_value.value() == other.m_value.value();
        return m_error.value() == other.m_error.value();
    }

    constexpr bool operator!=(const Result& other) const { return *this != other; }

    [[nodiscard]] constexpr bool has_value() const { return m_value.has_value(); }
    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }

    /**
     * Returns whether this Result holds a value.
     * @see has_value()
     * @return `true` if this Result holds a value, `false` if it holds an error.
     */
    [[nodiscard]] constexpr operator bool() const { return has_value(); }

    /**
     * Returns the value of this Result or panics if this Result holds an error.
     * @note As this function returns a reference, it is not safe to call it on a Result that holds an error.
     *       Use `has_value()` to check if this Result holds a value before calling this function, as otherwise
     *       it will panic.
     * @return The value of this Result.
     */
    [[nodiscard]] constexpr T& value() {
        CONSTEXPR_AWARE_ASSERT(has_value());
        return m_value.value();
    }

    /**
     * Returns the value of this Result or panics if this Result holds an error.
     * @note As this function returns a reference, it is not safe to call it on a Result that holds an error.
     *       Use `has_value()` to check if this Result holds a value before calling this function, as otherwise
     *       it will panic.
     * @return The value of this Result.
     */
    [[nodiscard]] constexpr const T& value() const {
        CONSTEXPR_AWARE_ASSERT(has_value());
        return m_value.value();
    }

    /**
     * Returns the error of this Result or panics if this Result holds a value.
     * @note As this function returns a reference, it is not safe to call it on a Result that holds a value.
     *       Use `has_error()` to check if this Result holds an error before calling this function, as otherwise
     *       it will panic.
     * @return The error of this Result.
     */
    [[nodiscard]] constexpr E& error() {
        CONSTEXPR_AWARE_ASSERT(has_error());
        return m_error.value();
    }

    /**
     * Returns the error of this Result or panics if this Result holds a value.
     * @note As this function returns a reference, it is not safe to call it on a Result that holds a value.
     *       Use `has_error()` to check if this Result holds an error before calling this function, as otherwise
     *       it will panic.
     * @return The error of this Result.
     */
    [[nodiscard]] constexpr const E& error() const {
        CONSTEXPR_AWARE_ASSERT(has_error());
        return m_error.value();
    }

    /**
     * Returns the value of this Result or the given default value if this Result holds an error.
     * @param default_value The default value to return if this Result is empty.
     * @return The value type if it is present, falling back to the default value otherwise.
     */
    [[nodiscard]] constexpr T& value_or(T& default_value) { return has_value() ? value() : default_value; }

    /**
     * Returns the value of this Result or the given default value if this Result holds an error.
     * @param default_value The default value to return if this Result is empty.
     * @return The value type if it is present, falling back to the default value otherwise.
     */
    [[nodiscard]] constexpr const T& value_or(const T& default_value) const { return has_value() ? value() : default_value; }

    /**
     * Returns the value of this Result or the given lazily evaluated default value if this Result holds an error.
     * @param f The function to call if this Result does not hold a value.
     * @return The value type if it is present, otherwise falling back to lazily evaluated default value.
     */
    template <typename F>
    [[nodiscard]] constexpr T value_or_else(F&& f) { return has_value() ? value() : f(); }

    /**
     * Maps a `Result<T, E>` to a `Result<U, E>` by applying a function to a contained value.
     * @param f The function to apply.
     * @return A new Result with the transformed value type if it is present, otherwise with the error of this Result.
     */
    template <typename F>
    [[nodiscard]] constexpr Result<T, E> map(F&& f) const {
        if (has_value())
            return Result<T, E>(f(m_value.value()));
        return Result<T, E>(m_error.value());
    }

    /**
     * Maps a `Result<T, E>` to a `Result<T, F>` by applying a function to a contained error.
     * @param f The function to apply.
     * @return A new Result with the transformed error type if it is present, otherwise with the error of this Result.
     */
    template <typename F>
    [[nodiscard]] constexpr Result<T, E> or_else(F&& f) const {
        if (has_value())
            return Result<T, E>(m_value.value());
        return f(m_error.value());
    }

private:
    Optional<T> m_value = Optional<T>::empty();
    Optional<E> m_error = Optional<E>::empty();
};

template <typename E>
class Result<void, E> final {
public:
    constexpr Result() = default;
    constexpr Result(const E& error)
        : m_error(error) {
    }

    constexpr Result(const Result& other) = default;
    constexpr Result(Result&& other) noexcept = default;

    constexpr Result& operator=(const Result& other) = default;
    constexpr Result& operator=(Result&& other) noexcept = default;

    constexpr bool operator==(const Result& other) const {
        if (other.has_value())
            return false;
        return error() == other.error();
    }

    constexpr bool operator!=(const Result& other) const { return *this != other; }

    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }
    [[nodiscard]] constexpr bool has_value() const { return !has_error(); }
    [[nodiscard]] constexpr operator bool() const { return has_value(); }

    constexpr void value() { }

    [[nodiscard]] constexpr E& error() {
        CONSTEXPR_AWARE_ASSERT(has_error());
        return m_error.value();
    }

    [[nodiscard]] constexpr const E& error() const {
        CONSTEXPR_AWARE_ASSERT(has_error());
        return m_error.value();
    }

private:
    Optional<E> m_error = Optional<E>::empty();
};
