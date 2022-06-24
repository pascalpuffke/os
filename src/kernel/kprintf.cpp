#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>

int kputchar(int c)
{
    return Kernel::TTY::put_char(c);
}

int _kprintf_char(char c)
{
    return kputchar(c);
}

int _kprintf_hex(u32 integer, bool upper)
{
    constexpr const char* upper_table = "0123456789ABCDEF";
    constexpr const char* lower_table = "0123456789abcdef";
    const char* table = upper ? upper_table : lower_table;
    u32 digits = 0;

    for (u32 i = 0; i < 8; i++) {
        const u32 digit = (integer >> (28 - i * 4)) & 0xF;
        if (digit == 0 && digits == 0)
            continue;
        digits++;
        kputchar(table[digit]);
    }

    if (digits == 0)
        kputchar('0');

    return digits;
}

int _kprintf_string(const char* str)
{
    int i = 0;
    while (str[i])
        i += kputchar(str[i]);
    return i;
}

int _kprintf_digit(int c)
{
    return kputchar(c);
}

// Only supports %c, %s, %d, %x
int vkprintf(const char* format, va_list args)
{
    int written = 0;
    while (*format) {
        if (*format == '%') {
            ++format;
            switch (*format) {
            case 'p': {
                u32 integer = va_arg(args, u32);
                // NOTE: The Standard C Library does not want me to print the 0x prefix for pointers, but I don't care.
                written += _kprintf_string("0x");
                written += _kprintf_hex(integer, false);
                break;
            }
            case 'c': {
                auto c = va_arg(args, int);
                written += _kprintf_char(c);
                break;
            }
            case 's': {
                const char* s = va_arg(args, const char*);
                written += _kprintf_string(s);
                break;
            }
            case 'd': {
                int n = va_arg(args, int);
                if (n < 0) {
                    kputchar('-');
                    ++written;
                    n = -n;
                }
                char buf[32];
                int i = 0;
                if (n == 0)
                    buf[i++] = '0';
                else
                    while (n > 0) {
                        buf[i++] = '0' + n % 10;
                        n /= 10;
                    }
                while (i > 0)
                    kputchar(buf[--i]);
                break;
            }
            case 'x': {
                int n = va_arg(args, int);
                written += _kprintf_hex(n, false);
                break;
            }
            case 'X': {
                int n = va_arg(args, int);
                written += _kprintf_hex(n, true);
                break;
            }
            case '%': {
                kputchar('%');
                ++written;
                break;
            }
            default: {
                kputchar('%');
                kputchar(*format);
                ++written;
                break;
            }
            }
        } else {
            kputchar(*format);
            ++written;
        }
        ++format;
    }
    return written;
}

int kprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vkprintf(format, args);
    va_end(args);
    return ret;
}
