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

*/

#include "xunit.h"
#include "xunit/XUNIT.errtab.h"

void xunit_failf_info(const char * const restrict expfmt, const char * const restrict actfmt, ...)
  __attribute__((nonnull(1, 2)));

void xunit_fails_info(const char * const restrict exp, const char * const restrict act)
  __attribute__((nonnull));

#define ufail()                   \
  tfail(perrtab_XUNIT, XUNIT_FAIL)

#define ufailf(expfmt, actfmt, ...)                   \
  do {                                                \
    xunit_failf_info(expfmt, actfmt, ##__VA_ARGS__);  \
    ufail();                                          \
  } while(0)

#define ufails(exp, act, ...)                     \
  do {                                            \
    xunit_fails_info(exp, act);                   \
    ufail();                                      \
  } while(0)

#define assert(bool)      \
  do {                    \
    if(!(bool))           \
    {                     \
      ufail();            \
    }                     \
  } while(0)

#define asserts(bool, exp, act, ...)          \
  do {                                        \
    if(!(bool))                               \
    {                                         \
      ufails(exp, act);                       \
    }                                         \
  } while(0)

#define assertf(bool, expfmt, actfmt, ...)    \
  do {                                        \
    if(!(bool))                               \
    {                                         \
      ufailf(expfmt, actfmt, ##__VA_ARGS__);  \
    }                                         \
  } while(0)

#define assert_exit(exit, etab, ecode)                                        \
  assertf(                                                                    \
         (ecode | xapi_exit_errcode(exit)) == 0                               \
      || (xapi_exit_errtab(exit) == etab && xapi_exit_errcode(exit) == ecode) \
    , "%s/%s(%d)", "%s/%s(%d)"                                                \
    , etab ? xapi_errtab_name(etab) : 0                                       \
    , etab ? xapi_errtab_errname(etab, ecode) : 0                             \
    , ecode                                                                   \
    , xapi_exit_errtab_name(exit)                                             \
    , xapi_exit_errname(exit)                                                 \
    , xapi_exit_errcode(exit)                                                 \
  )

#endif
