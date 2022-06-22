#pragma once

#include <stdlib/traits.h>

template <typename T>
constexpr remove_reference_t<T>&& move(T&& t)
{
    return static_cast<remove_reference_t<T>&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>& t)
{
    return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& t)
{
    return static_cast<T&&>(t);
}
