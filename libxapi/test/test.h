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
#include "xapi/error.h"
#include "xapi/errtab.h"
#include "xapi/XAPI.errtab.h"

#undef perrtab
#define perrtab perrtab_XAPI

#define MMS(a) a, strlen(a)

/*

In the xapi tests, all assertions are made in main, which is not itself a
xapi-enabled function

*/

#if XAPI_MODE_STACKTRACE
#define assert_etab(etab)                                                                 \
  do {                                                                                    \
    const struct etable * tab = xapi_errtab(exit);                                        \
    if(tab != etab)                                                                       \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] expected etab : %s, actual etab : %s, result : fail\n"   \
        , xapi_errtab_tag(etab)                                                           \
        , xapi_errtab_tag(tab)                                                            \
      );                                                                                  \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)
#endif
#if XAPI_MODE_ERRORCODE
#define assert_etab(etab)
#endif

#if XAPI_MODE_STACKTRACE
#define assert_code(ecode)                                                                \
  do {                                                                                    \
    if(xapi_errcode(exit) != ecode)                                                       \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] expected code : %s(%d), actual code : %s(%d), result : fail\n"   \
        , #ecode                                                                          \
        , ecode                                                                           \
        , xapi_errname(exit)                                                              \
        , xapi_errcode(exit)                                                              \
      );                                                                                  \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)
#endif
#if XAPI_MODE_ERRORCODE
#define assert_code(ecode)                                                                \
  do {                                                                                    \
    int code = exit & 0xFFFF;                                                             \
    if(code != ecode)                                                                     \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] expected code : %d, actual code : %d, result : fail\n"   \
        , ecode                                                                           \
        , code                                                                            \
      );                                                                                  \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)
#endif

#if XAPI_MODE_STACKTRACE
#define assert_exit(etab, ecode)                                                          \
  do {                                                                                    \
    const struct etable * tab = xapi_errtab(exit);                                        \
    if(tab != etab || xapi_errcode(exit) != ecode)                                        \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] expected etab : %s, code : %s(%d), actual etab : %s, code : %s(%d), result : fail\n" \
        , xapi_errtab_tag(etab)                                                           \
        , #ecode                                                                          \
        , ecode                                                                           \
        , xapi_errtab_tag(tab)                                                            \
        , xapi_errname(exit)                                                              \
        , xapi_errcode(exit)                                                              \
      );                                                                                  \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)
#endif
#if XAPI_MODE_ERRORCODE
#define assert_exit(etab, ecode)                                                          \
  do {                                                                                    \
    int code = exit & 0xFFFF;                                                             \
    if(code != ecode)                                                                     \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] expected code : %d, actual code : %d, result : fail\n"   \
        , ecode                                                                           \
        , code                                                                            \
      );                                                                                  \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)
#endif

#define assert(bool, fmt, ...)                                                            \
  do {                                                                                    \
    if(!(bool))                                                                           \
    {                                                                                     \
      dprintf(2, "[" __FILE__ "] " fmt ", result : fail\n", ##__VA_ARGS__);               \
      return 1;                                                                           \
    }                                                                                     \
  } while(0)

#if XAPI_MODE_STACKTRACE
#define succeed                                   \
  xapi_teardown();                                \
  dprintf(1, "[" __FILE__ "] result : pass\n");   \
  return 0
#endif
#if XAPI_MODE_ERRORCODE
#define succeed                                   \
  dprintf(1, "[" __FILE__ "] result : pass\n");   \
  return 0
#endif
