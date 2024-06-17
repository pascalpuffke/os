#pragma once

namespace Kernel {

void __assert_failure(const char* file, int line, const char* func, const char* expr);
void __assert_failure_msg(const char* file, int line, const char* func, const char* expr, const char* format);

[[noreturn]] void panic(const char* msg);

}

// Assertions in the kernel are always enabled, so no NDEBUG check is needed.
#define kassert(expr)         \
    if (!(expr)) [[unlikely]] \
    Kernel::__assert_failure(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr)

// TODO __VA_ARGS__
#define kassert_msg(expr, msg) \
    if (!(expr)) [[unlikely]]  \
    Kernel::__assert_failure_msg(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr, msg)

#define kstub() kassert_msg(false, "Stub called")
