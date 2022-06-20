#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

__attribute__((malloc)) __attribute__((alloc_size(1))) void* malloc(size_t);
__attribute__((malloc)) __attribute__((alloc_size(1, 2))) void* calloc(size_t, size_t);
__attribute__((alloc_size(2))) void* realloc(void*, size_t);
__attribute__((malloc, alloc_size(1), alloc_align(2))) void* _aligned_malloc(size_t, size_t);
void free(void*);
void _aligned_free(void*);

double atof(const char*);
int atoi(const char*);
long atol(const char*);
long long atoll(const char*);
double strtod(const char*, char**);
long strtol(const char*, char**, int);
long long strtoll(const char*, char**, int);
unsigned long strtoul(const char*, char**, int);
unsigned long long strtoull(const char*, char**, int);

int abs(int);
struct div_t {
    int quot;
    int rem;
};
struct div_t div(int, int);
long labs(long);
struct ldiv_t {
    long quot;
    long rem;
};
struct ldiv_t ldiv(long, long);
long long llabs(long long);
struct lldiv_t {
    long long quot;
    long long rem;
};
struct lldiv_t lldiv(long long, long long);

#ifdef __cplusplus
}
#endif
