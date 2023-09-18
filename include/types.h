/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * common type definitions - suitable for inclusion anywhere in the fab project
 */

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#define restrict __restrict

static inline size_t roundup(size_t x, size_t y)
{
  size_t r = x % y;
  if(r)
    return x + (y - r);
  return x;
}

#define API
#define APIDATA

/*
 * Get a pointer to a containing structure from an inner structure
 *
 * NOTE : under gcc10 and -O2, usage of this macro triggers -Warray-bounds
 */
#define containerof(ptr, type, member) ({        \
  void *__mptr = (void*)(ptr);                   \
  (type*)(__mptr - offsetof(type, member));      \
})

#define offsetofend(type, member) offsetof(type, member) + sizeof(((type*)(void*)0)->member)

#define STATIC_ASSERT(x) _Static_assert(x, #x)

#define RUNTIME_ASSERT(x) do {                          \
  if (!(x)) {                                           \
    fprintf(stderr, "ASSERT %s:%d - %s\n", __FILE__, __LINE__, #x);  \
    RUNTIME_ABORT();                                    \
  }                                                     \
} while(0)

#if CONFIG_DEBUG || CONFIG_DEVEL || XUNIT
/* this will cause asan to print a backtrace */
#define RUNTIME_ABORT() do {           \
  *((volatile char*)0x42) = 0x42;      \
} while(0)
#else
#define RUNTIME_ABORT() do { abort(); } while(0)
#endif

#if CONFIG_DEVEL
#include <stdio.h>
#include <unistd.h>
#define tracef(fmt, ...) do { printf("[%5d/%-5d]%50s:%-5d "fmt"\n", getpid(), gettid(), __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0)
#else
#define tracef(fmt, ...)
#endif
