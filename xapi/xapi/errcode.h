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

#ifndef _XAPI_CALLSTACK_H
#define _XAPI_CALLSTACK_H

#include "xapi/

typedef struct etable
{
	// indexed by error code
	struct
	{
		char * name;		// i.e. ENOMEM
		char * desc;		// i.e. Not enough space
	} * v;

	char * name;				// i.e. "PCRE", "SYS", "FAB", "LW"
};

typedef struct callstack
{
	struct
	{
		struct frame
		{
			etable *	table;		// error table
			int				code;			// error code

			char * 		func;
			char * 		file;
			char *		line;

			struct
			{
				char *	s;
				int			a;
				int			l;
			} * s;

			int a;
			int l;
		} * v;

		int a;
		int l;
	} frames;

	struct frame * root;
	struct frame * top;
};

extern __thread callstack callstack;

// callstack manipulation
int callstack_push()

int callstack_pop()

/*
** use at the site of an error
*/

// raise an error
#define fail(code, fmt, ...)
	do {
		/* create root stack frame for myself */
		callstack.top->table	= etable;
		callstack.top->code 	= code;

		callstack.top->file 	= __FILE__;
		callstack.top->line 	= __LINE__;
		callstack.top->func 	= __FUNCTION__;

		callstack.root = callstack.top;
		goto CODA_BAD;
	} while(0)

// if the called function fails, raise an error on its behalf
#define fatalize(table, code, func, args...)
	do {
		int __r = func(__VA_ARGS__);
		if(__r != 0)
		{
			/* create stack frame for the called function */
			callstack.top++;
			callstack.top->table	= table;
			callstack.top->code 	= code;

			callstack.top->file		= 0;
			callstack.top->line		= 0;
			callstack.top->func 	= func;

			callstack.root = callstack.top;
			callstack.top--;

			/* populate stack frame for myself */
			callstack.top->line = __LINE__;
			callstack.top->file = __FILE__;
			callstack.top->func = __FUNCTION__;

			goto CODA_BAD;
		}
	} while(0)

/*
** called elsewhere in the stack
*/

	foo++;

finally :
	free(bar);

	: EINFO("%s", path);
	: EINFO("%s", STROPT(options);
coda;
}

fatal(parsefile, path, options, 

#define fatal(func, args...)
	do {
		callstack.top++; = callstack.frames.v[callstack.frames.l++];
		int __r = func(__VA_ARGS__);
		if(__r != 0)
		{
			/* create stack frame for myself */
			callstack.top->line = __LINE__;
			callstack.top->file = __FILE__;
			callstack.top->func = __FUNCTION__;


			callstack.top--;

			coto CODA_BAD;
		}
	} while(0)

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally									\
	goto XAPI_SUCCESS;						\
XAPI_FAILURE:										\
	goto XAPI_FINALLY;						\
XAPI_SUCCESS:										\
	callstack.top->code = 0;			\
XAPI_FINALLY
	callstack.top--;
XAPI_FINALLY 

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda return callstack_pop()
	do {
		int __r = callstack.top->code;
		callstack.top--;
		return __r;
	} while(0) 

/// fail
//
// SUMMARY
//  log an error message and terminate the current scope with failure
//
#define fail(fmt, ...)								\
	do {																\
		LOG_ERROR(fmt, ##__VA_ARGS__);		\
		goto CODA_BAD;										\
	} while(0)

#define qfail()	goto CODA_BAD

/// leave
//
// SUMMARY
//  log an info message and terminate the current scope with success
//
#define leave(fmt, ...)								\
	do {																\
		LOG_INFO(fmt, ##__VA_ARGS__);			\
		goto CODA_GOOD;										\
	} while(0)

#define qleave() goto CODA_GOOD

/// fatal
//
// SUMMARY
//  execute the specified function with zero-return-failure semantics and if it fails
//  log an L_ERROR message and terminate the current scope with failure
//
#define fatal(x, ...)											\
	do {																		\
		int __r = x(__VA_ARGS__);							\
		if(__r != 0)													\
		{																			\
			if(__r > 0)													\
			{																		\
				LOG_ERROR(#x " failed");					\
			}																		\
			goto CODA_BAD;											\
		}																			\
	} while(0)

#define qfatal(x, ...)										\
	do {																		\
		int __r = x(__VA_ARGS__);							\
		if(__r != 0)													\
		{																			\
			goto CODA_BAD;											\
		}																			\
	} while(0)

/// fatal_os
//
// SUMMARY
//  execute the specified function with nonzero-return-failure semantics and if it fails
//  log an error message and terminate the current scope with failure
//
#define fatal_os(x, ...)													\
	do {																						\
		if((x(__VA_ARGS__)) != 0)											\
		{																							\
			LOG_ERROR(#x " failed with: [%d][%s]"				\
				, errno, strerror(errno)									\
			);																					\
			goto CODA_BAD;															\
		}																							\
	} while(0)

#endif
