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

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "macros.h"

extern uint32_t assertions_passed;
extern uint32_t assertions_failed;

void ufailf_info(const char * restrict value, const char * restrict expfmt, const char * restrict actfmt, ...)
  __attribute__((nonnull(1, 2, 3)));

void ufails_info(const char * restrict value, const char * restrict exp, const char * restrict act);

#define ufail()           \
  do {                    \
    assertions_failed++;  \
    RUNTIME_ABORT();      \
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

#define assert_eq_c(exp, act)                                 \
  ({                                                          \
    typeof(exp) _exp = (exp);                                 \
    typeof(act) _act = (act);                                 \
    _assertf(act == exp, QUOTE(act), "%c", "%c", _exp, _act); \
  })

#define assert_eq_d(exp, act)                                   \
  ({                                                            \
    typeof(exp) _exp = (exp);                                   \
    typeof(act) _act = (act);                                   \
    _assertf(_act == _exp, QUOTE(act), "%d", "%d", _exp, _act); \
  })

#define assert_eq_zu(exp, act)                                    \
  ({                                                              \
    typeof(exp) _exp = (exp);                                     \
    typeof(act) _act = (act);                                     \
    _assertf(_act == _exp, QUOTE(act), "%zu", "%zu", _exp, _act); \
  })

#define assert_eq_u32(exp, act)                                           \
  ({                                                                      \
    typeof(exp) _exp = (exp);                                             \
    typeof(act) _act = (act);                                             \
    _assertf(_act == _exp, QUOTE(act), "%"PRIu32, "%"PRIu32, _exp, _act); \
  })

#define assert_eq_u64(exp, act)                                           \
  ({                                                                      \
    typeof(exp) _exp = (exp);                                             \
    typeof(act) _act = act;                                               \
    _assertf(_act ==( _exp),QUOTE(act), "%"PRIu64, "%"PRIu64, _exp, _act); \
  })

#define assert_eq_b(exp, act)                                 \
  ({                                                          \
    typeof(exp) _exp = (exp);                                 \
    typeof(act) _act = (act);                                 \
    _assertf(_act == _exp, QUOTE(act), "%s", "%s", _exp ? "true" : "false", _act ? "true" : "false");  \
  })

#define assert_eq_s(exp, act)                 \
  _asserts(strcmp(act, exp) == 0, QUOTE(act), exp, act)

#define assert_that(act)  \
  _assertf(act, QUOTE(act), "true", "false")

#define assert_ne_c(exp, act)                 \
  _assertf(act != exp, QUOTE(act), "!= %c", "%c", exp, act)

#define assert_ne_d(exp, act)                 \
  _assertf(act != exp, QUOTE(act), "!= %d", "%d", exp, act)

#define assert_ne_zu(exp, act)                 \
  _assertf(act != exp, QUOTE(act), "!= %zu", "%zu", exp, act)

#define assert_ne_u32(exp, act)                 \
  _assertf(act != exp, QUOTE(act), "!= %"PRIu32, "%"PRIu32, exp, act)

#define assert_ne_u64(exp, act)                 \
  _assertf(act != exp, QUOTE(act), "!= %"PRIu64, "%"PRIu64, exp, act)


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

#define assert_eq_exit(expected, actual)                 \
  _assertf(                                              \
      expected == actual                                 \
    , QUOTE(actual)                                      \
    , "%s_%s(%d)", "%s_%s(%d)"                           \
    , xapi_exit_errtab_name(expected)                    \
    , xapi_exit_errname(expected)                        \
    , expected                                           \
    , xapi_exit_errtab_name(actual)                      \
    , xapi_exit_errname(actual)                          \
    , actual                                             \
  )

#define assert_eq_p(exp, act)                            \
  _assertf(act == exp, QUOTE(act), "%p", "%p", exp, act)

#define assert_null(act)                      \
  _assertf(!act, QUOTE(act), "null", "%p", act)

#define assert_notnull(act)                   \
  _assertf(act, QUOTE(act), "not null", "%p", act)

#if 0
static inline void assert_infos(const char * restrict key, const char * restrict val)
{
  xapi_info_pushs(key, val);
}

static inline void assert_infow(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  xapi_info_pushw(key, vbuf, vlen);
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
#endif

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

#endif
