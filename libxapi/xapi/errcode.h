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

#ifndef _XAPI_ERRCODE_H
#define _XAPI_ERRCODE_H

int __thread __xapi_r;

/*
** use at the site of an error
*/

// raise an error
#define fail(code, ...)												\
	do {																				\
		__xapi_r = code;													\
		goto XAPI_FINALLY;												\
	} while(0)

// if the called function fails, raise an error on its behalf
#define fatalize(code, func, ...)							\
	do {																				\
		if(func(__VA_ARGS__) != 0)								\
		{																					\
			__xapi_r = code;												\
			goto XAPI_FINALLY;											\
		}																					\
	} while(0)

/*
** called elsewhere in the stack
*/

#define fatal(func, ...)											\
	do {																				\
		if(func(__VA_ARGS__) != 0)								\
		{																					\
			goto XAPI_FINALLY;											\
		}																					\
	} while(0)


/*
** called after coda to provide info for the current frame
*/ 

#define XAPI_INFO(imp, k, vfmt, ...)

#define XAPI_FAILING 0

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally																\
	__xapi_r = 0;																\
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda return __xapi_r

#endif
