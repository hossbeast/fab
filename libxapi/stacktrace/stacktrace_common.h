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

// declarations of frame-manipulation functions (application-visible but not directly called)
#include "xapi/frame.h"

#define restrict __restrict

/*
** called at the site of an error
*/

/// fail
//
// SUMMARY
//  fail the current frame with the specified error and the prevailing error table
//
// ARGUMENTS
//  [etab]  - error table
//  code    - error code
//  [key]   - key for an info kvp to add to the frame
//  [vstr]  - error message
//  [vbuf]  - error message length (0 for strlen)
//  [vbufl] - error message length (0 for strlen)
//  [fmt]   - format string for error message
//
// VARIANTS
//  tfail - specify the error table
//  fails - error message specified as null-terminated string
//  failw - error message specified as pointer/length pair
//  failf - error message specified in printf/style
//
#define tfail(etab, code)                               \
  do {                                                  \
    /* populate the current stack frame */              \
    XAPI_FRAME_SET(etab, code);                         \
    /* jump to the finally label */                     \
    goto XAPI_FINALIZE;                                 \
  } while(0)

#define tfails(etab, code, key, vstr)   \
  tfailw(etab, code, key, vstr, 0)

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
#define failf(code, key, fmt, ...)    tfailf(perrtab, code, key, fmt, ##__VA_ARGS__)

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
// ARGUMENTS
//  [table]- error table
//  code   - error code returned from the function
//  [msg]  - error message returned from the function
//  [msgl] - error message length (0 for strlen)
//
// VARIANTS
//  fatalizes - error message specified as null-terminated string
//  fatalizew - error message specified as pointer/length pair
//
#define tfatalize(etab, code, func, ...)                          \
  do {                                                            \
    if(func(__VA_ARGS__))                                         \
      tfail(etab, code);                                          \
  } while(0)

#define tfatalizes(etab, code, msg, func, ...)                    \
  tfatalizew(etab, code, msg, 0, func, ##__VA_ARGS__)

#define tfatalizew(etab, code, msg, msgl, func, ...)              \
  do {                                                            \
    if(func(__VA_ARGS__))                                         \
      tfailw(etab, code, msg, msgl);                              \
  } while(0)

#define fatalize(code, func, ...)  tfatalize (perrtab, code, func, ##__VA_ARGS__)
#define fatalizes(code, func, ...) tfatalizes(perrtab, code, func, ##__VA_ARGS__)
#define fatalizew(code, func, ...) tfatalizew(perrtab, code, func, ##__VA_ARGS__)

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
  XAPI_FRAME_SET_INFOW(etab, code, key, vstr, 0)

// call xapi_frame_set with the current location, and a single key/value info pair
#define XAPI_FRAME_SET_INFOW(etab, code, key, vbuf, vlen)	\
	xapi_frame_set_infow(etab, code, __xapi_frame_index[1], key, vbuf, vlen, __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with the current location, and a single key/value info pair
#define XAPI_FRAME_SET_INFOF(etab, code, key, vfmt, ...)	\
	xapi_frame_set_infof(etab, code, __xapi_frame_index[1], key, vfmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/*
** called after finally
*/ 

/// XAPI_INFO
//
// SUMMARY
//  apply an info kvp to a calltree frame
//
// REMARKS
//  Before unwinding, infos are staged to be applied to the base frame. There is no way to unstage, so
//  infos should only be staged when a fail call is imminent. Otherwise, xapi_info should only
//  be called in a finally block, and infos are applied to the frame for that function invocation.
//
// VARIANTS
//  infos - value string specified as a null-terminated string
//  infow - value string specified as a buffer/length pair
//  infof - value string specified in prinft-style
//
#define XAPI_INFOS(k, vstr)                      \
  XAPI_INFOW(k, vstr, 0)

#define XAPI_INFOW(k, vbuf, vlen)                \
  xapi_frame_infow(k, 0, vbuf, vlen)

#define XAPI_INFOF(k, vfmt, ...)                 \
  xapi_frame_infof(k, 0, vfmt, ##__VA_ARGS__)

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
