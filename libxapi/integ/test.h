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

In the xapi integ tests, all assertions are made in main, which is not itself a xapi-enabled function

*/

#define _assertf(bool, value, expfmt, actfmt, ...)                \
  do {                                                            \
    if(!(bool))                                                   \
    {                                                             \
      dprintf(2, "[INTEG:FAIL] failed assertion with"             \
          " value " value ", expected " expfmt ", actual " actfmt \
          " in %s at " __FILE__ ":" XQUOTE(__LINE__) "\n"         \
        , ##__VA_ARGS__, __FUNCTION__                             \
      );                                                          \
      return 1;                                                   \
    }                                                             \
  } while(0)

#define assert_eq_s(exp, act)   \
  _assertf(strcmp(act, exp) == 0, QUOTE(act), "%s", "%s", exp, act)

#define assert_eq_d(exp, act)   \
  _assertf(act == exp, QUOTE(act), "%d", "%d", exp, act)

#define assert_eq_zu(exp, act)   \
  _assertf(act == exp, QUOTE(act), "%zu", "%zu", (size_t)exp, act)

#define assertf(bool, fmt, ...)                                   \
  do {                                                            \
    if(!(bool))                                                   \
    {                                                             \
      dprintf(2, "[INTEG:FAIL] failed assertion, " fmt            \
          " in %s at " __FILE__ ":" XQUOTE(__LINE__) "\n"         \
        , ##__VA_ARGS__, __FUNCTION__                             \
      );                                                          \
      return 1;                                                   \
    }                                                             \
  } while(0)

static inline int sameline(const char * restrict s, const char * restrict a, const char * restrict b)
{
  char * as = strstr(s, a);
  size_t al = strlen(a);
  char * bs = strstr(s, b);

  if(!as || !bs)
    return 0;

  if(bs <= (as + al))
    return 0;

  char * x;
  for(x = as + al; x != bs; x++)
  {
    if(*x == '\n')
      return 0;
  }

  return 1;
}

#if XAPI_STACKTRACE
#define assert_eq_exit(exp, act) \
  _assertf(act == exp, QUOTE(act), "%s(%d)", "%s(%d)", #exp, exp, xapi_exit_errname(act), act)
#else
#define assert_eq_exit(exp, act) \
  _assertf(act == exp, QUOTE(act), "%d", "%d", exp, act)
#endif

#if XAPI_MODE_STACKTRACE
#define MODE "MODE_STACKTRACE"
#elif XAPI_MODE_STACKTRACE_CHECKS
#define MODE "MODE_STACKTRACE_CHECKS"
#elif XAPI_MODE_ERRORCODE
#define MODE "MODE_ERRORCODE"
#endif

#if XAPI_MODE_STACKTRACE || XAPI_MODE_STACKTRACE_CHECKS
#define succeed                                                                     \
  do {                                                                              \
    xapi_teardown();                                                                \
    dprintf(1, "%-25s [INTEG:PASS] at " __FILE__ ":" XQUOTE(__LINE__) "\n", MODE);  \
    return 0;                                                           \
  } while(0)
#endif
#if XAPI_MODE_ERRORCODE
#define succeed                                                         \
  do {                                                                  \
    dprintf(1, "%-25s [INTEG:PASS] at " __FILE__ ":" XQUOTE(__LINE__) "\n", MODE);  \
    return 0;                                                           \
  } while(0)
#endif
