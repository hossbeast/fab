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
	// other stuff
	fatal(getdir, p->can, &p->can_dir);
	fatal(getdir, p->abs, &p->abs_dir);
	fatal(getdir, p->rel, &p->rel_dir);
	fatal(getname, p->can, &p->name);
	fatal(getext, p->can, &p->ext);
	fatal(getextlast, p->can, &p->ext_last);

	p->peer_dir = strdup(p->abs_dir);
	char * s = p->peer_dir + p->abs_dirl;
	while
	p->peer_dirl = strlen(p->peer_dir);

	// compute lengths
	p->canl				= strlen(p->can);
	p->absl				= strlen(p->abs);
	p->rell				= strlen(p->rel);
	p->can_dirl		= strlen(p->can_dir);
	p->abs_dirl		= strlen(p->abs_dir);
	p->rel_dirl		= strlen(p->rel_dir);
	p->namel			= strlen(p->name);
	if(p->ext)
		p->extl				= strlen(p->ext);
	if(p->ext_last)
		p->ext_lastl	= strlen(p->ext_last);

	// hash of the canonical path
	p->can_hash		= cksum(p->can, p->canl);

	// nofile determination
	p->is_nofile	= p->can_dirl >= 4 && memcmp(p->can_dir, "/../", 4) == 0;

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
	//  - resolve all symbolic links which do not cross mount points
	//
	if(canon(buf, 0, (*p)->abs, 512, base, CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_SYMREL | CAN_SYMABS) == 0)
		return 0;

	// relative path - very close to the users representation
	//  - internal dots and dotdots are resolved
	//	- symbolic links which do not cross mountpoints are resolved
	// 
	if(canon(buf, 0, (*p)->rel, 512, base, CAN_NEXT_DOT | CAN_SYMREL | CAN_SYMABS) == 0)
		return 0;

	path_init(*p);

/*
if(strcmp((*p)->name, "re.o") == 0 || strcmp((*p)->name, "fabfile") == 0)
{
	dprintf(2, "path_create : s=%s, base=%s\n", buf, base);

#include <execinfo.h>

	void * foo[16] = {};
	int nptrs = backtrace(foo, sizeof(foo) / sizeof(foo[0]));
	char ** strings = backtrace_symbols(foo, nptrs);
	int x;
	for(x = 0; x < nptrs; x++)
		dprintf(2, "%s\n", strings[x]);

	dprintf(2, " >can      =%s\n", (*p)->can);
	dprintf(2, " >candir   =%s\n", (*p)->can_dir);
	dprintf(2, " >abs      =%s\n", (*p)->abs);
	dprintf(2, " >absdir   =%s\n", (*p)->abs_dir);
	dprintf(2, " >rel      =%s\n", (*p)->rel);
	dprintf(2, " >reldir   =%s\n", (*p)->rel_dir);
	dprintf(2, " >name     =%s\n", (*p)->name);
	dprintf(2, " >ext      =%s\n", (*p)->ext);
	dprintf(2, " >ext_last =%s\n", (*p)->ext_last);
}
*/

	return 1;
}

int path_create_canon(path ** const p, const char * fmt, ...)
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
		free(p->can);
		free(p->abs);
		free(p->rel);
		free(p->can_dir);
		free(p->abs_dir);
		free(p->rel_dir);
		free(p->name);
		free(p->ext);
		free(p->ext_last);
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

	if(((*B)->can = strdup(A->can)) == 0)
		return 0;
	if(((*B)->abs = strdup(A->abs)) == 0)
		return 0;
	if(((*B)->rel = strdup(A->rel)) == 0)
		return 0;
	if(((*B)->can_dir = strdup(A->can_dir)) == 0)
		return 0;
	if(((*B)->abs_dir = strdup(A->abs_dir)) == 0)
		return 0;
	if(((*B)->rel_dir = strdup(A->rel_dir)) == 0)
		return 0;
	if(((*B)->name = strdup(A->name)) == 0)
		return 0;
	if(A->ext && ((*B)->ext = strdup(A->ext)) == 0)
		return 0;
	if(A->ext_last && ((*B)->ext_last = strdup(A->ext_last)) == 0)
		return 0;
	
	return 1;
}
