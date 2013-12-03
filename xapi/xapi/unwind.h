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

// declarations of frame-manipulation functions (application-visible but not directly called)
#include "xapi/frame.h"

#define restrict __restrict

// error table struct
typedef struct etable
{
	// indexed by error code
	struct
	{
		char * name;		// i.e. ENOMEM
		char * desc;		// i.e. Not enough space
	} * v;

	char * name;			// i.e. "PCRE", "SYS", "FAB", "LW"
} etable;

// an error table for system errors is provided by libxapi
extern etable * perrtab_SYS;

/*
** called at the site of an error
*/

// raise an error
#define fail(code, fmt, ...)																				\
	do {																															\
		/* populate top stack frame */																	\
		XAPI_FRAME_SET(perrtab, code);																	\
		if(xapi_frame_set_message(fmt, #__VA_ARGS__) != 0)							\
		{																																\
			/* xapi_frame_set_message populated alt[1] with ENOMEM */			\
			XAPI_FRAME_SET(0, -1);																				\
		}																																\
		if(!xapi_frame_finalized())																			\
		{																																\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

// raise an error on behalf of another function
#define fatality(func, table, code)																	\
	do {																															\
		if(xapi_frame_push() != 0)																			\
		{																																\
			/* xapi_frame_push populated alt[1] with ENOMEM */						\
			XAPI_FRAME_SET(0, -1);																				\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		/* populate stack frame for the called function */							\
		xapi_frame_set(table, code, 0, 0, func);												\
		xapi_frame_leave();																							\
																																		\
		/* populate stack frame for myself */														\
		XAPI_FRAME_SET(0, -1);																					\
		if(!xapi_frame_finalized())																			\
		{																																\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

// fatality macro for SYS
#define fatality_sys(func, code)																		\
	fatality(func, perrtab_SYS, code)

// if the called function fails, raise an error on its behalf
#define fatalize(table, code, func, ...)														\
	do {																															\
		if(xapi_frame_push() != 0)																			\
		{																																\
			/* xapi_frame_push populated alt[1] with ENOMEM */						\
			XAPI_FRAME_SET(0, -1);																				\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		int __r = func(__VA_ARGS__);																		\
		if(__r != 0)																										\
		{																																\
			/* populate stack frame for the called function */						\
			xapi_frame_set(table, code, 0, 0, #func);											\
			xapi_frame_leave();																						\
																																		\
			/* populate stack frame for myself */													\
			XAPI_FRAME_SET(0, -1);																				\
			if(!xapi_frame_finalized())																		\
			{																															\
				goto XAPI_FAILURE;																					\
			}																															\
		}																																\
	} while(0)

// fatalize macro for calling sys
#define fatalize_sys(func, ...)																			\
	fatalize(perrtab_SYS, errno, func, __VA_ARGS__)

/*
** called elsewhere in the stack
*/

#define fatal(func, ...)																						\
	do {																															\
		if(xapi_frame_push() != 0)																			\
		{																																\
			/* xapi_frame_push populated alt[1] with ENOMEM */						\
			XAPI_FRAME_SET(0, -1);																				\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		int __r = func(__VA_ARGS__);																		\
		if(__r != 0)																										\
		{																																\
			/* populate stack frame for myself */													\
			XAPI_FRAME_SET(0, -1);																				\
			if(!xapi_frame_finalized())																		\
			{																															\
				goto XAPI_FAILURE;																					\
			}																															\
		}																																\
	} while(0)

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
	xapi_frame_pop();														\
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda																	\
	goto XAPI_LEAVE;														\
XAPI_LEAVE:																		\
	return xapi_frame_exit() || xapi_frame_top_code_alt()

/*
** called after finally
*/ 

/// XAPI_INFO
//
// set info for the current frame
//
#define XAPI_INFO(imp, k, vfmt, ...)															\
	do {																														\
		if(xapi_frame_add_info(imp, k, 0, vfmt, ##__VA_ARGS__) != 0)	\
		{																															\
			/* xapi_frame_add_info populated alt[1] with ENOMEM */			\
			XAPI_FRAME_SET(0, -1);																			\
			goto XAPI_LEAVE;																						\
		}																															\
	} while(0)

/// XAPI_FAILING
//
// true if the current frame is about to return with failure
//
#define XAPI_FAILING xapi_frame_top_code()

/*
** called after finally iff XAPI_FAILING
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
//  call xapi_trace_pity and write the output to stderr
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
