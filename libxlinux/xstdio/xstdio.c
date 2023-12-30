/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "types.h"

#include "xstdio.h"

#include "fmt.h"

int API xvdprintf(int fd, const char * const restrict fmt, va_list va)
{
  int r;

  r = vdprintf(fd, fmt, va);
  RUNTIME_ASSERT(r != -1);

  return r;
}

void API xrenames(const char * const restrict old, const char * const restrict new)
{
  RUNTIME_ASSERT(rename(old, new) == 0);
}

void API xrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  va_list va;

  va_start(va, newfmt);
  xrenamevf(oldfmt, newfmt, va);
  va_end(va);
}

void API xrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  char old[PATH_MAX];
  char new[PATH_MAX];

  fmt_apply(old, sizeof(old), oldfmt, va);
  fmt_apply(new, sizeof(new), newfmt, va);

  xrenames(old, new);
}

void API uxrenames(const char * const restrict old, const char * const restrict new)
{
  int r;

  r = rename(old, new);
  RUNTIME_ASSERT(r == 0 || errno == ENOENT || errno == ENOTDIR);
}

void API uxrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  va_list va;

  va_start(va, newfmt);
  uxrenamevf(oldfmt, newfmt, va);
  va_end(va);
}

void API uxrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  char old[PATH_MAX];
  char new[PATH_MAX];

  fmt_apply(old, sizeof(old), oldfmt, va);
  fmt_apply(new, sizeof(new), newfmt, va);

  uxrenames(old, new);
}
