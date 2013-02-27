#include <string.h>

#include "strstack.h"

#include "control.h"
#include "xmem.h"

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

int strstack_string(strstack * const stk, const char * const d, char ** const r)
{
	int dl = strlen(d);

	int x;
	for(x = 0; x < stk->l; x++)
	{
		int r = stk->v[x].l;
		if(x)
			r += dl;

		while(stk->sa <= (stk->sl + r))
		{
			int ns = stk->sa ?: 10;
			ns = ns * 2 + ns / 2;

			fatal(xrealloc, &stk->s, sizeof(*stk->s), ns, stk->sa);
			stk->sa = ns;
		}

		if(x)
		{
			memcpy(stk->s + stk->sl, d, dl);
			stk->sl += dl;
		}

		memcpy(stk->s + stk->sl, stk->v[x].v, stk->v[x].l);
		stk->s[stk->sl + stk->v[x].l] = 0;
		stk->sl += stk->v[x].l;
	}

	(*r) = stk->s;

	finally : coda;
}
