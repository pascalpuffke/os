#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void _assert(const char* file, int line, const char* func, const char* expr);

#ifndef NDEBUG
#define assert(expr) \
    if (!(expr))     \
    _assert(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr)
#else
#define assert(expr) ((void)0)
#endif

#ifdef __cplusplus
}
#endif
