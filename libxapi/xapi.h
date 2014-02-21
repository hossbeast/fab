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

#ifndef _XAPI_H
#define _XAPI_H

#include <stdint.h>

/*
** xapi is an interface for propagating detailed and specific error information. It is a
** calling convention and these header(s) provides macros to facilitate its application
**
** xapi-enabled code is compiled in one of two modes specifying what is available when an error occurs:
**  1. UNWIND   - a complete backtrace
**  2. ERRCODE  - a nonzero error code
**
** for UNWIND-ing mode, the complete backtrace is accessible via the xapi_frame_* functions. there are
** also functions for producing a terse and complete error string from the backtrace information
**
** non UNWIND-ing code (even non-xapi-code) that calls UNWIND-ing code simply receives an error code
**
*/

#if XAPI_UNWIND
// error table struct
typedef struct etable
{
	// indexed by lower int16 of the error code + (min * -1)
	struct
	{
		char * name;		// i.e. ENOMEM
		char * desc;		// i.e. Not enough space
		char * str;			// i.e. ENOMEM : Not enough space
	} * v;

	char *  tag;			// i.e. "PCRE", "SYS", "FAB", "LW"
	int16_t id;				// upper 2 bytes of the error code
	int16_t	min;			// min err
	int16_t max;			// max err
} etable;

// an error table for system errors is provided by libxapi
extern etable * perrtab_SYS;

/// xapi_errstr
//
// SUMMARY
//  returns a static string associated with an error code returned from a libxapi
//
const char * xapi_errstr(const int code);
#endif

#if XAPI_UNWIND
# include "xapi/unwind.h"
#elif XAPI_ERRCODE
# include "xapi/errcode.h"
#else
# error "either XAPI_UNWIND or XAPI_ERRCODE must be defined"
#endif

#endif
