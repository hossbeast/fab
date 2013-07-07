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

#ifndef _CONTROL_H
#define _CONTROL_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

/// finally
//
//
//
#define finally				\
	int _coda_r;				\
	goto CODA_GOOD;			\
CODA:									\
	_coda_r = 0;				\
	goto CODA_FINALLY;	\
CODA_GOOD:						\
	_coda_r = 1;				\
CODA_FINALLY

/// coda
//
//
//
#define coda return _coda_r

/// error
//
// SUMMARY
//  log an L_ERROR message
//
#define error(fmt, ...)									\
	do {																	\
		log(L_ERROR, fmt " at [%s:%d (%s)]"	\
			, ##__VA_ARGS__										\
			, __FILE__												\
			, __LINE__												\
			, __FUNCTION__										\
		);																	\
	} while(0)

/// fail
//
// SUMMARY
//  log an L_ERROR message, terminate the current scope with failure
//
#define fail(fmt, ...)					\
	do {													\
		error(fmt, ##__VA_ARGS__);	\
		goto CODA;									\
	} while(0)

/// qfail
//
// SUMMARY
//  silently terminate the current scope with failure
//
#define qfail()	goto CODA

/// qterm
//
// SUMMARY
//  silently terminate the current scope with success
//
#define qterm() goto CODA_GOOD

/// fatal
//
// SUMMARY
//  execute the specified function with zero-return-failure semantics and if it fails
//  log an L_ERROR message and terminate the current scope with failure
//
#define fatal(x, ...)														\
	do {																					\
		if((x(__VA_ARGS__)) == 0)										\
		{																						\
			log(L_ERROR, #x " failed at [%s:%d (%s)]"	\
				, __FILE__															\
				, __LINE__															\
				, __FUNCTION__													\
			);																				\
			goto CODA;																\
		}																						\
	} while(0)
 
/// qfatal
//
// SUMMARY
//  execute the specified function with zero-return-failure semantics and if it fails
//  silently terminate the current scope with failure
//
#define qfatal(x, ...)													\
	do {																					\
		if((x(__VA_ARGS__)) == 0)										\
		{																						\
			goto CODA;																\
		}																						\
	} while(0)

/// fatal_os
//
// SUMMARY
//  execute the specified function with nonzero-return-failure semantics and if it fails
//  log an L_ERROR message and terminate the current scope with failure
//
#define fatal_os(x, ...)																				\
	do {																													\
		if((x(__VA_ARGS__)) != 0)																		\
		{																														\
			log(L_ERROR, #x " failed with: [%d][%s] at [%s:%d (%s)]"	\
				, errno																									\
				, strerror(errno)																				\
				, __FILE__																							\
				, __LINE__																							\
				, __FUNCTION__																					\
			);																												\
			goto CODA;																								\
		}																														\
	} while(0)

/// fatal_os
//
// SUMMARY
//  execute the specified function with nonzero-return-failure semantics and if it fails
//  silently terminate the current scope with failure
//
#define qfatal_os(x, ...)																				\
	do {																													\
		if((x(__VA_ARGS__)) != 0)																		\
		{																														\
			goto CODA;																								\
		}																														\
	} while(0)

#endif

