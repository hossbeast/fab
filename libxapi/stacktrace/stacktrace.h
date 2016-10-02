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

#if XAPI_STACKTRACE

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

// common to all stacktrace modes
#include "xapi/stacktrace_common.h"

#define restrict __restrict

/// enter
//
// SUMMARY
//  must be the first line of any xapi function
//
// DETAILS
//  __xapi_f1            - true if the finalize label has been hit
//  __xapi_topframe      - true if this is a top-level frame
//  __xapi_base_frame    - index of the base frame for this invocation
//  __xapi_current_frame - index of the current frame for this invocation
//
#define enter                                                                           \
  __label__ XAPI_LEAVE, XAPI_FINALIZE, XAPI_FINALLY;                                    \
  int __xapi_f1 = 0;                                                                    \
  int __xapi_topframe = !xapi_sentinel;                                                 \
  xapi_sentinel = 1;                                                                    \
  xapi_frame_index __attribute__((unused)) __xapi_base_frame = xapi_top_frame_index;    \
  xapi_frame_index __attribute__((unused)) __xapi_current_frame = xapi_top_frame_index

#define enter_nochecks enter

/*
** called elsewhere in the stack
*/
#define xapi_invoke(func, ...)                              \
  ({                                                        \
      func(__VA_ARGS__);                                    \
      xapi __r = 0;                                         \
      if(xapi_top_frame_index != __xapi_current_frame)      \
      {                                                     \
        XAPI_FRAME_SET(0);                                  \
        __r = xapi_frame_errval(__xapi_current_frame + 1);  \
      }                                                     \
      __r;                                                  \
  })

/// xproxy
//
// enables writing 1-liner wrappers around xapi functions
//
#define xproxy(func, ...) return func(__VA_ARGS__)

#endif
#endif
