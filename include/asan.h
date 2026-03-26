/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#if CONFIG_DEVEL
#include "sanitizer/asan_interface.h"

#define ASAN_TAINT(x) __asan_poison_memory_region(x, sizeof(*x))
#define ASAN_CLEAN(x) __asan_unpoison_memory_region(x, sizeof(*x))
#else
#define ASAN_TAINT(x)
#define ASAN_CLEAN(x)
#endif
