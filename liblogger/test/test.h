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

#ifndef _LOGGER_TEST_H
#define _LOGGER_TEST_H

#include "xapi.h"
#include "xapi/exit.h"
#include "xapi/errtab.h"

#include "errtab/TEST.errtab.h"

#include "macros.h"

#define restrict __restrict

const char * errtab_name(const etable * const restrict etab);

const char * errtab_errname(const etable * const restrict etab, const xapi exit);

#undef restrict

#define assert(bool, etab, code, fmt, ...)      \
  do                                            \
  {                                             \
    if(!(bool))                                 \
    {                                           \
      tfailf(                                   \
          etab                                  \
        , code                                  \
        , fmt ", result : fail"                 \
        , ##__VA_ARGS__                         \
      );                                        \
    }                                           \
  } while(0)

#define assert_exit(exit, etab, ecode)                                        \
  assert(                                                                     \
         (ecode | xapi_exit_errcode(exit)) == 0                               \
      || (xapi_exit_errtab(exit) == etab && xapi_exit_errcode(exit) == ecode) \
    , perrtab_TEST                                                            \
    , TEST_FAIL                                                               \
    , "expected : %s/%s(%d), actual : %s/%s(%d)"                              \
    , errtab_name(etab)                                                       \
    , errtab_errname(etab, ecode)                                             \
    , ecode                                                                   \
    , xapi_exit_errtab_name(exit)                                             \
    , xapi_exit_errname(exit)                                                 \
    , xapi_exit_errcode(exit)                                                 \
  )

#define succeed                                   \
  do {                                            \
    dprintf(1, "[" __FILE__ ":" XQUOTE(__LINE__) "] result : pass\n"); \
  } while(0)

#endif
