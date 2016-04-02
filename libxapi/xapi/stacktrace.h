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

#ifndef _XAPI_STACKTRACE_H
#define _XAPI_STACKTRACE_H

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

// declarations of frame-manipulation functions (application-visible but not directly called)
#include "xapi/frame.h"

#define restrict __restrict

/*

in xapi-code, you use fail at the site of an error, and fatal to call other xapi-functions

when calling non-xapi code, you have a couple of options.

1) if the function you are calling follows the zero-return-success, nonzero-return-error model
   you can call that function directly with fatalize, and supply the table/code to use in the event
   of failure

    ADVANTAGES    - call it directly
    DISADVANTAGES - cannot supply message, or info k/v/p
                  - will NOT work with a function that provides a freeform error string or which otherwise
                    cannot be made to fit the return code model (like dlopen) - see #3 for this case
    EXAMPLE : fatalize(perrtab_SYS, errno, setrlimit, ...);

2) you can write a non-xapi wrapper function that follows the zero-return-success nonzero-return-error
   model, and call that function with fatalize

   by convention, these wrapper functions are named "w<function>"

    ADVANTAGES/DISADVANTAGES - see above, also you must write the (small) wrapper
    EXAMPLE : common/wstdlib/wrealloc

3) you can write a proxy function that invokes that function, and calls fail on its behalf when an error
   is returned. Any relevant message should be supplied to fail, and info k/v/p provided in the
   finally section of the proxy. You call the proxy with fatal.

   by convention, these proxy functions are named "x<function>"

    ADVANTAGES    - full information : code, message, k/v/p
    DISADVANTAGES - you must write the proxy
    EXAMPLE : libxlinux/xdlfcn/xdlopen

*/

/// enter
//
// SUMMARY
//  must be the first line of any xapi function
//
// DETAILS
//  __xapi_f1       - tracks whether the finalize label has been hit
//  __xapi_topframe - used by xapi_frame_leave to cleanup when the top-level function exits
//  __xapi_frame_index - index of the frame recorded when this function failed
//
#define enter                                         \
  __label__ XAPI_LEAVE, XAPI_FINALIZE, XAPI_FINALLY;  \
  int __xapi_f1 = 0;                                  \
  int __xapi_topframe = !xapi_sentinel;               \
  xapi_sentinel = 1;                                  \
  xapi_frame_index __attribute__((unused)) __xapi_frame_index[2] = { -1, -1 };\
  __xapi_frame_index[0] = xapi_top_frame_index

/*
** called at the site of an error
*/

/// fail
//
// SUMMARY
//  fail the current frame with the specified error and the prevailing error table
//
// ARGUMENTS
//  [table]- error table
//  code   - error code
//  [msg]  - error message
//  [msgl] - error message length (0 for strlen)
//  [fmt]  - format string for error message
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

#define tfails(etab, code, msg) tfailw(etab, code, msg, 0)

#define tfailw(etab, code, msg, msgl)                       \
  do {                                                      \
    /* populate the current stack frame */                  \
    XAPI_FRAME_SET_MESSAGEW(etab, code, msg, msgl);         \
    /* jump to the finally label */                         \
    goto XAPI_FINALIZE;                                     \
  } while(0)

#define tfailf(etab, code, fmt, ...)                         \
  do {                                                       \
    /* populate the current stack frame */                   \
    XAPI_FRAME_SET_MESSAGEF(etab, code, fmt, ##__VA_ARGS__); \
    /* jump to the finally label */                          \
    goto XAPI_FINALIZE;                                      \
  } while(0)

#define fail(code)             tfail (perrtab, code)
#define fails(code, msg)       tfails(perrtab, code, msg)
#define failw(code, msg, msgl) tfailw(perrtab, code, msg, msgl)
#define failf(code, fmt, ...)  tfailf(perrtab, code, fmt, ##__VA_ARGS__)

/*
** called elsewhere in the stack
*/
#define xapi_invoke(func, ...)                              \
  ({                                                        \
      func(__VA_ARGS__);                                    \
      xapi __r = 0;                                         \
      if(xapi_top_frame_index != __xapi_frame_index[0])     \
      {                                                     \
        XAPI_FRAME_SET(0, 0);                               \
        __r = xapi_frame_errval(__xapi_frame_index[0] + 1); \
      }                                                     \
      __r;                                                  \
  })

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

/// xproxy
//
// enables writing 1-liner wrappers around xapi functions
//
#define xproxy(func, ...) return func(__VA_ARGS__)

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

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET(etab, code)	\
	xapi_frame_set(etab, code, __xapi_frame_index[1], __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET_MESSAGEW(etab, code, msg, msgl)	\
	xapi_frame_set_messagew(etab, code, __xapi_frame_index[1], msg, msgl, __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET_MESSAGEF(etab, code, fmt, ...)	\
	xapi_frame_set_messagef(etab, code, __xapi_frame_index[1], fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/*
** called after finally
*/ 

/// XAPI_INFO
//
// SUMMARY
//  add info kvp to the current frame
//
// REMARKS
//  this is a no-op when not unwinding
//
// VARIANTS
//  infos - value string specified as a null-terminated string
//  infow - value string specified as a pointer/length pair
//  infof - value string specified in prinft-style
//
#define XAPI_INFOS(k, vstr)                      \
  XAPI_INFOW(k, vstr, 0)

#define XAPI_INFOW(k, vstr, vlen)                \
  xapi_frame_infow(k, 0, vstr, vlen)

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
//  true while unwinding due to throwing the specified error
//
// PARAMETERS
//  errtab  - pointer to error table
//  errcode - error code
//
#define XAPI_THROWING(c) (XAPI_UNWINDING && XAPI_ERRTAB == (perrtab) && XAPI_ERRCODE == (c))

#undef restrict
#endif
