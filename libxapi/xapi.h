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

XAPI_MODE_STACKTRACE
XAPI_MODE_ERRORCODE

STACKTRACE mode implies a runtime link against libxapi.so, ERRCODE mode does not

*/

// error table struct
typedef struct etable
{
  // indexed by lower int16 of the error code + (min * -1)
  struct
  {
    char * name;    // e.g. ENOMEM
    char * desc;    // e.g. Not enough space
    char * str;     // e.g. ENOMEM : Not enough space
  } * v;

  char *  tag;      // e.g. "PCRE", "SYS", "FAB", "LW"
  int16_t id;       // upper 2 bytes of the error code, nonzero
  int16_t min;      // min err
  int16_t max;      // max err
} etable;

// an error table for system errors is provided by libxapi
#include "xapi/SYS.errtab.h"

// pull in the appropriate implementation
#if XAPI_MODE_STACKTRACE
# include "xapi/stacktrace.h"
#elif XAPI_MODE_ERRORCODE
# include "xapi/errorcode.h"
#else
# error "neither XAPI_MODE_STACKTRACE nor XAPI_MODE_ERRORCODE is defined"
#endif

#endif
