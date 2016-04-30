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

#ifndef _XAPI_H
#define _XAPI_H

#include <stdint.h>

/*

xapi is a calling convention in which the return value of a function communicates
its success or failure. These header(s) provide macros that facilitate the application
of the calling convention.

To use this library, you must specify an error propagation mode by defining one
of the following macros:

XAPI_MODE_STACKTRACE_CHECKS
XAPI_MODE_STACKTRACE
XAPI_MODE_ERRORCODE

STACKTRACE* modes require a runtime link against libxapi.so, ERRCODE does not

*/

// return type for xapi functions, called its exit value
typedef uint32_t xapi;

// types for components of xapi exit value
typedef uint16_t xapi_code;
typedef uint16_t xapi_etable_id;

// error table struct
typedef struct etable
{
  // indexed by lower uint16 of the error code + (min * -1)
  struct
  {
    char * name;         // e.g. ENOMEM
    char * desc;         // e.g. Not enough space
    char * str;          // e.g. ENOMEM : Not enough space
  } * v;

  char *          name;  // e.g. "PCRE", "SYS", "FAB", "LW"
  xapi_etable_id  id;    // upper uint16 of an exit value, nonzero
  xapi_code       min;   // min err
  xapi_code       max;   // max err
} etable;

// pull in the appropriate implementation
#if XAPI_MODE_STACKTRACE_CHECKS
# include "xapi/stacktrace_checks.h"
#elif XAPI_MODE_STACKTRACE
# include "xapi/stacktrace.h"
#elif XAPI_MODE_ERRORCODE
# include "xapi/errorcode.h"
#else
# error "one of { XAPI_MODE_STACKTRACE_CHECKS, XAPI_MODE_STACKTRACE, XAPI_MODE_ERRORCODE } must be defined"
#endif

/// xapi_teardown
//
// SUMMARY
//  release memory
//
void xapi_teardown();

#endif
