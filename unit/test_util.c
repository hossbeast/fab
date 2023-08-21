/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "test_util.h"

uint32_t assertions_passed;
uint32_t assertions_failed;

void ufailf_info(const char * restrict value, const char * restrict expfmt, const char * restrict actfmt, ...)
{
  va_list va;
  va_start(va, actfmt);
//
//  xapi_info_inserts (0, "value", value);
//  xapi_info_insertvf(1, "expected", expfmt, va);
//  xapi_info_insertvf(2, "actual", actfmt, va);
//

  fprintf(stderr, "%s : expected ", value);
  vfprintf(stderr, expfmt, va);
  fprintf(stderr, " actual ");
  vfprintf(stderr, actfmt, va);
  fprintf(stderr, "\n");

  va_end(va);
}

void ufails_info(const char * restrict value, const char * restrict exp, const char * restrict act)
{
  //xapi_info_inserts(0, "actual", act);
  //xapi_info_inserts(0, "expected", exp);
  //xapi_info_inserts(0, "value", value);

  fprintf(stderr, "%s : expected %s actual %s\n", value, exp, act);
}
