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

#include "xapi/callstack.h"

typedef struct
{
	// indexed by error code
	struct
	{
		char * name;		// i.e. ENOMEM
		char * desc;		// i.e. Not enough space
	} * v;

	char * name;				// i.e. "PCRE", "SYS", "FAB", "LW"
} etable;

/*
** use at the site of an error
*/

// raise an error
#define fail(code, fmt, ...)																				\
	do {																															\
		/* populate top stack frame */																	\
		CALLSTACK_FRAME(etable, code);																	\
		callstack_frame_message(fmt, #__VA_ARGS__);											\
		goto XAPI_FAILURE;																							\
	} while(0)

// if the called function fails, raise an error on its behalf
#define fatalize(table, code, func, ...)														\
	do {																															\
		if(callstack_push(1) != 0)																			\
		{																																\
			/* callstack_push populated my frame with ENOMEM */						\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		int __r = func(__VA_ARGS__);																		\
		if(__r != 0)																										\
		{																																\
			/* populate stack frame for the called function */						\
			callstack_frame(etab, code, 0, 0, func);											\
			callstack.top--;																							\
																																		\
			/* populate stack frame for myself */													\
			CALLSTACK_FRAME(0, -1);																				\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

/*
** called elsewhere in the stack
*/

#define fatal(func, ...)																						\
	do {																															\
		if(callstack_push(1) != 0)																			\
		{																																\
			/* callstack_push populated my frame with ENOMEM */						\
			goto XAPI_FAILURE;																						\
		}																																\
																																		\
		int __r = func(__VA_ARGS__);																		\
		if(__r != 0)																										\
		{																																\
			/* create stack frame for myself */														\
			CALLSTACK_FRAME(0, -1);																				\
			goto XAPI_FAILURE;																						\
		}																																\
	} while(0)

/*
** called after coda to provide info for the current frame
*/ 

#define XAPI_INFO(imp, k, vfmt, ...)																\
	do {																															\
		if(callstack.frames[callstack.top].code != 0)										\
		{																																\
			if(callstack_frame_info(imp, k, 0, vfmt, __VA_ARGS__) != 0)		\
				goto XAPI_LEAVE;																						\
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
	goto XAPI_FINALLY;													\
XAPI_SUCCESS:																	\
	callstack.frames[callstack.top].code = 0;		\
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda XAPI_LEAVE : return callstack.frames[callstack.top--].code

#endif
