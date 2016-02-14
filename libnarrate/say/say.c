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
#include "xlinux.h"

#include "internal.h"

#include "dynamic/dynamic.internal.h"
#include "fixed/fixed.internal.h"
#include "file/file.internal.h"

#define restrict __restrict

//
// public
//

API xapi narrate_sayf(narrator * n, const char * const restrict fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);

	fatal(narrate_vsayf, n, fmt, va);

finally:
	va_end(va);
coda;
}

xapi narrate_sayc(narrator * const restrict n, int c)
{
	xproxy(narrate_sayw, n, (char*)&c, 1);
}

// route to appropriate implementation

xapi narrate_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

	if(n->ps)
		fatal(dynamic_vsayf, n, fmt, va);
	else if(n->bb)
		fatal(fixed_vsayf, n, fmt, va);
	else if(n->fd)
		fatal(file_vsayf, n, fmt, va);

	finally : coda;
}

API xapi narrate_sayw(narrator * const restrict n, char * const restrict b, size_t l)
{
  enter;

	if(n->ps)
		fatal(dynamic_sayw, n, b, l);
	else if(n->bb)
		fatal(fixed_sayw, n, b, l);
	else if(n->fd)
		fatal(file_sayw, n, b, l);

	finally : coda;
}
