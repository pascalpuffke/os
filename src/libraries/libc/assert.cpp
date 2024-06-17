#include <libc/assert.h>

void _assert(const char* file, int line, const char* func, const char* expr) {
    (void)file;
    (void)line;
    (void)func;
    (void)expr;
}
