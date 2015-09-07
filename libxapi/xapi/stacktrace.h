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

// the system error table is the default
#define perrtab perrtab_SYS

/*
** declared by the application
**  perrtab - pointer to etable
*/

// declarations of frame-manipulation functions (application-visible but not directly called)
#include "xapi/frame.h"

// declarations of trace-description functions
#include "xapi/trace.h"

/// xapi_teardown
//
// SUMMARY
//  release memory
//
void xapi_teardown();

/*
** enable XAPI_RUNTIME_CHECKS to :
**  [x] detect non-UNWIND-ing function invoked with fatal
**  [x] detect UNWIND-ing function invoked without fatal in the presence of an active callstack
*/
#if XAPI_RUNTIME_CHECKS
#include "xapi/XAPI.errtab.h"
#endif

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
    EXAMPLE : common/wstdlib/wmalloc

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
#if XAPI_RUNTIME_CHECKS
#define enter                             \
printf("enter %d\n", __LINE__);       \
  int __xapi_f1 = 0;                      \
  void * __xapi_s = 0;                    \
  int __xapi_sentinel = !xapi_sentinel;   \
  xapi_sentinel = 1;                      \
  xapi_record_frame(xapi_calling_frame_address);  \
  if(xapi_calling_frame_address && xapi_calling_frame_address != __builtin_frame_address(1))  \
  {                                       \
printf("NOFATAL\n");                      \
    tfail(perrtab_XAPI, XAPI_NOFATAL);    \
  }
#else
#define enter                             \
  int __xapi_f1 = 0;                      \
  void * __xapi_s = 0;                    \
  int __xapi_sentinel = !xapi_sentinel;   \
  xapi_sentinel = 1
#endif

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
/*printf("tfail @ %s:%d\n", __FILE__, __LINE__);*/   \
    /* populate the current stack frame */              \
    XAPI_FRAME_SET(etab, code);                         \
    /* jump to the finally label */                     \
    goto XAPI_FINALIZE;                                 \
  } while(0)

#define tfails(etab, code, msg) tfailw(etab, code, msg, 0)

#define tfailw(etab, code, msg, msgl)                       \
  do {                                                      \
/* printf("tfail @ %s:%d\n", __FILE__, __LINE__);*/   \
    /* populate the current stack frame */                  \
    XAPI_FRAME_SET_MESSAGEW(etab, code, msg, msgl);         \
    /* jump to the finally label */                         \
    goto XAPI_FINALIZE;                                     \
  } while(0)

