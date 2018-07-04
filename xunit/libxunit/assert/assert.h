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

#ifndef _XUNIT_ASSERT_H
#define _XUNIT_ASSERT_H

/*

MODULE
 xunit/assert

SUMMARY
 test assertions that fail with XUNIT_FAIL

REMARKS
 macros instead of static inline to preserve file name and line number, pending address based
 location support in libxapi

*/

#include <string.h>

#include "types.h"

#include "xapi.h"
#include "xapi/exit.h"
#include "xapi/info.h"

#include "xunit.h"
#include "xunit/XUNIT.errtab.h"
#include "macros.h"

#define XU_EQ 1         // equal to
#define XU_NE 2         // not equal to
#define XU_GT 3         // greater than
#define XU_GE 4         // greater than or equal to
#define XU_LT 5         // less than
#define XU_LE 6         // less than or equal to
#define XU_NULL 7       // is null
#define XU_NOTNULL 8    // is not null

extern uint32_t xunit_assertions_passed;
extern uint32_t xunit_assertions_failed;

// libxapi-provided assertion data types
extern struct xunit_type * xunit_buffer;
extern struct xunit_type * xunit_string;
extern struct xunit_type * xunit_int;
extern struct xunit_type * xunit_char;
extern struct xunit_type * xunit_xapi;
extern struct xunit_type * xunit_int32;
extern struct xunit_type * xunit_uint32;
extern struct xunit_type * xunit_int64;
extern struct xunit_type * xunit_uint64;
extern struct xunit_type * xunit_float;
extern struct xunit_type * xunit_bool;
extern struct xunit_type * xunit_pointer;

/// assert
//
// SUMMARY
//  raise a unit test assertion failure if a condition is false
//
#define assert_true(actual)                 _assertion(xunit_bool, XU_EQ, QUOTE(actual), true, actual)
#define assert_false(actual)                _assertion(xunit_bool, XU_EQ, QUOTE(actual), false, actual)

#define assert_eq_b(expected, actual)       _assertion(xunit_bool, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_b(expected, actual)       _assertion(xunit_bool, XU_NE, QUOTE(actual), expected, actual)

#define assert_eq_c(expected, actual)       _assertion(xunit_char, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_c(expected, actual)       _assertion(xunit_char, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_c(expected, actual)       _assertion(xunit_char, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_c(expected, actual)       _assertion(xunit_char, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_c(expected, actual)       _assertion(xunit_char, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_c(expected, actual)       _assertion(xunit_char, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_d(expected, actual)       _assertion(xunit_int, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_d(expected, actual)       _assertion(xunit_int, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_d(expected, actual)       _assertion(xunit_int, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_d(expected, actual)       _assertion(xunit_int, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_d(expected, actual)       _assertion(xunit_int, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_d(expected, actual)       _assertion(xunit_int, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_e(expected, actual)       _assertion(xunit_xapi, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_e(expected, actual)       _assertion(xunit_xapi, XU_NE, QUOTE(actual), expected, actual)

#define assert_eq_f(expected, actual)       _assertion(xunit_float, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_f(expected, actual)       _assertion(xunit_float, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_f(expected, actual)       _assertion(xunit_float, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_f(expected, actual)       _assertion(xunit_float, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_f(expected, actual)       _assertion(xunit_float, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_f(expected, actual)       _assertion(xunit_float, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_u32(expected, actual)     _assertion(xunit_uint32, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_u32(expected, actual)     _assertion(xunit_uint32, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_u32(expected, actual)     _assertion(xunit_uint32, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_u32(expected, actual)     _assertion(xunit_uint32, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_u32(expected, actual)     _assertion(xunit_uint32, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_u32(expected, actual)     _assertion(xunit_uint32, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_i64(expected, actual)     _assertion(xunit_int64, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_i64(expected, actual)     _assertion(xunit_int64, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_i64(expected, actual)     _assertion(xunit_int64, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_i64(expected, actual)     _assertion(xunit_int64, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_i64(expected, actual)     _assertion(xunit_int64, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_i64(expected, actual)     _assertion(xunit_int64, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_u64(expected, actual)     _assertion(xunit_uint64, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_u64(expected, actual)     _assertion(xunit_uint64, XU_NE, QUOTE(actual), expected, actual)
#define assert_gt_u64(expected, actual)     _assertion(xunit_uint64, XU_GT, QUOTE(actual), expected, actual)
#define assert_ge_u64(expected, actual)     _assertion(xunit_uint64, XU_GE, QUOTE(actual), expected, actual)
#define assert_lt_u64(expected, actual)     _assertion(xunit_uint64, XU_LT, QUOTE(actual), expected, actual)
#define assert_le_u64(expected, actual)     _assertion(xunit_uint64, XU_LE, QUOTE(actual), expected, actual)

#define assert_eq_s(expected, actual)       _assertion(xunit_string, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_s(expected, actual)       _assertion(xunit_string, XU_NE, QUOTE(actual), expected, actual)

#define assert_eq_w(expected, expected_size, actual, actual_size) \
                                            _assertion(xunit_buffer, XU_EQ, QUOTE(actual), expected, expected_size, actual, actual_size)
#define assert_ne_w(expected, expected_size, actual, actual_size) \
                                            _assertion(xunit_buffer, XU_NE, QUOTE(actual), expected, expected_size, actual, actual_size)

#define assert_eq_p(expected, actual)       _assertion(xunit_pointer, XU_EQ, QUOTE(actual), expected, actual)
#define assert_null(actual)                 _assertion(xunit_pointer, XU_NULL, QUOTE(actual), actual)
#define assert_notnull(actual)              _assertion(xunit_pointer, XU_NOTNULL, QUOTE(actual), actual)
#define assert_nullity(expected, actual)    _assertion(xunit_pointer, expected ? XU_NULL : XU_NOTNULL, QUOTE(actual), actual)

#define _assertion(type, op, value, ...)                                 \
  do {                                                                   \
    if(!xunit_assertion_evaluate(type, op, value, ##__VA_ARGS__)) {      \
      fail(XUNIT_FAIL);                                                  \
    }                                                                    \
  } while(0)

/// xunit_assertion_evaluate
//
// SUMMARY
//  evaluate an assertion
//
// PARAMETERS
//  type    - data
//  op      -
//  [value] -
//
// RETURNS
//  boolean value indicating whether the assertion passed
//
bool xunit_assertion_evaluate(const struct xunit_type * const restrict type, uint8_t op, const char * const restrict value, ...)
  __attribute__((nonnull(1)));

#if XAPI_STACKTRACE
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
#endif

#endif
