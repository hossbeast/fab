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

int getstem(const char * const abs, const char * const rel, char ** const r)
{
	int dotdots = 0;
	const char * s = rel;
	while(1)
	{
		const char * ps = s;
		const char * pe = s;
		while(pe[0] != 0 && pe[0] != '/')
			pe++;

		if(pe - ps == 1 && ps[0] == '.')
		{
			s = pe + 1;
		}
		else if(pe - ps == 2 && ps[0] == '.' && ps[1] == '.')
		{
			s = pe + 1;
			dotdots++;
		}
		else
		{
			break;
		}
	}

	// s now points to the portion of rel following any leading dots and dotdots
	int e = strlen(abs);
	while(e && strcmp(abs + e, s))
		e--;

	while(e && abs[e-1] == '/')
		e--;

	if(((*r) = calloc(1, e + 1)) == 0)
		return 0;

	memcpy((*r), abs, e);

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

	// the stem : absolute - relative
	fatal(getstem, p->abs, p->rel, &p->stem);

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
	p->steml				= strlen(p->stem);

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
		return 0;

	// relative path - very close to the users representation
	//  - internal dots and dotdots are resolved
	//  - resolve internal symbolic links which do not cross mount points
	// 
	if(canon(buf, 0, (*p)->rel, 512, base, CAN_NEXT_DOT | CAN_NEXT_SYM) == 0)
		return 0;

	path_init(*p);

(*p)->in = strdup(buf);
(*p)->base = strdup(base);

/*
dprintf(2, ">>%5s: %s\n", "in", (*p)->in);
dprintf(2, "> %5s: %s\n", "base", (*p)->base);
dprintf(2, "> %5s: %s\n", "can", (*p)->can);
dprintf(2, "> %5s: %s\n", "abs", (*p)->abs);
dprintf(2, "> %5s: %s\n", "rel", (*p)->rel);
dprintf(2, "> %5s: %s\n", "stem", (*p)->stem);
*/

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

int path_copy(const path * const A, path ** const B)
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
