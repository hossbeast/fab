/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "unit.h"

uint32_t assertions_passed;
uint32_t assertions_failed;

void ufailf_info(const char * restrict value, const char * restrict expfmt, const char * restrict actfmt, ...)
{
  va_list va;
  va_start(va, actfmt);

  fprintf(stderr, "%s : expected ", value);
  vfprintf(stderr, expfmt, va);
  fprintf(stderr, " actual ");
  vfprintf(stderr, actfmt, va);
  fprintf(stderr, "\n");

  va_end(va);
}

void ufails_info(const char * restrict value, const char * restrict exp, const char * restrict act)
{
  fprintf(stderr, "%s : expected %s actual %s\n", value, exp, act);
}
