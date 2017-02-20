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

#include "xapi.h"
#include "xapi/exit.h"

#include "xunit.h"
#include "xunit/XUNIT.errtab.h"
#include "macros.h"

#define XU_EQ 1
#define XU_GT 2
#define XU_LT 3
#define XU_NULL 4
#define XU_NOTNULL 5

extern uint32_t xunit_assertions_passed;
extern uint32_t xunit_assertions_failed;

// libxapi-provided assertion data types
extern struct xunit_type * xunit_buffer;
extern struct xunit_type * xunit_string;
extern struct xunit_type * xunit_int;
extern struct xunit_type * xunit_xapi;
extern struct xunit_type * xunit_int64;
extern struct xunit_type * xunit_float;
extern struct xunit_type * xunit_bool;

/// ufail
//
// SUMMARY
//  raise a unit test assertion failure
//
#define assert_eq_s(expected, actual)       _assertion(xunit_string, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_w(exp, expz, act, actz)   _assertion(xunit_buffer, XU_EQ, QUOTE(act), exp, expz, act, actz)
#define assert_eq_d(expected, actual)       _assertion(xunit_int, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_e(expected, actual)       _assertion(xunit_xapi, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_i64(expected, actual)     _assertion(xunit_int64, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_f(expected, actual)       _assertion(xunit_float, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_b(expected, actual)       _assertion(xunit_bool, XU_EQ, QUOTE(actual), expected, actual)

#define _assertion(type, op, value, ...)                                  \
  do {                                                                   \
    if(!xunit_assertion_evaluate(type, op, value, ##__VA_ARGS__)) {      \
      fail(XUNIT_FAIL);                                                  \
    }                                                                    \
  } while(0)

#define ufails(exp, act, ...)                     \
  do {                                            \
    xunit_fails_info(exp, act);                   \
    ufail();                                      \
  } while(0)

#define ufailf(expfmt, actfmt, ...)                   \
  do {                                                \
    xunit_failf_info(expfmt, actfmt, ##__VA_ARGS__);  \
    ufail();                                          \
  } while(0)

/// assert
//
// SUMMARY
//  raise a unit test assertion failure if a condition is false
//
#define assert_eq_s(expected, actual)       _assertion(xunit_string, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_w(exp, expz, act, actz)   _assertion(xunit_buffer, XU_EQ, QUOTE(act), exp, expz, act, actz)
#define assert_eq_d(expected, actual)       _assertion(xunit_int, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_e(expected, actual)       _assertion(xunit_xapi, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_i64(expected, actual)     _assertion(xunit_int64, XU_EQ, QUOTE(actual), expected, actual)
#define assert_eq_f(expected, actual)       _assertion(xunit_float, XU_EQ, QUOTE(actual), expected, actual)

#define _assertion(type, op, value, ...)                                  \
  do {                                                                   \
    if(!xunit_assertion_evaluate(type, op, value, ##__VA_ARGS__)) {      \
      fail(XUNIT_FAIL);                                                  \
    }                                                                    \
  } while(0)

#define restrict __restrict

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
int xunit_assertion_evaluate(const struct xunit_type * const restrict type, uint8_t op, const char * const restrict value, ...)
  __attribute__((nonnull(1)));

#undef restrict
#endif
