/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdarg.h>

#include "types.h"

/// snarf
//
// SUMMARY
//  load the contents of a file into a buffer with read
//
// PARAMETERS
//  dst     - (returns) xmalloc'd buffer
//  [dstl]  - (returns) size of buffer
//  path    - path to file
//
// VARIANTS
//  *at
//   [dirfd] - relative path resolution dir
//
//  u* - only fails when errno != ENOENT
//
void snarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1, 3)));

void snarff(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)));

void snarfvf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 3)));

void snarfats(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1, 4)));

void snarfatf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 4)))
  __attribute__((format(printf, 4, 5)));

void snarfatvf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 4)));

void usnarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1, 3)));

void usnarff(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)));

void usnarfvf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 3)));

void usnarfats(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1, 2, 4)));

void usnarfatf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 4)))
  __attribute__((format(printf, 4, 5)));

void usnarfatvf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 4)));

void fsnarf(char ** const restrict dst, size_t * const restrict dstl, int fd)
  __attribute__((nonnull));