#define tfailf(etab, code, fmt, ...)                         \
  do {                                                       \
/* printf("tfail @ %s:%d\n", __FILE__, __LINE__);*/   \
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

#if XAPI_RUNTIME_CHECKS
#define xapi_invoke(func, ...)                                                                              \
  ({                                                                                                        \
      xapi_calling_frame_address = __builtin_frame_address(0);                                              \
/* printf("INVOKE   __x : %d\n", __x); */                                                                   \
      int __r = func(__VA_ARGS__);                                                                          \
      if(xapi_caller_frame_address != __builtin_frame_address(0))                                           \
      {                                                                                                     \
/* printf("ILLFATAL __r : %d, calling : %p, caller : %p\n", __r, __builtin_frame_address(0), xapi_caller_frame_address); */ \
        __r = 1;                                                                                            \
        XAPI_FRAME_SET_MESSAGEW(perrtab_XAPI, XAPI_ILLFATAL, "function " #func " invoked with fatal", 0);   \
      }                                                                                                     \
      __r;                                                                                                  \
  })
#else
#define xapi_invoke(func, ...)              \
  ({ func(__VA_ARGS__); })
#endif

/// invoke
//
// SUMMARY
//  used in conjunction with xapi_callstack_unwindto to invoke an xapi-enabled function
//  and, when it returns an error, conditionally propagate or discard that error
//
#define invoke(func, ...) xapi_invoke(func, ##__VA_ARGS__)

/// fatal
//
// SUMMARY
//  invoke an UNWIND-ing function and fail the current frame if that function fails
//
// REMARKS
//  fatal can appear in a finally block ; while unwinding, the fatal is a no-op
//
#undef fatal
#define fatal(func, ...)                    \
  do {                                      \
    if(xapi_invoke(func, ##__VA_ARGS__))    \
    {                                       \
      fail(0);                              \
    }                                       \
  } while(0)

/// fatalize
//
// SUMMARY
//  invoke a non-UNWIND-ing function and if it fails, capture its error code and possibly
//  message and fail the current frame using that code/msg and the prevailing error table
//
//  fatalize should not be used in a finally block
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
// enables writing 1-liner wrappers around UNWIND-ing functions
//
#define xproxy(func, ...)       \
  fatal(func, ##__VA_ARGS__);   \
  finally : coda

/// prologue
//
// to be called at the beginning of an UNWIND-ing function which was not itself called with fatal
//  examples : xqsort_r, xnftw
//
#if XAPI_RUNTIME_CHECKS
#define prologue                                      \
  __label__ XAPI_FINALLY;                             \
  __label__ XAPI_FINALIZE;                            \
  __label__ XAPI_LEAVE;                               \
  do {                                                \
    if(xapi_frame_enter(__builtin_frame_address(1)))  \
    {                                                 \
      tfail(perrtab_XAPI, 0);                         \
    }                                                 \
  } while(0)
#else
#define prologue                \
  __label__ XAPI_FINALLY;       \
  __label__ XAPI_FINALIZE;      \
  __label__ XAPI_LEAVE;         \
  do {                          \
    if(xapi_frame_enter())      \
    {                           \
      fail(0);                  \
    }                           \
  } while(0)
#endif

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
  __xapi_f1 = 1;                \
  goto XAPI_FINALLY;            \
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda                            \
  goto XAPI_LEAVE;                      \
XAPI_LEAVE:                             \
do {                                    \
  int rr = xapi_frame_leave(__xapi_sentinel);          \
/*printf("leave %d\n", __LINE__);*/ \
  return rr;                            \
} while(0)

/// conclude
//
// SUMMARY
//  capture the error code from the current function
//
// PARAMETERS
//  [e] - pointer to error table
//  [c] - error code
//  [r] - return value, that is (e->id << 16) | c
//
#define conclude(r)                 \
  goto XAPI_LEAVE;                  \
XAPI_LEAVE:                         \
  (*r) = xapi_frame_leave(__xapi_sentinel)

#define conclude3(e, c, r)          \
  goto XAPI_LEAVE;                  \
XAPI_LEAVE:                         \
  xapi_frame_leave3(__xapi_sentinel, e, c, r)

/*
** called after finally
*/ 

/// XAPI_INFO
//
// SUMMARY
//  add info kvp to the current frame
//
// REMARKS
//  this is a no-op when not unwinding, and the statements are not even evaluated
//
// VARIANTS
//  infos - value string specified as a null-terminated string
//  infow - value string specified as a pointer/length pair
//  infof - value string specified in prinft-style
//
#define XAPI_INFOS(k, vstr)                           \
  XAPI_INFOW(k, vstr, 0)

#define XAPI_INFOW(k, vstr, vlen)                     \
  do {                                                \
    if(xapi_unwinding()) {                            \
      xapi_frame_infow(k, 0, vstr, vlen);             \
    }                                                 \
  } while(0)

#define XAPI_INFOF(k, vfmt, ...)                      \
  do {                                                \
    if(xapi_unwinding()) {                            \
      xapi_frame_infof(k, 0, vfmt, ##__VA_ARGS__);    \
    }                                                 \
  } while(0)

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
#define XAPI_ERRTAB xapi_frame_errtab()

/// XAPI_ERROR
//
// SUMMARY
//  while unwinding, the error id, that is, errtab->id << 16 | errcode
//
#define XAPI_ERRVAL xapi_frame_errval()

/// XAPI_ERRCODE
//
// SUMMARY
//  while unwinding, the error code, and zero otherwise
//
#define XAPI_ERRCODE xapi_frame_errcode()

#undef restrict
#endif
