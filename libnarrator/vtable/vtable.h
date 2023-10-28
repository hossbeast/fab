/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct narrator;

typedef struct narrator_vtable {
  int (*sayvf)(struct narrator * const restrict n, const char * const restrict fmt, va_list va)
    __attribute__((nonnull));
  int (*sayw)(struct narrator * const restrict n, const void * const restrict b, size_t l)
    __attribute__((nonnull));
  void (*seek)(struct narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
    __attribute__((nonnull(1)));
  void (*read)(struct narrator * restrict n, void * dst, size_t count, size_t * restrict r)
    __attribute__((nonnull(1, 2)));
  void (*flush)(struct narrator * restrict n)
    __attribute__((nonnull));
} narrator_vtable;

#define NARRATOR_VTABLE(T) {    \
  sayvf : T ## _sayvf,          \
  sayw : T ## _sayw,            \
  seek : T ## _seek,            \
  read : T ## _read,            \
  flush : T ## _flush,          \
}
