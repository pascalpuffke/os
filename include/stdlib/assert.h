#pragma once

#ifdef KERNEL
#include <kernel/util/kassert.h>
#endif

#ifdef KERNEL
#define ASSERT(expr) kassert(expr)
#define ASSERT_MSG(expr, msg) kassert_msg(expr, msg)
#else
#define ASSERT(expr) ((void)0)
#define ASSERT_MSG(expr, msg) ((void)0)
#endif
