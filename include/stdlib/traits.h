#pragma once

// integral_constant
template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

// remove_reference

template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T&> {
    using type = T;
};

template <typename T>
struct remove_reference<T&&> {
    using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

// remove_extent

template <typename T>
struct remove_extent {
    using type = T;
};

template <typename T>
struct remove_extent<T[]> {
    using type = T;
};

template <typename T, usize N>
struct remove_extent<T[N]> {
    using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

// is_array

template <typename T>
struct is_array : false_type {
};

template <typename T>
struct is_array<T[]> : true_type {
};

template <typename T, size_t N>
struct is_array<T[N]> : true_type {
};

template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;
