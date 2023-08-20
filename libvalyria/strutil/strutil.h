/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

SUMMARY
 dynamically resizing string

*/

#include <stdarg.h>

#include "types.h"

void strloadc(char ** restrict dst, int c)
  __attribute__((nonnull));

void strloads(char ** restrict dst, const char * restrict s)
  __attribute__((nonnull));

void strloadw(char ** restrict dst, const void * restrict buf, size_t bufl)
  __attribute__((nonnull));

void strloadf(char ** restrict dst, const char * restrict fmt, ...)
  __attribute__((nonnull(2)))
  __attribute__((format(printf, 2, 3)));

void strloadvf(char ** restrict dst, const char * restrict fmt, va_list va)
  __attribute__((nonnull));
