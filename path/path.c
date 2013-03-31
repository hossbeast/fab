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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "path.h"

#include "control.h"
#include "xmem.h"
#include "canon.h"
#include "cksum.h"

//
// static
//
int getdir(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	if(s == p)
	{
		*r = strdup(".");
		return 1;
	}

	if(((*r) = calloc(1, (s - p) + 1)))
	{
		memcpy(*r, p, s - p);
		return 1;
	}
	
	return 0;
}

int getname(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	if(s[0] == '/')
		s++;
	if((*r = strdup(s)))
		return 1;
	return 0;
}

int getext(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	while(s != (p + strlen(p)) && s[0] != '.')
		s++;

	if(s[0] == '.')
	{
		if((*r = strdup(s + 1)))
			return 1;
		else
			return 0;
	}

	return 1;
}

int getextlast(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/' && s[0] != '.')
		s--;

	if(s[0] == '.')
	{
		if((*r = strdup(s + 1)))
			return 1;
		else
			return 0;
	}

	return 1;
}

static int path_init(path * const p)
{
	// directories
	fatal(getdir, p->can, &p->can_dir);
	fatal(getdir, p->abs, &p->abs_dir);
	fatal(getdir, p->rel, &p->rel_dir);

	// other properties
	fatal(getname, p->can, &p->name);
	fatal(getext, p->can, &p->ext);
	fatal(getextlast, p->can, &p->ext_last);

	// lengths
	p->canl					= strlen(p->can);
	p->absl					= strlen(p->abs);
	p->rell					= strlen(p->rel);
	p->can_dirl			= strlen(p->can_dir);
	p->abs_dirl			= strlen(p->abs_dir);
	p->rel_dirl			= strlen(p->rel_dir);
	p->namel				= strlen(p->name);
	if(p->ext)
		p->extl				= strlen(p->ext);
	if(p->ext_last)
		p->ext_lastl	= strlen(p->ext_last);

	if(p->base)
		p->basel			= strlen(p->base);
	p->inl					= strlen(p->in);

	// hash of the canonical path
	p->can_hash			= cksum(p->can, p->canl);

	// nofile determination
	p->is_nofile		= p->canl >= 4 && memcmp(p->can, "/../", 4) == 0;

	finally : coda;
}

//
// public
//

int path_create(path ** const p, const char * const base, const char * const fmt, ...)
{
	if(xmalloc(p, sizeof(**p)) == 0)
		return 0;

	if(xmalloc(&(*p)->can, 512) == 0)
		return 0;

	if(xmalloc(&(*p)->abs, 512) == 0)
		return 0;

	if(xmalloc(&(*p)->rel, 512) == 0)
		return 0;

	va_list va;
	va_start(va, fmt);
	char buf[512];
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	// save pameters of creation
	(*p)->in = strdup(buf);
	(*p)->base = strdup(base);

	// canonical path - fully canonicalized
	//
	if(canon(buf, 0, (*p)->can, 512, base, CAN_REALPATH) == 0)
		return 0;

	// absolute path - close to the users representation - but forced to absolute notation
	//  - will always begin with a slash (FORCE_DOT)
	//	- all dots and dotdots resolved
	//  - resolve internal symbolic links which do not cross mount points
	//
	if(canon(buf, 0, (*p)->abs, 512, base, CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_NEXT_SYM) == 0)
		return printf("2\n"), 0;

	// relative path - very close to the users representation
	//  - internal dots and dotdots are resolved
	//  - resolve internal symbolic links which do not cross mount points
	// 
	if(canon(buf, 0, (*p)->rel, 512, base, CAN_NEXT_DOT | CAN_NEXT_SYM) == 0)
		return printf("3\n"), 0;

	path_init(*p);

	return 1;
}

int path_create_canon(path ** const p, const char * fmt, ...)
{
	if(xmalloc(p, sizeof(**p)) == 0)
		return 0;

	if(xmalloc(&(*p)->can, 512) == 0)
		return 0;

	va_list va;
	va_start(va, fmt);
	vsnprintf((*p)->can, 512, fmt, va);
	va_end(va);

	(*p)->abs = strdup((*p)->can);
	(*p)->rel = strdup((*p)->can);
	(*p)->in  = strdup((*p)->can);

	path_init(*p);

	return 1;
}

int path_cmp(const path * const A, const path * const B)
{
	return strcmp(A->can, B->can);
}

void path_free(path * const p)
{
	if(p)
	{
		// free all the strings
		int x;
		for(x = 0; x < sizeof(p->strings) / sizeof(p->strings[0]); x++)
			free(p->strings[x]);
	}

	free(p);
}

void path_xfree(path ** const p)
{
	path_free(*p);
	*p = 0;
}

int path_copy(path ** const B, const path * const A)
{
	if(xmalloc(B, sizeof(**B)) == 0)
		return 0;

	// copy
	(**B) = *A;

	// duplicate all the strings
	int x;
	for(x = 0; x < sizeof((*B)->strings) / sizeof((*B)->strings[0]); x++)
	{
		if((*B)->strings[x])
		{
			if(((*B)->strings[x] = strdup((*B)->strings[x])) == 0)
				return 0;
		}
	}
	
	return 1;
}

