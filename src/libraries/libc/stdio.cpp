#include <libc/stdio.h>

int putchar(int c)
{
    (void)c;
    return 0;
}

int vprintf(const char* format, va_list args)
{
    (void)format;
    (void)args;
    return 0;
}

int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int written = vprintf(format, args);
    va_end(args);
    return written;
}
