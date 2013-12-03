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

#include <stdlib.h>

#include "xapi/internal.h"

void callstack_free()
{
	if(callstack.v)
	{
		int x;
		for(x = 0; x < callstack.frames.stor.a; x++)
		{
			struct frame * A = &callstack.frames.stor.v[x];

			free(A->msg);

			int y;
			for(y = 0; y < A->info.a; y++)
			{
				free(A->info.v[y].ks);
				free(A->info.v[y].vs);
			}
			free(A->info.v);
		}

		free(callstack.frames.stor.v);
	}

	free(callstack.v);
	callstack.v = 0;
}
