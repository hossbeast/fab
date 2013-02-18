#include <string.h>

#include <listwise/lstack.h>

#include "var.h"

#include "xmem.h"
#include "control.h"
#include "log.h"

#define restrict __restrict

static int ensure_exists(map * const restrict vmap, const char * const restrict s, var_container ** const restrict con)
{
	var_container * c = 0;
	var_container ** cc = 0;

	if((cc = map_get(vmap, s, strlen(s))))
	{
		c = (*cc);
	}
	else
	{
		fatal(xmalloc, &c, sizeof(*c));
		fatal(map_set, vmap, s, strlen(s), MM(c));
	}

	(*con) = c;

	finally : coda;
}

#undef restrict

///
/// public
///

// select logtag based on variable name
#define TAG strcmp(s, "@") && strcmp(s, "#") && strcmp(s, "<") ? L_VARUSER : L_VARAUTO

int var_undef(map * const vmap, const char * const s, int * r)
{
	var_container * cc = 0;
	fatal(ensure_exists, vmap, s, &cc);

	if(cc->l && cc->v[cc->l - 1].sticky)
	{
		*r = 0;	// denied
	}
	else
	{
		log(TAG, "%10s(%s)", "undef", s);

		*r = 1;
		cc->l = 0;
	}

	finally : coda;
}

int var_pop(map * const vmap, const char * const s)
{
	var_container * cc = 0;
	fatal(ensure_exists, vmap, s, &cc);
	
	log(TAG, "%10s(%s)", "pop", s);

	if(cc->l)
		cc->l--;

	finally : coda;
}

int var_push(map * const vmap, const char * const s, void * const v, const uint8_t t, int sticky)
{
	var_container * cc = 0;
	fatal(ensure_exists, vmap, s, &cc);
	
	if(cc->l == cc->a)
	{
		int ns = cc->a ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &cc->v, sizeof(*cc->v), ns, cc->a);
		cc->a = ns;
	}

	cc->v[cc->l].ls = v;
	cc->v[cc->l].type = t;
	cc->v[cc->l].sticky = sticky;

	if(t == VV_LS)
	{
		if(log_would(TAG))
		{
			log_start(TAG, "%10s(%s, [ ", "push", s);

			int i = 0;
			int j = 0;

			lstack * ls = v;

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
				{
					log_add("%.*s", ls->s[0].s[i].l, ls->s[0].s[i].s);
				}
			}
			LSTACK_ITEREND;
			log_finish(" ] )");
		}
	}
	if(t == VV_AL)
	{
		log(TAG, "%10s(%s, alias(%s))", "push", s, v); 
	}

	cc->l++;

	finally : coda;
}

void var_container_free(var_container * const cc)
{
	if(cc)
		free(cc->v);

	free(cc);
}
