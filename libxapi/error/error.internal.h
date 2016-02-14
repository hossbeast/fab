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

#ifndef _XAPI_ERROR_INTERNAL_H
#define _XAPI_ERROR_INTERNAL_H

#include "xapi.h"

#include "error.h"

struct memblk;

typedef struct error
{
	const struct etable *	etab;		// error table
	xapi_code             code;		// error code
	char *	              msg;	  // error message
  size_t                msgl;
} error;

/// error_freeze
//
//
//
void error_freeze(struct memblk * const restrict mb, error * restrict e)
  __attribute__((nonnull));

/// error_unfreeze
//
//
//
void error_unfreeze(struct memblk * const restrict mb, error * restrict e)
  __attribute__((nonnull));

/// error_thaw
//
//
//
void error_thaw(char * const restrict mb, error * restrict e)
  __attribute__((nonnull));

#undef restrict
#endif
