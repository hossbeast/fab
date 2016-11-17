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
 macros instead of static inline to preserve file name and line number

*/

#include <string.h>

#include "xapi.h"
#include "xapi/exit.h"

#include "xunit.h"
#include "xunit/XUNIT.errtab.h"

void xunit_failf_info(const char * const restrict expfmt, const char * const restrict actfmt, ...)
  __attribute__((nonnull(1, 2)));

void xunit_fails_info(const char * const restrict exp, const char * const restrict act)
  __attribute__((nonnull));

extern __thread uint32_t xunit_assertions_passed;
extern __thread uint32_t xunit_assertions_failed;

/// ufail
//
// SUMMARY
//  raise a unit test assertion failure
//
#define ufail()                       \
  do {                                \
    xunit_assertions_failed++;        \
    fail(XUNIT_FAIL); \
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
#define assert(bool)              \
  do {                            \
    if(!(bool))                   \
      ufail();                    \
    else                          \
      xunit_assertions_passed++;  \
  } while(0)

#define asserts(bool, exp, act, ...)          \
  do {                                        \
    if(!(bool))                               \
      ufails(exp, act);                       \
    else                                      \
      xunit_assertions_passed++;              \
  } while(0)

#define assertf(bool, expfmt, actfmt, ...)    \
  do {                                        \
    if(!(bool))                               \
      ufailf(expfmt, actfmt, ##__VA_ARGS__);  \
    else                                      \
      xunit_assertions_passed++;              \
  } while(0)

/// assert_exit
//
// SUMMARY
//  special-purpose assert for exit values
//
#define assert_exit(expected, actual)         \
  assertf(                                    \
      expected == actual                      \
    , "%s(%d)", "%s(%d)"                      \
    , xapi_exit_errname(expected)             \
    , expected                                \
    , xapi_exit_errname(actual)               \
    , actual                                  \
  )

#define assert_eq_d(exp, act)                 \
  assertf(act == exp, "%d", "%d", exp, act)

#define assert_eq_s(exp, act, ...)            \
  asserts(strcmp(act, exp) == 0, exp, act)

#endif
