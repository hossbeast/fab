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

#include "listwise/internal.h"

/// lwx_getflags
//
// get application-use flags associated with lw
//
uint64_t API lwx_getflags(lwx * const lx)
{
	return lx->flags;
}

/// lwx_setflags
//
// set application-use flags associated with lw
//
uint64_t API lwx_setflags(lwx * const lx, const uint64_t g)
{
	return ((lx->flags = g));
}

/// lwx_getptr
//
// get application-use ptr associated with lw
//
void * API lwx_getptr(lwx * const lx)
{
	return lx->ptr;
}

/// lwx_setptr
//
// set application-use ptr associated with lw
//
void * API lwx_setptr(lwx * const lx, void * const g)
{
	return ((lx->ptr = g));
}
