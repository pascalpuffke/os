#pragma once

void _kassert(const char* file, int line, const char* func, const char* expr);
void _kassert_msg(const char* file, int line, const char* func, const char* expr, const char* msg);

// Assertions in the kernel are always enabled, so no NDEBUG check is needed.
#define kassert(expr) \
    if (!(expr))      \
    _kassert(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr)

#define kassert_msg(expr, msg) \
    if (!(expr))               \
    _kassert_msg(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr, msg)
