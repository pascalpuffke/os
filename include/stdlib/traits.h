#pragma once

#include <stdlib/types.h>

// integral_constant

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

// bool_constant

template <bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

// is_same

template <typename T, typename U>
struct is_same : false_type {
};

template <typename T>
struct is_same<T, T> : true_type {
};

template <typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

static_assert(is_same_v<int, int>);
static_assert(!is_same_v<float, int>);

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

static_assert(is_same_v<int, remove_reference_t<int>>);
static_assert(is_same_v<int, remove_reference_t<int&>>);
static_assert(is_same_v<int, remove_reference_t<int&&>>);

// remove_const

template <typename T>
struct remove_const {
    using type = T;
};

template <typename T>
struct remove_const<const T> {
    using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

static_assert(is_same_v<int, remove_const_t<int>>);
static_assert(is_same_v<int, remove_const_t<const int>>);

// remove_volatile

template <typename T>
struct remove_volatile {
    using type = T;
};

template <typename T>
struct remove_volatile<volatile T> {
    using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

static_assert(is_same_v<int, remove_volatile_t<int>>);
static_assert(is_same_v<int, remove_volatile_t<volatile int>>);

// remove_cv

template <typename T>
struct remove_cv {
    using type = T;
};

template <typename T>
struct remove_cv<const T> {
    using type = T;
};

template <typename T>
struct remove_cv<volatile T> {
    using type = T;
};

template <typename T>
struct remove_cv<const volatile T> {
    using type = T;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

static_assert(is_same_v<int, remove_cv_t<int>>);
static_assert(is_same_v<int, remove_cv_t<const int>>);
static_assert(is_same_v<int, remove_cv_t<volatile int>>);
static_assert(is_same_v<int, remove_cv_t<const volatile int>>);

// remove_cvref

template <typename T>
struct remove_cvref {
    using type = remove_cv_t<remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

static_assert(is_same_v<int, remove_cvref_t<int>>);
static_assert(is_same_v<int, remove_cvref_t<const int>>);
static_assert(is_same_v<int, remove_cvref_t<volatile int>>);
static_assert(is_same_v<int, remove_cvref_t<const volatile int>>);
static_assert(is_same_v<int, remove_cvref_t<int&>>);
static_assert(is_same_v<int, remove_cvref_t<const int&>>);
static_assert(is_same_v<int, remove_cvref_t<volatile int&>>);
static_assert(is_same_v<int, remove_cvref_t<const volatile int&>>);

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

static_assert(is_same_v<int, remove_extent_t<int>>);
static_assert(is_same_v<int, remove_extent_t<int[]>>);
static_assert(is_same_v<int, remove_extent_t<int[2]>>);

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

static_assert(!is_array_v<int>);
static_assert(is_array_v<int[]>);
static_assert(is_array_v<int[42]>);

// is_floating_point

// clang-format off
template <typename T>
struct is_floating_point : bool_constant<
                               is_same_v<remove_cv_t<T>, float> || 
                               is_same_v<remove_cv_t<T>, double>> { };
// clang-format on

template <typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

static_assert(!is_floating_point_v<int>);
static_assert(!is_floating_point_v<float&>);
static_assert(!is_floating_point_v<double&>);
static_assert(is_floating_point_v<float>);
static_assert(is_floating_point_v<double>);
static_assert(is_floating_point_v<const float>);
static_assert(is_floating_point_v<const double>);
static_assert(is_floating_point_v<const volatile float>);
static_assert(is_floating_point_v<const volatile double>);

// is_integral

// clang-format off
// TODO unsigned
template <typename T>
struct is_integral : bool_constant<
                         is_same_v<remove_cv_t<T>, bool> ||
                         is_same_v<remove_cv_t<T>, char> ||
                         is_same_v<remove_cv_t<T>, short> ||
                         is_same_v<remove_cv_t<T>, int> ||
                         is_same_v<remove_cv_t<T>, long> ||
                         is_same_v<remove_cv_t<T>, long long>> { };
// clang-format on

template <typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;
