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

#include <stdio.h>

#include "xapi.h"

#include "internal.h"

#include "macros.h"

#define restrict __restrict

//
// public
//

int fixed_vsayf(char * const restrict dst, size_t sz, const char * const restrict fmt, va_list va)
{
	znvloadf(dst, sz, fmt, va);

	finally : coda;
}

int fixed_sayw(char * const restrict dst, size_t sz, char * const restrict b, size_t l)
{
	znloadw(dst, sz, b, l);

	finally : coda;
}
