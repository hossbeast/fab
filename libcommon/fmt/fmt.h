/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdarg.h>

#include "types.h"

void fmt_apply(char * const restrict dst, size_t dst_size, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));
