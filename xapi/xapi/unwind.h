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
//  raise an error with the specified code and the prevailing errtab
//
#define fail(code, fmt, ...)																				\
	do {																															\
		/* populate top stack frame */																	\
		XAPI_FRAME_SET(perrtab, code);																	\
																																		\
		/* set message for top stack frame */														\
		if(xapi_frame_set_message(fmt, #__VA_ARGS__) != 0)							\
		{																																\
			/* xapi_frame_set_message populated alt[0] with ENOMEM */			\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);																\
		}																																\
																																		\
		/* jump to the end unless the frame is already finalized */			\
		if(!xapi_frame_finalized())																			\
		{																																\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

/// fatality
//
// SUMMARY
//  raise an error on behalf of another function that just failed
//
#define fatality(func, table, code, fmt, ...)												\
	do {																															\
		if(xapi_frame_push() != 0)																			\
		{																																\
			/* xapi_frame_push populated alt[0] with ENOMEM */						\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);																\
																																		\
			/* while isalt, frame_push cannot return nonzero */						\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		/* populate stack frame for the called function */							\
		xapi_frame_set(table, code, 0, 0, func);												\
																																		\
		if(xapi_frame_set_message(fmt, ##__VA_ARGS__) != 0)							\
		{																																\
			/* xapi_frame_set_message populated alt[0] with ENOMEM */			\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);																\
		}																																\
																																		\
		xapi_frame_leave();																							\
																																		\
		/* populate stack frame for myself */														\
		XAPI_FRAME_SET(0, 0);																						\
		if(!xapi_frame_finalized())																			\
		{																																\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

// fatality macro for SYS - assumes there is no message
#define sysfatality(func)																		\
	fatality(#func, perrtab_SYS, errno, 0)

/*
** called elsewhere in the stack
*/

#define XINVOKE(func, ...)																	\
	({																												\
		int __d = xapi_frame_depth();														\
		if(xapi_frame_push() != 0)															\
		{																												\
			/* xapi_frame_push populated alt[0] with ENOMEM */		\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);												\
			goto XAPI_FAILURE;																		\
		}																												\
																														\
		int __r = func(__VA_ARGS__);														\
																														\
		if(xapi_frame_depth() != __d)														\
		{																												\
			xapi_frame_leave();																		\
		}																												\
		__r;																										\
	})

#define XALTINVOKE(func, ...)																\
	({																												\
		int __d = xapi_frame_depth();														\
		xapi_frame_alt_push();																	\
		int __r = func(__VA_ARGS__);														\
		if(xapi_frame_depth() != __d)														\
			xapi_frame_leave();																		\
		}																												\
		__r;																										\
	})

/// fatalize
//
// SUMMARY
//  call a function. if it fails, and did not UNWIND, raise an error on its behalf
//
#define fatalize(table, code, func, ...)														\
	do {																															\
		int __d = xapi_frame_depth();																		\
		if(xapi_frame_push() != 0)																			\
		{																																\
			/* xapi_frame_push populated alt[0] with ENOMEM */						\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);																\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		int __r = func(__VA_ARGS__);																		\
		if(xapi_frame_depth() != __d)																		\
		{																																\
			if(__r != 0)																									\
			{																															\
				/* populate stack frame on called functions behalf */				\
				xapi_frame_set(table, code ?: __r, 0, 0, #func);						\
			}																															\
			xapi_frame_leave();																						\
		}																																\
		if(__r != 0)																										\
		{																																\
			/* populate stack frame for myself */													\
			XAPI_FRAME_SET(0, 0);																					\
			if(!xapi_frame_finalized())																		\
			{																															\
				goto XAPI_FAILURE;																					\
			}																															\
		}																																\
	} while(0)

// fatalize macro for calling sys
#define sysfatalize(func, ...)																			\
	fatalize(perrtab_SYS, errno, func, __VA_ARGS__)

/// fatal
//
// SUMMARY
//  call a function that is known to be UNWIND-ing
//
#define fatal(func, ...) fatalize(0, 0, func, __VA_ARGS__)

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally																\
	goto XAPI_SUCCESS;													\
XAPI_FAILURE:																	\
	xapi_frame_finalize();											\
	goto XAPI_FINALLY;													\
XAPI_SUCCESS:																	\
	xapi_frame_finalize();											\
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda										\
	goto XAPI_LEAVE;							\
XAPI_LEAVE:											\
	return xapi_frame_exit()


/// coda_custom
//
// SUMMARY
//
#define coda_custom									\
	goto XAPI_LEAVE;									\
XAPI_LEAVE:													\
	_xapi_r = xapi_frame_exit()

/*
** called after finally
*/ 

/// XAPI_INFO
//
// set info for the current frame if XAPI_UNWINDING
//
#define XAPI_INFO(k, vfmt, ...)																		\
	do {																														\
		if(xapi_frame_add_info(k, 0, vfmt, ##__VA_ARGS__) != 0)				\
		{																															\
			/* xapi_frame_add_info populated alt[0] with ENOMEM */			\
			XAPI_FRAME_SET_AND_LEAVE(0, 0);															\
			goto XAPI_LEAVE;																						\
		}																															\
	} while(0)

/// XAPI_UNWINDING
//
// true if the current frame is about to return with failure
//
#define XAPI_UNWINDING xapi_frame_unwinding()

/*
** called after finally iff XAPI_UNWINDING
*/

/// xapi_frame_count
//
// SUMMARY
//  returns the number of frames in the callstack
//
int xapi_frame_count();

/// xapi_frame_count_alt
//
// SUMMARY
//  returns the number of frames in the alt callstack
//
int xapi_frame_count_alt();

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

size_t xapi_frame_error_alt(char * const restrict dst, const size_t sz, int x)
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

size_t xapi_frame_function_alt(char * const restrict dst, const size_t sz, int x)
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

size_t xapi_frame_location_alt(char * const restrict dst, const size_t sz, int x)
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
// REMARKS
//  alt stack frames do not have info k/v pairs
//
size_t xapi_frame_info(char * const restrict dst, const size_t sz, int x)
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

size_t xapi_frame_trace_alt(char * const restrict dst, const size_t sz, int x)
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
