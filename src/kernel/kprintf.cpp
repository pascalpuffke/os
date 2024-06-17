#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>
#include <stdlib/string_view.h>

// Extremely weird and unreliable printf implementation for use in kernel code only.
// Supported syntax: %[flags][width][.precision]type
//                   No support for POSIX parameter fields
// Supported types:  d, i, u, X, x, o, s, c, p
//                   No support for %n or any floating point

namespace Kernel {

int kputchar(int c) { return Kernel::TTY::put_char(static_cast<char>(c)); }

static inline int _kprintf_char(char c) { return kputchar(c); }

static inline int _kprintf_hex(int integer, bool upper) {
    static constexpr auto upper_table = StringView { "0123456789ABCDEF" };
    static constexpr auto lower_table = StringView { "0123456789abcdef" };
    const auto& table = upper ? upper_table : lower_table;
    auto digits = 0;

    for (auto i = 0; i < 8; i++) {
        const auto digit = (integer >> (28 - i * 4)) & 0xF;
        if (digit == 0 && digits == 0)
            continue;
        digits++;
        kputchar(table[digit]);
    }

    if (digits == 0)
        kputchar('0');

    return digits;
}

static inline int _kprintf_string(const char* str) {
    int i = 0;
    while (str[i])
        i += kputchar(str[i]);
    return i;
}

int vkprintf(const char* format, va_list args) {
    int written = 0;
    while (*format) {
        if (*format != '%') {
            kputchar(*format);
            ++written;
            ++format;
            continue;
        }

        ++format;
        switch (*format) {
        case 'p': {
            auto integer = va_arg(args, int);
            written += _kprintf_string("0x");
            written += _kprintf_hex(integer, false);
            break;
        }
        case 'c': {
            auto c = static_cast<char>(va_arg(args, int));
            written += _kprintf_char(c);
            break;
        }
        case 's': {
            const auto* s = va_arg(args, const char*);
            written += _kprintf_string(s);
            break;
        }
        case 'd': {
            auto n = va_arg(args, int);
            if (n < 0) {
                kputchar('-');
                ++written;
                n = -n;
            }
            char buf[32];
            auto i = 0;
            if (n == 0)
                buf[i++] = '0';
            else
                while (n > 0) {
                    buf[i++] = static_cast<char>('0' + n % 10);
                    n /= 10;
                }
            while (i > 0)
                kputchar(buf[--i]);
            break;
        }
        case 'x': {
            auto n = va_arg(args, int);
            written += _kprintf_hex(n, false);
            break;
        }
        case 'X': {
            auto n = va_arg(args, int);
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
        ++format;
    }
    return written;
}

int kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vkprintf(format, args);
    va_end(args);
    return ret;
}

int kprintln(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vkprintf(format, args);
    va_end(args);
    kputchar('\n');
    return ret + 1;
}

}
