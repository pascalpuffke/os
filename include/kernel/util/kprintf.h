#pragma once

#include <stdarg.h>
#include <stdlib/string_view.h>

namespace Kernel {

int kputchar(int);
int vkprintf(const char*, va_list);
int kprintf(const char*, ...);
int kprintln(const char*, ...);

}
