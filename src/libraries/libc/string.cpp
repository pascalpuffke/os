#include <libc/string.h>

void memcpy(void* dest, const void* src, size_t limit) {
    const char* source = reinterpret_cast<const char*>(src);
    char* destination = reinterpret_cast<char*>(dest);
    for (size_t i = 0; i < limit; ++i)
        destination[i] = source[i];
}

void memmove(void* dest, const void* src, size_t limit) {
    const char* source = reinterpret_cast<const char*>(src);
    char* destination = reinterpret_cast<char*>(dest);
    if (source < destination)
        for (size_t i = 0; i < limit; ++i)
            destination[i] = source[i];
    else
        for (size_t i = limit; i > 0; --i)
            destination[i - 1] = source[i - 1];
}

void memset(void* dest, int character, size_t limit) {
    char* destination = reinterpret_cast<char*>(dest);
    for (size_t i = 0; i < limit; ++i)
        destination[i] = static_cast<char>(character);
}

int memcmp(const void* s1, const void* s2, size_t limit) {
    const char* string1 = reinterpret_cast<const char*>(s1);
    const char* string2 = reinterpret_cast<const char*>(s2);
    for (size_t i = 0; i < limit; ++i)
        if (string1[i] != string2[i])
            return string1[i] - string2[i];
    return 0;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        ++len;
    return len;
}

char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    while (src[i]) {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t limit) {
    size_t i = 0;
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

char* strcat(char* dest, const char* src) {
    const size_t len = strlen(dest);
    size_t i = 0;
    while (src[i]) {
        dest[len + i] = src[i];
        ++i;
    }
    dest[len + i] = '\0';
    return dest;
}

char* strncat(char* dest, const char* src, size_t limit) {
    const size_t len = strlen(dest);
    size_t i = 0;
    while (src[i] && i < limit) {
        dest[len + i] = src[i];
        ++i;
    }
    dest[len + i] = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i] == s2[i]) {
        if (s1[i] == '\0')
            return 0;
        ++i;
    }
    return s1[i] - s2[i];
}

int strncmp(const char* s1, const char* s2, size_t limit) {
    size_t i = 0;
    while (s1[i] == s2[i] && i < limit) {
        if (s1[i] == '\0')
            return 0;
        ++i;
    }
    return s1[i] - s2[i];
}

const void* memchr(const void* ptr, int value, size_t limit) {
    const char* string = reinterpret_cast<const char*>(ptr);
    for (size_t i = 0; i < limit; ++i)
        if (string[i] == value)
            return &string[i];
    return nullptr;
}

const char* strchr(const char* str, int character) {
    size_t i = 0;
    while (str[i] != character) {
        if (!str[i])
            return nullptr;
        ++i;
    }
    return &str[i];
}

size_t strcspn(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i] && !strchr(s2, s1[i]))
        ++i;
    return i;
}

const char* strpbrk(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i] && !strchr(s2, s1[i]))
        ++i;
    return s1 + i;
}

const char* strrchr(const char* str, int character) {
    size_t i = strlen(str);
    while (i > 0 && str[i - 1] != character)
        --i;
    return i > 0 ? &str[i - 1] : nullptr;
}

size_t strspn(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i] && strchr(s2, s1[i]))
        ++i;
    return i;
}

const char* strstr(const char* s1, const char* s2) {
    size_t i = 0;
    while (s1[i]) {
        size_t j = 0;
        while (s2[j] && s1[i + j] == s2[j])
            ++j;
        if (!s2[j])
            return &s1[i];
        ++i;
    }
    return nullptr;
}

char* strtok(char* str, const char* delim) {
    static char* next = nullptr;
    if (str)
        next = str;
    else if (!next)
        return nullptr;
    str = next;
    while (*str && strchr(delim, *str))
        ++str;
    if (!*str)
        return nullptr;
    next = str + 1;
    while (*next && !strchr(delim, *next))
        ++next;
    if (*next) {
        *next = '\0';
        ++next;
    }
    return str;
}
