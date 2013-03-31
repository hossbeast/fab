/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

int API lstack_last_clear(lstack* const restrict ls)
{
	ls->last.sl = 0;
	ls->last.l = 0;

	return 1;
}

int API lstack_last_set(lstack* const restrict ls, int y)
{
	// reallocate if necessary
	if(ls->last.sa <= (y / 8))
	{
		fatal(xrealloc, &ls->last.s, sizeof(ls->last.s[0]), (y/8)+1, ls->last.sa);
		ls->last.sa = (y/8)+1;
	}

	// reset any already-allocated elements now visible
	int x;
	for(x = ls->last.sl; x <= (y/8); x++)
		ls->last.s[x] = 0;

	// increase length
	ls->last.sl = MAX(ls->last.sl, (y/8)+1);

	// set this particular bit
	if((ls->last.s[y/8] & (0x01 << (y%8))) == 0)
		ls->last.l++;

	ls->last.s[y/8] |= (0x01 << y%8);

	finally : coda;
}





