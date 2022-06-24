#pragma once

#ifdef KERNEL
#include <kernel/util/kassert.h>
#endif

#ifdef KERNEL
#define assert(expr) kassert(expr)
#define assert_msg(expr, msg) kassert_msg(expr, msg)
#define stub() kstub()
#else
#define assert(expr) ((void)0)
#define assert_msg(expr, msg) ((void)0)
#define stub() ((void)0)
#endif
