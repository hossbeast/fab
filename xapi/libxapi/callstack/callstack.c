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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xapi/internal.h"

void xfree(void * v)
{
	free(*(void**)v);
	*(void**)v = 0;
}

void callstack_free()
{
//printf("CALLSTACK_FREE\n");

	if(callstack.v)
	{
		int x;
		for(x = 0; x < callstack.frames.stor.a; x++)
		{
			struct frame * A = &callstack.frames.stor.v[x];

			xfree(&A->msg);

			int y;
			for(y = 0; y < A->info.a; y++)
			{
				xfree(&A->info.v[y].ks);
				A->info.v[y].ka = 0;
				A->info.v[y].kl = 0;

				xfree(&A->info.v[y].vs);
				A->info.v[y].va = 0;
				A->info.v[y].vl = 0;
			}
			xfree(&A->info.v);
			A->info.a = 0;
			A->info.l = 0;
		}

		xfree(&callstack.frames.stor.v);
		callstack.frames.stor.a = 0;
	}

	xfree(&callstack.v);
	callstack.a = 0;
	callstack.l = 0;
	callstack.depth = 0;

	memset(&callstack.alt, 0, sizeof(callstack.alt));
}
