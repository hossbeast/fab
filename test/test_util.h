/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

/*

MODULE
 test_util

SUMMARY
 util functions for unit tests in components that are dependencies of libxunit, other than libxapi

REMARKS
 intended to present roughly the same api as libxunit

*/

#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/exit.h"
#include "xapi/errtab.h"

#include "errtab/TEST.errtab.h"

#include "macros.h"

extern uint32_t assertions_passed;
extern uint32_t assertions_failed;

#define restrict __restrict

void ufailf_info(const char * restrict value, const char * restrict expfmt, const char * restrict actfmt, ...)
  __attribute__((nonnull(1, 2, 3)));

void ufails_info(const char * restrict value, const char * restrict exp, const char * restrict act);

#define ufail()           \
  do {                    \
    assertions_failed++;  \
    fail(TEST_FAIL);      \
  } while(0)

#define ufailf(value, expfmt, actfmt, ...)                \
  do {                                                    \
    ufailf_info(value, expfmt, actfmt, ##__VA_ARGS__);    \
    ufail();                                              \
  } while(0)

#define ufails(value, exp, act, ...)              \
  do {                                            \
    ufails_info(value, exp, act);                 \
    ufail();                                      \
  } while(0)

#define _asserts(bool, value, exp, act, ...)  \
  do {                                        \
    if((bool))                                \
    {                                         \
      assertions_passed++;                    \
    }                                         \
    else                                      \
    {                                         \
      ufails(value, exp, act);                \
    }                                         \
  } while(0)

#define _assertf(bool, value, expfmt, actfmt, ...)  \
  do {                                              \
    if(bool)                                        \
    {                                               \
      assertions_passed++;                          \
    }                                               \
    else                                            \
    {                                               \
      ufailf(value, expfmt, actfmt, ##__VA_ARGS__); \
    }                                               \
  } while(0)

#define assert_eq_c(exp, act)                 \
  _assertf(act == exp, QUOTE(act), "%c", "%c", exp, act)

#define assert_eq_d(exp, act)                 \
  _assertf(act == exp, QUOTE(act), "%d", "%d", exp, act)

#define assert_eq_zu(exp, act)                 \
  _assertf(act == exp, QUOTE(act), "%zu", "%zu", exp, act)

#define assert_eq_u32(exp, act)                 \
  _assertf(act == exp, QUOTE(act), "%"PRIu32, "%"PRIu32, exp, act)

#define assert_eq_u64(exp, act)                 \
  _assertf(act == exp, QUOTE(act), "%"PRIu64, "%"PRIu64, exp, act)

#define assert_ge_d(exp, act)                 \
  _assertf(act >= exp, QUOTE(act), ">= %d", "%d", exp, act)

#define assert_gt_d(exp, act)                 \
  _assertf(act > exp, QUOTE(act), "> %d", "%d", exp, act)

#define assert_gt_u64(exp, act)                 \
  _assertf(act > exp, QUOTE(act), "> %"PRIu64, "%"PRIu64, exp, act)

#define assert_gt_zu(exp, act)                  \
  _assertf(act > exp, QUOTE(act), "> %zu", "%zu", exp, act)

#define assert_lt_d(exp, act)                 \
  _assertf(act < exp, QUOTE(act), "< %d", "%d", exp, act)

#define assert_le_d(exp, act)                 \
  _assertf(act <= exp, QUOTE(act), "<= %d", "%d", exp, act)

#define assert_eq_s(exp, act)                 \
  _asserts(strcmp(act, exp) == 0, QUOTE(act), exp, act)

#define assert_eq_b(exp, act)                                         \
  _assertf(act == exp, QUOTE(act), "%s", "%s", exp ? "true" : "false", act ? "true" : "false")

#define assert_eq_exit(expected, actual)                 \
  _assertf(                                              \
      expected == actual                                 \
    , QUOTE(actual)                                      \
    , "%s(%d)", "%s(%d)"                                 \
    , xapi_exit_errname(expected)                        \
    , expected                                           \
    , xapi_exit_errname(actual)                          \
    , actual                                             \
  )

#define assert_eq_p(exp, act)                            \
  _assertf(act == exp, QUOTE(act), "%p", "%p", exp, act)

#define assert_null(act)                      \
  _assertf(!act, QUOTE(act), "null", "%p", act)

#define assert_notnull(act)                   \
  _assertf(act, QUOTE(act), "not null", "%p", act)

static inline void assert_infos(const char * restrict key, const char * restrict val)
{
  xapi_info_pushs(key, val);
}

static inline void assert_infof(const char * restrict key, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  xapi_info_pushvf(key, fmt, va);

  va_end(va);
}

static inline void assert_info_unstage()
{
  xapi_info_unstage();
}

#define summarize                                                                           \
  do {                                                                                      \
    printf("%s with %%%6.2f pass rate on %6d assertions at %s:%d\n"                         \
      , assertions_failed ? "[TEST:FAIL]" : "[TEST:PASS]"                                   \
      , 100 * ((double)assertions_passed / (double)(assertions_passed + assertions_failed)) \
      , assertions_passed + assertions_failed                                               \
      , __FILE__                                                                            \
      , __LINE__                                                                            \
    );                                                                                      \
  } while(0)

#undef restrict
#endif
