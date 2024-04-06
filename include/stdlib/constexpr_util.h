#pragma once

#include <stdlib/assert.h>
#include <stdlib/types.h>

// Would've preferred to use 'if consteval { static_assert ... }' but that appears to not work.
// Ignore assertions entirely then as the compiler will seemingly catch the weirdness by itself anyway.
#define CONSTEXPR_AWARE_ASSERT(cond) if !consteval { assert(cond); }

// Various libc-like functionality, reimplemented for compile-time evaluation and safety.
namespace cmptime {

[[nodiscard]] constexpr auto memcmp(const char* s1, const char* s2, usize limit) -> i32 {
    for (usize i = 0; i < limit; ++i)
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
    return 0;
}

constexpr auto memcpy(char* dest, const char* src, usize limit) -> void {
    for (usize i = 0; i < limit; ++i)
        dest[i] = src[i];
}

constexpr auto memmove(char* dest, const char* src, usize limit) -> void {
    if (src < dest)
        for (usize i = 0; i < limit; ++i)
            dest[i] = src[i];
    else
        for (usize i = limit; i > 0; --i)
            dest[i - 1] = src[i - 1];
}

constexpr auto memset(char* dest, i32 character, usize limit) -> void {
    for (usize i = 0; i < limit; ++i)
        dest[i] = static_cast<char>(character);
}

[[nodiscard]] constexpr auto strlen(const char* str) -> usize {
    usize len = 0;
    while (str[len])
        ++len;
    return len;
}

[[nodiscard]] constexpr auto strcpy(char* dest, const char* src) -> char* {
    usize i = 0;
    while (src[i]) {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
    return dest;
}

[[nodiscard]] constexpr auto strncpy(char* dest, const char* src, usize limit) -> char* {
    usize i = 0;
    while (src[i] && i < limit) {
        dest[i] = src[i];
        ++i;
    }
    while (i < limit) {
        dest[i] = '\0';
        ++i;
    }
    return dest;
}

[[nodiscard]] constexpr auto strcat(char* dest, const char* src) -> char* {
    const usize len = strlen(dest);
    usize i = 0;
    while (src[i]) {
        dest[len + i] = src[i];
        ++i;
    }
    dest[len + i] = '\0';
    return dest;
}

[[nodiscard]] constexpr auto strncat(char* dest, const char* src, usize limit) -> char* {
    const usize len = strlen(dest);
    usize i = 0;
    while (src[i] && i < limit) {
        dest[len + i] = src[i];
        ++i;
    }
    dest[len + i] = '\0';
    return dest;
}

[[nodiscard]] constexpr auto strcmp(const char* s1, const char* s2) -> i32 {
    usize i = 0;
    while (s1[i] == s2[i]) {
        if (s1[i] == '\0')
            return 0;
        ++i;
    }
    return s1[i] - s2[i];
}

[[nodiscard]] constexpr auto strncmp(const char* s1, const char* s2, usize limit) -> i32 {
    usize i = 0;
    while (s1[i] == s2[i] && i < limit) {
        if (s1[i] == '\0')
            return 0;
        ++i;
    }
    return s1[i] - s2[i];
}

}
