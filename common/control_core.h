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

#ifndef _CONTROL_CORE_H
#define _CONTROL_CORE_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
** UNWIND_ERRORS		- whether to return a different value on failure depending on whether this is the immediate site of the error
** CODA_BAD_ACTION  - executed in coda during error resolution
** CODA_GOOD_ACTION - executed in coda during normal execution
** LOG_ERROR     		- handle error message
** LOG_INFO      		- handle info message
*/

/// finally
//
// statements between finally and coda are executed even upon fail/leave
//
#define finally						\
	int _coda_r;						\
	goto CODA_GOOD;					\
CODA_BAD:									\
	CODA_BAD_ACTION;				\
	goto CODA_FINALLY;			\
CODA_GOOD:								\
	CODA_GOOD_ACTION;				\
CODA_FINALLY							\

#define coda return _coda_r

/// FAILURE_CODE
//
// SUMMARY
//  value to return from a function to indicate failure
//
#define FAILURE_CODE									\
	({																	\
		int __x = -1;											\
		if(UNWIND_ERRORS)									\
			__x = 1;												\
																			\
		__x;															\
	})

/// error
//
// SUMMARY
//  log an error message
//
#define error(fmt, ...)								\
	LOG_ERROR(fmt, ##__VA_ARGS__)

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
