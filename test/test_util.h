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

*/

#include "xapi.h"
#include "xapi/exit.h"
#include "xapi/errtab.h"

#include "errtab/TEST.errtab.h"

#include "macros.h"

void ufailf_info(const char * const restrict expfmt, const char * const restrict actfmt, ...)
  __attribute__((nonnull(1, 2)));

#define ufail()                   \
  tfail(perrtab_TEST, TEST_FAIL)

#define ufailf(expfmt, actfmt, ...)               \
  do {                                            \
    ufailf_info(expfmt, actfmt, ##__VA_ARGS__);   \
    ufail();                                      \
  } while(0)

#define assert(bool)      \
  do {                    \
    if(!(bool))           \
    {                     \
      ufail();            \
    }                     \
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
    , "expected : %s/%s(%d), actual : %s/%s(%d)"                              \
    , etab ? xapi_errtab_name(etab) : 0                                       \
    , etab ? xapi_errtab_errname(etab, ecode) : 0                             \
    , ecode                                                                   \
    , xapi_exit_errtab_name(exit)                                             \
    , xapi_exit_errname(exit)                                                 \
    , xapi_exit_errcode(exit)                                                 \
  )

#define success                                                        \
  do {                                                                 \
    dprintf(1, "[" __FILE__ ":" XQUOTE(__LINE__) "] result : pass\n"); \
  } while(0)

#endif
