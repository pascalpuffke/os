#include <kernel/heap/kmalloc.h>
#include <libc/stdlib.h>

#define MM Kernel::MemoryManager::get()

void* malloc(size_t size) {
    return MM.allocate(size);
}

void* calloc(size_t count, size_t size) {
    return MM.allocate(count * size);
}

void* realloc([[maybe_unused]] void* ptr, size_t size) {
    return MM.allocate(size);
}

void* _aligned_malloc(size_t size, size_t alignment) {
    return MM.allocate_aligned(size, alignment);
}

void free(void* ptr) {
    MM.free(ptr);
}

void _aligned_free(void* ptr) {
    MM.free_aligned(ptr);
}

double atof(const char* str) {
    return strtod(str, nullptr);
}

int atoi(const char* str) {
    return strtol(str, nullptr, 10);
}

long atol(const char* str) {
    return strtol(str, nullptr, 10);
}

long long atoll(const char* str) {
    return strtoll(str, nullptr, 10);
}

double strtod(const char* str, char** endptr) {
    double result = 0.0;
    bool negative = false;

    if (*str == '-') {
        negative = true;
        ++str;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10.0 + (*str - '0');
        ++str;
    }

    if (endptr)
        *endptr = const_cast<char*>(str);

    if (negative)
        result = -result;

    return result;
}

long strtol(const char* str, char** endptr, int base) {
    (void)base;
    (void)str;
    (void)endptr;
    return 0;
}

long long strtoll(const char* str, char** endptr, int base) {
    (void)base;
    (void)str;
    (void)endptr;
    return 0;
}

unsigned long strtoul(const char* str, char** endptr, int base) {
    (void)base;
    (void)str;
    (void)endptr;
    return 0;
}

unsigned long long strtoull(const char* str, char** endptr, int base) {
    (void)base;
    (void)str;
    (void)endptr;
    return 0;
}

int abs(int value) {
    return value < 0 ? -value : value;
}

struct div_t div(int numerator, int denominator) {
    struct div_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

long labs(long value) {
    return value < 0 ? -value : value;
}

struct ldiv_t ldiv(long numerator, long denominator) {
    struct ldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

long long llabs(long long value) {
    return value < 0 ? -value : value;
}

struct lldiv_t lldiv(long long numerator, long long denominator) {
    struct lldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}
