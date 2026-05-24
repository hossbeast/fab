/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <unistd.h>
#include <sys/syscall.h>

#include "types.h"

struct timespec;

/**
 * wrapper for futex - suppress failure cases { EAGAIN, EINTR, ETIMEDOUT }
 *
 * futex_wordp - must be 4 byte aligned
 */
int uxfutex(
    int32_t * restrict futex_wordp
  , int futex_op
  , int32_t val
  , const struct timespec * restrict timeout
  , int32_t * restrict uaddr2
  , int32_t val3
)
  __attribute__((nonnull(1)));
