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

/*

SUMMARY
 application-visible frame manipulation functions used in XAPI_STACKTRACE mode

REMARKS
 none of these functions are called directly by application code; they are accessed
 through the macros provided in xapi/stacktrace.h

*/

#include "xapi.h"

#if XAPI_STACKTRACE

#include <stdint.h>
#define restrict __restrict

struct errtab;
struct stack;

typedef int_fast32_t xapi_frame_index;

extern __thread xapi_frame_index xapi_top_frame_index;
extern __thread int xapi_sentinel;

#if XAPI_MODE_STACKTRACE_CHECKS
/// xapi_frame_*_frame_address
//
// SUMMARY
//  implementation of ILLFATAL and NOFATAL checks
//
// REMARKS
//  with STACKTRACE_CHECKS enabled, fatal/leave are instrumented to call these functions to
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
int xapi_unwinding(void);

/// xapi_frame_errval
//
// SUMMARY
//  get the exit status for the specified frame
//
// PARAMETERS
//  index - index of the frame
//
xapi xapi_frame_errval(xapi_frame_index index);

/// xapi_frame_set
//
// SUMMARY
//  set properties for the current frame during unwinding, and, optionally, apply a single info kvp
//  to a calltree frame
//
// PARAMETERS
//  [exit]         - exit value (nonzero when raising a new error)
//  parent_index   -
//  [key]          - key for a single, optional kvp to apply to the frame
//  [vstr]         - value string
//  [vbuf]         - value buffer
//  [vlen]         - value buffer length
//  [vfmt]         - value in printf-style
//  [file]         - file name
//  [line]         - line number
//  func           - function name
//
void xapi_frame_set(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
  __attribute__((nonnull(5)));

void xapi_frame_set_infos(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vstr
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
  __attribute__((nonnull(7)));

void xapi_frame_set_infow(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vbuf
  , size_t vlen
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
  __attribute__((nonnull(8)));

void xapi_frame_set_infof(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vfmt
  , const char * const restrict file
  , const int line
  , const char * const restrict func
  , ...
)
  __attribute__((nonnull(7)));

///
//
//
//
xapi_code xapi_calltree_errcode(void);
xapi xapi_calltree_errval(void);
const errtab * xapi_calltree_errtab(void);

#undef restrict
#endif
#endif
