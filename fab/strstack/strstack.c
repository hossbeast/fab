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

#include <string.h>

#include "strstack.h"

#include "global.h"
#include "xlinux.h"

struct strstack_t
{
	struct
	{
		char *	v;
		int			l;
		int			a;
	} * 		v;
	int			l;
	int			a;

	char *	s;
	int			sl;
	int			sa;
};

int strstack_create(strstack ** const stk)
{
	// initial allocation
	fatal(xmalloc, stk, sizeof(**stk));
	
	finally : coda;
}

void strstack_free(strstack * const stk)
{
	if(stk)
	{
		int x;
		for(x = 0; x < stk->a; x++)
			free(stk->v[x].v);

		free(stk->v);
		free(stk->s);
	}

	free(stk);
}

void strstack_xfree(strstack ** const stk)
{
	strstack_free(*stk);
	*stk = 0;
}

void strstack_undef(strstack* const stk)
{
	stk->l = 0;
}

int strstack_push(strstack * const stk, const void* const v)
{
	if(stk->l == stk->a)
	{
		int ns = stk->a ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, &stk->v, sizeof(*stk->v), ns, stk->a);
		stk->a = ns;
	}

	int vl = strlen(v);

	while(stk->v[stk->l].a <= vl)
	{
		int ns = stk->v[stk->l].a ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, &stk->v[stk->l].v, sizeof(*stk->v[stk->l].v), ns, stk->v[stk->l].a);
		stk->v[stk->l].a = ns;
	}

	stk->v[stk->l].l = vl;
	memcpy(stk->v[stk->l].v, v, vl);
	stk->v[stk->l].v[vl] = 0;
	stk->l++;

	finally : coda;
}

void strstack_pop(strstack * const stk)
{
	if(stk->l)
		stk->l--;
}

int strstack_string(strstack * const stk, const int off, const char * const ldr, const char * const d, char ** const r)
{
	int dl = 0;
	int ldrl = 0;

	if(d)
		dl = strlen(d);

	if(ldr)
		ldrl = strlen(ldr);

	stk->sl = 0;
	while(stk->sa <= (stk->sl + ldrl))
	{
		int ns = stk->sa ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, &stk->s, sizeof(*stk->s), ns, stk->sa);
		stk->sa = ns;
	}

	memcpy(stk->s + stk->sl, ldr, ldrl);
	stk->sl += ldrl;

	int x;
	for(x = off; x < stk->l; x++)
	{
		int t = stk->v[x].l;
		if(stk->sl)
			t += dl;

		while(stk->sa <= (stk->sl + t))
		{
			int ns = stk->sa ?: 10;
			ns = ns * 2 + ns / 2;

			fatal(xrealloc, &stk->s, sizeof(*stk->s), ns, stk->sa);
			stk->sa = ns;
		}

		if(x != off && d)
		{
			memcpy(stk->s + stk->sl, d, dl);
			stk->sl += dl;
		}

		memcpy(stk->s + stk->sl, stk->v[x].v, stk->v[x].l);
		stk->sl += stk->v[x].l;
	}

	stk->s[stk->sl] = 0;

	(*r) = stk->s;

	finally : coda;
}
