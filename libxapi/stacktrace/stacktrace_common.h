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

#ifndef _XAPI_STACKTRACE_COMMON_H
#define _XAPI_STACKTRACE_COMMON_H

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

// declarations of frame-manipulation functions (application-visible but not directly called)
#include "xapi/frame.h"
#include "xapi/info.h"

#define restrict __restrict

/*
** called at the site of an error
*/

/// fail
//
// SUMMARY
//  fail the current frame with the specified error and the prevailing error table
//
// REMARKS
//  it is possible to inline a single info kvp with a fail call. If more than one info kvp
//  is to be applied, use xapi_info
//
// PARAMETERS 
//  [etab]  - error table, otherwise use the prevailing error table
//  code    - error code
//  [key]   - key for an info kvp to add to the frame
//  [vstr]  - value string
//  [vbuf]  - value buffer
//  [vbufl] - value buffer length
//  [vfmt]  - value in printf-style
//
#define tfail(etab, code)                               \
  do {                                                  \
    /* populate the current stack frame */              \
    XAPI_FRAME_SET(etab, code);                         \
    /* jump to the finally label */                     \
    goto XAPI_FINALIZE;                                 \
  } while(0)

#define tfails(etab, code, key, vstr)   \
  tfailw(etab, code, key, vstr, strlen(vstr))

#define tfailw(etab, code, key, vbuf, vlen)               \
  do {                                                    \
    /* populate the current stack frame */                \
    XAPI_FRAME_SET_INFOW(etab, code, key, vbuf, vlen);    \
    /* jump to the finally label */                       \
    goto XAPI_FINALIZE;                                   \
  } while(0)

