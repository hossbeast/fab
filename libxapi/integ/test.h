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
#include "integ/errtab/TEST.errtab.h"

#define MMS(a) a, strlen(a)
#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

/*

In the xapi tests, all assertions are made in main, which is not itself a
xapi-enabled function

*/

#define assertf(bool, fmt, ...)                            \
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

#if XAPI_STACKTRACE
#define assert_exit(expected, actual)                                   \
  assertf(                                                              \
      expected == actual                                                \
    , "expected exit : %s(%d), actual exit : %s(%d)"                    \
    , #expected                                                         \
    , expected                                                          \
    , xapi_exit_errname(actual)                                         \
    , actual                                                            \
  )
#else
#define assert_exit(expected, actual)                                   \
  assertf(                                                              \
      expected == actual                                                \
    , "expected exit : %d, actual exit : %d"                            \
    , expected                                                          \
    , actual                                                            \
  )
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
