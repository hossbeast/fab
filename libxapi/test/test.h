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

#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/exit.h"
#include "xapi/errtab.h"
#include "xapi/XAPI.errtab.h"
#include "errtab/TEST.errtab.h"

#undef perrtab
#define perrtab perrtab_TEST

#define MMS(a) a, strlen(a)
#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define XAPI_STACKTRACE_INCL 1
#else
#define XAPI_STACKTRACE_INCL 0
#endif

/*

In the xapi tests, all assertions are made in main, which is not itself a
xapi-enabled function

*/

#define assert(bool, fmt, ...)                             \
  do {                                                     \
    if(!(bool))                                            \
    {                                                      \
      dprintf(2, "[" __FILE__ ":" XQUOTE(__LINE__) "] "    \
          fmt ", result : fail\n"                          \
        , ##__VA_ARGS__                                    \
      );                                                   \
      return 1;                                            \
    }                                                      \
  } while(0)

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define assert_etab(exit, etab)                       \
  assert(                                             \
      xapi_exit_errtab(exit) == etab                  \
    , "expected etab : %s, actual etab : %s"          \
  )
#endif
#if XAPI_MODE_ERRORCODE
#define assert_etab(etab)
#endif

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define assert_code(exit, ecode)                              \
  assert(                                                     \
      xapi_exit_errcode(exit) == ecode                        \
    , "expected code : %s(%d), actual code : %s(%d)"          \
    , #ecode                                                  \
    , ecode                                                   \
    , xapi_exit_errname(exit)                                 \
    , xapi_exit_errcode(exit)                                 \
  )
#endif
#if XAPI_MODE_ERRORCODE
#define assert_code(exit, ecode)                      \
  assert(                                             \
      ((exit) & 0xFFFF) == ecode                      \
    , "expected code : %d, actual code : %d"          \
    , ecode                                           \
    , (exit) & 0xFFFF                                 \
  )
#endif

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define assert_exit(exit, etab, ecode)                                                  \
  assert(                                                                               \
      xapi_exit_errtab(exit) == etab && xapi_exit_errcode(exit) == ecode                \
    , "expected exit : %s/%s(%d), actual exit : %s/%s(%d)"                              \
    , ({ const char * s = 0; if(etab) { s = xapi_errtab_name(etab ?: (void*)1); } s; }) \
    , #ecode                                                                            \
    , ecode                                                                             \
    , xapi_exit_errtab_name(exit)                                                       \
    , xapi_exit_errname(exit)                                                           \
    , xapi_exit_errcode(exit)                                                           \
  )
#endif
#if XAPI_MODE_ERRORCODE
#define assert_exit(exit, etab, ecode)          \
  assert_code(exit, ecode)
#endif

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define succeed                                                         \
  do {                                                                  \
    xapi_teardown();                                                    \
    dprintf(1, "[" __FILE__ ":" XQUOTE(__LINE__) "] result : pass\n");  \
    return 0;                                                           \
  } while(0)
#endif
#if XAPI_MODE_ERRORCODE
#define succeed                                                         \
  do {                                                                  \
    dprintf(1, "[" __FILE__ ":" XQUOTE(__LINE__) "] result : pass\n");  \
    return 0;                                                           \
  } while(0)
#endif
