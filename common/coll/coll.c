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

#include "xlinux.h"
#include "xapi.h"

#include "coll.h"

int coll_doubly_add(coll_doubly* c, void* el, void* ret)
{
	if(c->len == c->alloc)
	{
		int ns = c->alloc ?: 10;
		ns = ns * 2 + ns / 2;

		void* tmp = c->e;
		fatal(xmalloc, &c->e, ns * sizeof(void*));
		if(tmp)
			memcpy(c->e, tmp, c->alloc * sizeof(void*));
		free(tmp);
		c->alloc = ns;
	}

	char** t = ((char**)(c->e + (sizeof(void*) * c->len)));

	if(el)
		memcpy(t, el, sizeof(void*));
	else
	{
		fatal(xmalloc, t, c->size);
	}

	if(ret)
		*(void**)ret = *t;

	c->len++;

	finally : coda;
}

void coll_doubly_free(coll_doubly* c)
{
	int x;
	for(x = 0; x < c->len; x++)
		free(((char**)c->e)[x]);

	free(c->e);

	c->len = 0;
	c->alloc = 0;
	c->e = 0;
}