#define tfailf(etab, code, key, vfmt, ...)                      \
  do {                                                          \
    /* populate the current stack frame */                      \
    XAPI_FRAME_SET_INFOF(etab, code, key, vfmt, ##__VA_ARGS__); \
    /* jump to the finally label */                             \
    goto XAPI_FINALIZE;                                         \
  } while(0)

#define fail(code)                    tfail (perrtab, code)
#define fails(code, key, vstr)        tfails(perrtab, code, key, vstr)
#define failw(code, key, vbuf, vbufl) tfailw(perrtab, code, key, vbuf, vbufl)
#define failf(code, key, vfmt, ...)   tfailf(perrtab, code, key, vfmt, ##__VA_ARGS__)

/// fail_intent
//
// SUMMARY
//  allows to stage info kvps just before calling fail, see xapi_info
//
#define fail_intent() xapi_fail_intent()

/// invoke
//
// SUMMARY
//  used in conjunction with xapi_callstack_unwind to invoke a xapi-enabled function
//  and, when it returns an error, conditionally propagate or discard that error
//
#define invoke(func, ...) xapi_invoke(func, ##__VA_ARGS__)

/// fatal
//
// SUMMARY
//  invoke a xapi function and fail the current frame if that function fails
//
// REMARKS
//  fatal can appear in a finally block ; while unwinding, the fatal is a no-op
//
#undef fatal
#define fatal(func, ...)                                \
  do {                                                  \
    xapi_invoke(func, ##__VA_ARGS__);                   \
    if(xapi_top_frame_index != __xapi_frame_index[0])   \
    {                                                   \
      goto XAPI_FINALIZE;                               \
    }                                                   \
  } while(0)

/// fatalize
//
// SUMMARY
//  invoke a non-xapi function which follows the nonzero-return-error pattern and, if it fails,
//  capture its error code and fail the current frame using that code and the prevailing error table
//
// REMARKS
//  fatalize is typically used in a small wrapper for some underlying function. A new frame is not
//  created for the underlying function, its failures are "subsumed" into the calling frame
//
// PARAMETERS
//  [etab] - error table
//  code   - error code returned from the function
//
#define tfatalize(etab, code, func, ...)                          \
  do {                                                            \
    if(func(__VA_ARGS__))                                         \
      tfail(etab, code);                                          \
  } while(0)

#define fatalize(code, func, ...)  tfatalize (perrtab, code, func, ##__VA_ARGS__)

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail
//
#define finally                 \
  goto XAPI_FINALIZE;           \
XAPI_FINALIZE:                  \
  if(__xapi_f1)                 \
  {                             \
    goto XAPI_LEAVE;            \
  }                             \
  __xapi_frame_index[0] = xapi_top_frame_index; \
  __xapi_frame_index[1] = xapi_top_frame_index; \
  __xapi_f1 = 1;                \
  goto XAPI_FINALLY;            \
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda                              \
  goto XAPI_LEAVE;                        \
XAPI_LEAVE:                               \
  return xapi_frame_leave(__xapi_topframe)

/// conclude
//
// SUMMARY
//  capture the return value for the current function
//
// PARAMETERS
//  [r] - pointer to int, receives the return value
//
#define conclude(r)                 \
  goto XAPI_LEAVE;                  \
XAPI_LEAVE:                         \
  (*r) = xapi_frame_leave(__xapi_topframe)

// call xapi_frame_set with the current location
#define XAPI_FRAME_SET(etab, code)	\
	xapi_frame_set(etab, code, __xapi_frame_index[1], __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with the current location, and a single key/value info pair
#define XAPI_FRAME_SET_INFOS(etab, code, key, vstr) \
  XAPI_FRAME_SET_INFOW(etab, code, key, vstr, strlen(vstr))

// call xapi_frame_set with the current location, and a single key/value info pair
#define XAPI_FRAME_SET_INFOW(etab, code, key, vbuf, vlen)	\
	xapi_frame_set_infow(etab, code, __xapi_frame_index[1], key, vbuf, vlen, __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with the current location, and a single key/value info pair
#define XAPI_FRAME_SET_INFOF(etab, code, key, vfmt, ...)	\
	xapi_frame_set_infof(etab, code, __xapi_frame_index[1], key, vfmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/*
** called after finally
*/ 

/// xapi_info
//
// SUMMARY
//  apply an info kvp to a calltree frame
//
// REMARKS
//  Before unwinding, but after calling xapi_fail_intent, infos are staged to be applied to the base
//  frame. There is no way to unstage, so infos should only be staged when a fail call is imminent.
//  Otherwise, xapi_info should only be called in a finally block, and infos are applied to the
//  frame for that function invocation.
//
#define xapi_infos(key, vstr)                      \
  xapi_info_addw(key, vstr, strlen(vstr))

#define xapi_infow(key, vbuf, vlen)                \
  xapi_info_addw(key, vbuf, vlen)

#define xapi_infof(key, vfmt, ...)                 \
  xapi_info_addf(key, vfmt, ##__VA_ARGS__)

#define xapi_vinfof(key, vfmt, va)                 \
  xapi_info_vaddf(key, vfmt, va)

/// XAPI_UNWINDING
//
// SUMMARY
//  true after fatal has been called on this thread and before xapi_unwind
//
#define XAPI_UNWINDING xapi_unwinding()

/// XAPI_ERRTAB
//
// SUMMARY
//  while unwinding, const pointer to the error table, and zero otherwise
//
#define XAPI_ERRTAB xapi_calltree_errtab()

/// XAPI_ERROR
//
// SUMMARY
//  while unwinding, the exit value, that is, errtab->id << 16 | errcode
//
#define XAPI_ERRVAL xapi_calltree_errval()

/// XAPI_ERRCODE
//
// SUMMARY
//  while unwinding, the error code, and zero otherwise
//
#define XAPI_ERRCODE xapi_calltree_errcode()

/// XAPI_THROWING
//
// SUMMARY
//  true while unwinding if the specified error is being thrown
//
// PARAMETERS
//  errtab  - pointer to error table
//  errcode - error code
//
#define XAPI_THROWING(c) (XAPI_UNWINDING && XAPI_ERRTAB == (perrtab) && XAPI_ERRCODE == (c))

#undef restrict
#endif
