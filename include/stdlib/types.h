#pragma once

#include <stddef.h>
#include <stdint.h>

using u8 = uint8_t;
static_assert(sizeof(u8) == 1);
using u16 = uint16_t;
static_assert(sizeof(u16) == 2);
using u32 = uint32_t;
static_assert(sizeof(u32) == 4);
using u64 = uint64_t;
static_assert(sizeof(u64) == 8);

using i8 = int8_t;
static_assert(sizeof(i8) == 1);
using i16 = int16_t;
static_assert(sizeof(i16) == 2);
using i32 = int32_t;
static_assert(sizeof(i32) == 4);
using i64 = int64_t;
static_assert(sizeof(i64) == 8);

using usize = size_t;
using isize = intptr_t;

using f32 = float;
using f64 = double;

static constexpr usize KiB = 1024;
static constexpr usize MiB = 1024 * KiB;
static constexpr usize GiB = 1024 * MiB;
