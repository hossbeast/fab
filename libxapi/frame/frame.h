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

#ifndef _XAPI_FRAME_H
#define _XAPI_FRAME_H

#include <stdint.h>

/*
** application-visible frame manipulation functions used in XAPI_STACKTRACE mode
**
** none of these functions are called directly by application code; they are accessed
** through the macros provided in xapi/stacktrace.h
*/

#define restrict __restrict

struct etable;
struct stack;

typedef int_fast32_t xapi_frame_index;

extern __thread xapi_frame_index xapi_top_frame_index;
extern __thread int xapi_sentinel;

#if XAPI_RUNTIME_CHECKS
/// xapi_frame_*_frame_address
//
// SUMMARY
//  implementation of ILLFATAL and NOFATAL checks
//
// REMARKS
//  with RUNTIME_CHECKS enabled, fatal/leave are instrumented to call these functions to
//  track the addresses of the fatal callstack and raise an error when there is disagreement
//  with the underlying callstack
//
extern __thread void * xapi_calling_frame_address;
extern __thread void * xapi_caller_frame_address;
void xapi_record_frame(void * calling_frame);
extern __thread int xapi_raised_exit;
#endif

/// xapi_frame_leave
//
// SUMMARY
//  pop a frame from the callstack
//
// RETURNS
//  zero when no error has been raised; otherwise, the error id
//
// RETURNS
//  the value for the function return ; zero when no error has been raised, otherwise, the
//  error id, which is a composite of the error table id and the error code
//
xapi xapi_frame_leave(int topframe);

/// xapi_unwinding
//
// SUMMARY
//  true if an error has been raised
//
int xapi_unwinding();

xapi_code xapi_frame_errcode();
xapi xapi_frame_errval();
const etable * xapi_frame_errtab();

/// xapi_frame_set
//
// SUMMARY
//  set properties for the top frame during unwinding
//
// PARAMETERS
//  [s]    - saved pointer to stack
//  [etab] - errtab table (nonnull when code is nonzero)
//  [code] - error code (nonzero when raising a new error)
//  [msg]  - message
//  [msgl] - message length (0 for strlen)
//  [fmt]  - format string
//  [file] - file name
//  [line] - line number
//  func   - function name
//
// VARIANTS
//  messagew - message is specified as pointer/length pair
//  messagef - message is specified in printf/style
//
void xapi_frame_set(
    const struct etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict file
  , const int line
  , const char * const restrict func
);

void xapi_frame_set_messagew(
    const struct etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict msg
  , int msgl
  , const char * const restrict file
  , const int line
  , const char * const restrict func
);

void xapi_frame_set_messagef(
    const struct etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict fmt
  , const char * const restrict file
  , const int line
  , const char * const restrict func
  , ...
);

/// xapi_frame_info
//
// SUMMARY
//  add key/value info to the top frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  kstr   - key string
//  [kl]   - key length or 0 for strlen
//  [vstr] - value string
//  [vl]   - vstr length or 0 for strlen
//  [vfmt] - format string for value
//
// VARIANTS
//  infow - value is specified in as pointer/length pair
//  infof - value is specified in printf/style
//
void xapi_frame_infow(const char * const restrict k, int kl, const char * const restrict vstr, int vlen);
void xapi_frame_infof(const char * const restrict k, int kl, const char * const restrict vfmt, ...);

#undef restrict
#endif
