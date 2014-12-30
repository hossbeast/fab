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
#include <stdarg.h>

#include "xapi.h"
#include "xlinux.h"

#include "path.h"
#include "args.h"
#include "params.h"

#include "cksum.h"
#include "canon.h"
#include "macros.h"
#include "memblk.h"
#include "memblk.def.h"

#define restrict __restrict

//
// static
//
static int getdir(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	if(s == p)
	{
		fatal(ixstrdup, r, ".");
	}
	else
	{
		fatal(xmalloc, r, (s - p) + 1);
		memcpy(*r, p, s - p);
	}

	finally : coda;
}

static int getname(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	if(s[0] == '/')
		s++;

	fatal(ixstrdup, r, s);

	finally : coda;
}

static int getext(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/')
		s--;

	while(s != (p + strlen(p)) && s[0] != '.')
		s++;

	if(s[0] == '.')
	{
		fatal(ixstrdup, r, s + 1);
	}

	finally : coda;
}

static int getextlast(const char * const p, char ** const r)
{
	const char * s = p + strlen(p);
	while(s != p && s[0] != '/' && s[0] != '.')
		s--;

	if(s[0] == '.')
	{
		fatal(ixstrdup, r, s + 1);
	}

	finally : coda;
}

static int path_init(path * const p)
{
	// directories
	fatal(getdir, p->can, &p->can_dir);
	fatal(getdir, p->abs, &p->abs_dir);
	fatal(getdir, p->rel_cwd, &p->rel_cwd_dir);
	fatal(getdir, p->rel_fab, &p->rel_fab_dir);
	fatal(getdir, p->rel_nofile, &p->rel_nofile_dir);

	// other properties
	fatal(getname, p->can, &p->name);
	fatal(getext, p->can, &p->ext);
	fatal(getextlast, p->can, &p->ext_last);

	// lengths
	p->canl							= strlen(p->can);
	p->absl							= strlen(p->abs);
	p->rel_cwdl					= strlen(p->rel_cwd);
	p->rel_fabl					= strlen(p->rel_fab);
	p->rel_nofilel			= strlen(p->rel_nofile);
	p->can_dirl					= strlen(p->can_dir);
	p->abs_dirl					= strlen(p->abs_dir);
	p->rel_cwd_dirl			= strlen(p->rel_cwd_dir);
	p->rel_fab_dirl			= strlen(p->rel_fab_dir);
	p->rel_nofile_dirl	= strlen(p->rel_nofile_dir);
	p->namel						= strlen(p->name);
	if(p->ext)
		p->extl						= strlen(p->ext);
	if(p->ext_last)
		p->ext_lastl			= strlen(p->ext_last);

	p->in_pathl					= strlen(p->in_path);
	p->in_basel					= strlen(p->in_base);

	// hash of the canonical path
	p->can_hash			= cksum(p->can, p->canl);

	// nofile determination
	p->is_nofile		= p->canl >= 4 && memcmp(p->can, "/../", 4) == 0;

	finally : coda;
}

static int create(path ** const p, const char * const in_base, const char * const fmt, va_list va, int init)
{
	fatal(xmalloc, p, sizeof(**p));
	fatal(xmalloc, &(*p)->can, 512);
	fatal(xmalloc, &(*p)->abs, 512);
	fatal(xmalloc, &(*p)->rel_cwd, 512);
	fatal(xmalloc, &(*p)->rel_fab, 512);
	fatal(xmalloc, &(*p)->rel_nofile, 512);

	char buf[512];
	vsnprintf(buf, sizeof(buf), fmt, va);

	// save pameters of creation
	fatal(ixstrdup, &(*p)->in_path, buf);
	fatal(ixstrdup, &(*p)->in_base, in_base);

	//
	// canonical path - fully canonicalized
	//
	fatal(canon, buf, 0, in_base, 0, (*p)->can, 512, 0, CAN_REALPATH);

	// 
	// absolute path - close to the users representation - but forced to absolute notation
	//  - will always begin with a slash (FORCE_DOT)
	//	- all dots and dotdots resolved
	//  - resolve internal symbolic links which do not cross mount points
	//
	fatal(canon, buf, 0, in_base, 0, (*p)->abs, 512, 0, CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_NEXT_SYM);

	// 
	// absolute path rebased to cwd
	//
	fatal(rebase, (*p)->abs, 0, g_params.cwd, 0, (*p)->rel_cwd, 512, 0);

	// 
	// absolute path rebased to init-fabfile-dir
	//
	if(init)
	{
		fatal(rebase, (*p)->abs, 0, g_params.cwd, 0, (*p)->rel_fab, 512, 0);
	}
	else
	{
		fatal(rebase, (*p)->abs, 0, g_args->init_fabfile_path->abs_dir, 0, (*p)->rel_fab, 512, 0);
	}

	// 
	// absolute path rebased to /..
	//
	fatal(rebase, (*p)->abs, 0, "/..", 0, (*p)->rel_nofile, 512, 0);

	// initialize
	fatal(path_init, *p);

	finally : coda;
}

//
// public
//

int path_create(path ** const p, const char * const in_base, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fatal(create, p, in_base, fmt, va, 0);

finally:
	va_end(va);
coda;
}

int path_create_init(path ** const p, const char * const in_base, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fatal(create, p, in_base, fmt, va, 1);

finally:
	va_end(va);
coda;
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
	fatal(xmalloc, B, sizeof(**B));

	// copy
	(**B) = *A;

	// duplicate all the strings
	int x;
	for(x = 0; x < sizeof((*B)->strings) / sizeof((*B)->strings[0]); x++)
	{
		if((*B)->strings[x])
		{
			char * z = 0;
			fatal(ixstrdup, &z, (*B)->strings[x]);
			(*B)->strings[x] = z;
		}
	}
	
	finally : coda;
}

void path_freeze(memblk * const restrict mb, path * restrict pth)
{
	int x;
	for(x = 0; x < sizeof(pth->strings) / sizeof(pth->strings[0]); x++)
		memblk_freeze(mb, &pth->strings[x]);
}

void path_thaw(char * const restrict mb, path * restrict pth)
{
	int x;
	for(x = 0; x < sizeof(pth->strings) / sizeof(pth->strings[0]); x++)
		memblk_thaw(mb, &pth->strings[x]);
}
