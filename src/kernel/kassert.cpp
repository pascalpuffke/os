#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

namespace Kernel {

void __assert_failure(const char* file, int line, const char* func, const char* expr) {
    if (TTY::is_initialized()) {
        TTY::set_color(VGA::Color::LIGHT_RED, VGA::Color::BLACK);
        kprintln("ASSERTION FAILED: %s\n         in file: %s:%d\n        function: %s", expr, file, line, func);
    }

    hang();
}

void __assert_failure_msg(const char* file, int line, const char* func, const char* expr, const char* format) {
    if (TTY::is_initialized()) {
        TTY::set_color(VGA::Color::LIGHT_RED, VGA::Color::BLACK);
        kprintln("ASSERTION FAILED: %s\n      expression: %s\n         in file: %s:%d\n        function: %s", format, expr, file, line, func);
    }
    hang();
}

void panic(const char* msg) {
    if (TTY::is_initialized()) {
        TTY::set_color(VGA::Color::LIGHT_RED, VGA::Color::BLACK);
        for (usize x = 0; x < VGA::width; x++)
            kputchar('#');
        kprintln("kernel panic: %s", msg);
    }
    hang();
}

}
