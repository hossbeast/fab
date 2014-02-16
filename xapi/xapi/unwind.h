/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _XAPI_UNWIND_H
#define _XAPI_UNWIND_H

/*
** declared by the application
**  perrtab - pointer to etable
*/

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

2) you can write a wrapper function that follows the zero-return-success nonzero-return-error
   model, and call that function with fatalize

	 by convention, these wrapper functions are named "w<function>"

    ADVANTAGES/DISADVANTAGES - see above, also you must write the (small) wrapper
		EXAMPLE : wstdlib/wmalloc

3) you can write a proxy function that invokes that function, and calls fatality when an error
   occurs. Any relevant message should be supplied to fatality, and info k/v/p provided in the
   finally section of the proxy. You call the proxy with fatal.

   by convention, these proxy functions are named "x<function>"

    ADVANTAGES    - full information : code, message, k/v/p
    DISADVANTAGES - you must write the proxy
		EXAMPLE : xdlfcn/xdlopen

*/

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
//  fails - error message specified as null-terminated string
//  failw - error message specified as pointer/length pair
//  failf - error message specified in printf/style
//
#define fail(code)																				\
	do {																										\
		/* populate the current stack frame */								\
		if(XAPI_FRAME_SET(perrtab, code))											\
		{																											\
			/* jump to the finally label */											\
			goto XAPI_FINALIZE;																	\
		}																											\
	} while(0)

#define fails(code, msg) failw(code, msg, 0)

#define failw(code, msg, msgl)														\
	do {																										\
		/* populate the current stack frame */								\
		if(XAPI_FRAME_SET_MESSAGEW(perrtab, code, msg, msgl))	\
		{																											\
			/* jump to the finally label */											\
			goto XAPI_FINALIZE;																	\
		}																											\
	} while(0)

#define failf(code, fmt, ...)																				\
	do {																															\
		/* populate the current stack frame */													\
		if(XAPI_FRAME_SET_MESSAGEF(perrtab, code, fmt, ##__VA_ARGS__))	\
		{																																\
			/* jump to the finally label */																\
			goto XAPI_FINALIZE;																						\
		}																																\
	} while(0)

/*
** called elsewhere in the stack
*/

/// fatal
//
// SUMMARY
//  invoke an UNWIND-ing function and fail the current frame if that function fails
//
#define fatal(func, ...)																																	\
	do {																																										\
		if(xapi_frame_enter() != -1 && (xapi_frame_enter_last() == 1 || func(__VA_ARGS__)))		\
			fail(0);																																						\
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
#define fatalize(code, func, ...)																	\
	do {																														\
		if(func(__VA_ARGS__))																					\
			fail(code);																									\
	} while(0)

#define fatalizes(code, msg, func, ...)														\
	fatalizew(code, msg, 0, func, ##__VA_ARGS__)

#define fatalizew(code, msg, msgl, func, ...)											\
	do {																														\
		if(func(__VA_ARGS__))																					\
			failw(code, msg, msgl);																			\
	} while(0)

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally																\
XAPI_FINALIZE:																\
if(xapi_frame_finalized())										\
	goto XAPI_LEAVE;														\
xapi_frame_finalize();												\
	goto XAPI_FINALLY;													\
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda										\
	goto XAPI_LEAVE;							\
XAPI_LEAVE:											\
	return xapi_frame_leave()

/// conclude
//
// SUMMARY
//  capture the error code from the current function
//
#define conclude										\
	goto XAPI_LEAVE;									\
XAPI_LEAVE:													\
	_xapi_r = xapi_frame_leave()

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
#define XAPI_INFOS(k, vstr)														\
	XAPI_INFOW(k, vstr, 0)

#define XAPI_INFOW(k, vstr, vlen)											\
	do {																								\
		xapi_frame_infow(k, 0, vstr, vlen);								\
	} while(0)

#define XAPI_INFOF(k, vfmt, ...)											\
	do {																								\
		xapi_frame_infof(k, 0, vfmt, ##__VA_ARGS__);			\
	} while(0)

/// XAPI_UNWINDING
//
// SUMMARY
//  true if an error has been raised
//
#define XAPI_UNWINDING xapi_unwinding()

/*
** called after finally iff XAPI_UNWINDING
*/

/// xapi_frame_count
//
// SUMMARY
//  returns the number of frames in the callstack
//
int xapi_frame_count();

/// xapi_frame_error
//
// SUMMARY
//  write a string with the error for the specified frame
//  
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//  x    - frame index
//
size_t xapi_frame_error(char * const restrict dst, const size_t sz, int x)
	__attribute__((nonnull));

/// xapi_frame_function
//
// SUMMARY
//  write a string with the function for the specified frame
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//  x    - frame index
//
size_t xapi_frame_function(char * const restrict dst, const size_t sz, int x)
	__attribute__((nonnull));

/// xapi_frame_location
//
// SUMMARY
//  write a string with the file and line number for the specified frame
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//  x    - frame index
//
size_t xapi_frame_location(char * const restrict dst, const size_t sz, int x)
	__attribute__((nonnull));

/// xapi_frame_info
//
// SUMMARY
//  write a string with the info k/v pairs for the specified frame
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//  x    - frame index
//
size_t xapi_frame_infostring(char * const restrict dst, const size_t sz, int x)
	__attribute__((nonnull));

/// xapi_frame_trace
//
// SUMMARY
//  write a string with the error, function, info, and location for the specified frame
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//  x    - frame index
//
size_t xapi_frame_trace(char * const restrict dst, const size_t sz, int x)
	__attribute__((nonnull));

/// xapi_trace_pithy
//
// SUMMARY
//  write a string summarizing the entire callstack
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//
size_t xapi_trace_pithy(char * const restrict dst, const size_t sz)
	__attribute__((nonnull));

/// xapi_trace_full
//
// SUMMARY
//  write a string describing the entire callstack in full
//
// REMARKS
//  will contain newlines for a multi-frame stack, but does not terminate with a newline
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//
size_t xapi_trace_full(char * const restrict dst, const size_t sz)
	__attribute__((nonnull));

/// xapi_pithytrace
//
// SUMMARY
//  call xapi_trace_pithy and write the output to stderr
//
void xapi_pithytrace();

/// xapi_fulltrace
/// xapi_backtrace
//
// SUMMARY
//  call xapi_trace_full and write the output to stderr
//
void xapi_fulltrace();
void xapi_backtrace();

#undef restrict
#endif
