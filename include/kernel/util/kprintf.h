#pragma once

#include <stdarg.h>

int kputchar(int);
int vkprintf(const char*, va_list);
int kprintf(const char*, ...);
int kprintln(const char*, ...);
