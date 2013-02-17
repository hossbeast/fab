#include <string.h>

#include <listwise/lstack.h>

#include "var.h"

#include "control.h"
#include "log.h"

///
/// private
///

static int var_set(map * vmap, char * s, lstack * ls, int stickys)
{
	if(sticky || map_get(vmap, s, strlen(s)) == 0)
	{
		uint64_t lo = 0;

		if(strcmp(s, "@") == 0)
			lo = L_VARAUTO;
		else if(strcmp(s, "#") == 0)
			lo = L_VARAUTO;
		else if(strcmp(s, "<") == 0)
			lo = L_VARAUTO;
		else
			lo = L_VARUSER;

		if(log_would(lo))
		{
			log_start(lo, "%10s = [ ", s);

			int i = 0;
			int j = 0;

			LSTACK_ITERATE(ls, i, go);
			if(go)
			{
				if(j++)
					log_add(" ");

				if(ls->s[0].s[i].type)
				{
					char * vs = 0;
					int    vl = 0;
					lstack_string(ls, 0, i, &vs, &vl);

					log_add("[%hhu]%p (%.*s)"
						, ls->s[0].s[i].type
						, *(void**)ls->s[0].s[i].s
						, vl
						, vs
					);
				}
				else
					log_add("%.*s", ls->s[0].s[i].l, ls->s[0].s[i].s);
			}
			LSTACK_ITEREND;
			log_finish(" ]");
		}

		fatal(map_set, vmap, s, strlen(s), &ls, sizeof(ls));
	}

	finally : coda;
}

int var_undef(const map * const vmap, const char * const s, int * r)
{
	var_container * cc = 0;
	if((cc = map_get(vmap, s, strlen(s))) == 0)
	{
		fatal(xmalloc, &cc, sizeof(*cc));
		fatal(map_set, vmap, s, strlen(s), MM(cc));
	}

	if(cc->l && cc->v[cc->l - 1].sticky)
	{
		*r = 0;	// denied
	}
	else
	{
		*r = 1;
		cc->l = 0;
	}

	finally : coda;
}

int var_pop(const map * const vmap, const char * const s)
{
	var_container * cc = 0;
	if((cc = map_get(vmap, s, strlen(s))) == 0)
	{
		fatal(xmalloc, &cc, sizeof(*cc));
		fatal(map_set, vmap, s, strlen(s), MM(cc));
	}
	
	if(cc->l)
		cc->l--;

	finally : coda;
}

int var_push(const map * const vmap, const char * const s, const void * const v, const uint8_t t, int sticky)
{
	var_container * cc = 0;
	if((cc = map_get(vmap, s, strlen(s))) == 0)
	{
		fatal(xmalloc, &cc, sizeof(*cc));
		fatal(map_set, vmap, s, strlen(s), MM(cc));
	}
	
	if(cc->l == cc->a)
	{
		int ns = cc->a ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &cc->v, sizeof(*cc->v), ns, cc->a);
		cc->a = ns;
	}

	cc->v[cc->l].ls = v;
	cc->v[cc->l].t = t;
	cc->v[cc->l].sticky = sticky;

	cc->l++;

	finally : coda;
}

int var_container_free(var_container * const restrict cc)
{
	if(cc)
	{
		free(cc->v);
	}
	free(cc);
}
