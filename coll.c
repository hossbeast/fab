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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coll.h"

int coll_singly_add(coll_singly* c, void* el, void* ret)
{
	if(c->len == c->alloc)
	{
		int ns = 8;
		if(c->alloc)
			ns = (c->alloc * 2) + (c->alloc / 2);

		void* tmp = c->e;
		if(!(c->e = calloc(ns, c->size)))
			return 0;
		if(tmp)
			memcpy(c->e, tmp, c->alloc * c->size);
		free(tmp);
		c->alloc = ns;
	}

	if(el)
		memcpy(c->e + (c->size * c->len), el, c->size);

	if(ret)
		*(void**)ret = c->e + (c->size * c->len);

	c->len++;

	return 1;
}

int coll_doubly_add(coll_doubly* c, void* el, void* ret)
{
	if(c->len == c->alloc)
	{
		int ns = 8;
		if(c->alloc)
			ns = (c->alloc * 2) + (c->alloc / 2);

		void* tmp = c->e;
		if(!(c->e = calloc(ns, sizeof(void*))))
			return 0;
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
		*t = calloc(1, c->size);
	}

	if(ret)
		*(void**)ret = *t;

	c->len++;

	return 1;
}

void coll_singly_free(coll_singly* c)
{
	free(c->e);

	c->len = 0;
	c->alloc = 0;
	c->e = 0;
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

