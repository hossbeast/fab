#include <string.h>

#include <listwise/lstack.h>

#include "var.h"
#include "log.h"

#include "xmem.h"
#include "control.h"

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
#define TAG(x) (((x[0] >= 'a' && x[0] <= 'z') || (x[0] >= 'A' && x[0] <= 'Z') || x[0] == '_') ? L_VARUSER : L_VARAUTO)

int var_undef(map * const vmap, const char * const s, int * r, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
{
	var_container * cc = 0;
	fatal(ensure_exists, vmap, s, &cc);

	if(cc->l && cc->v[cc->l - 1].sticky)
	{
		*r = 0;	// denied
	}
	else
	{
		log(L_VAR | TAG(s), "%10s(%s)", "undef", s);

		*r = 1;
		cc->l = 0;
	}

	finally : coda;
}

int var_pop(map * const vmap, const char * const s, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
{
	var_container * cc = 0;
	fatal(ensure_exists, vmap, s, &cc);
	
	log(L_VAR | TAG(s), "%10s(%s)", "pop", s);

	// disclaim ownership of this stack instance
	// ensure enough lstacks are allocated
	if((*staxa) <= staxp)
	{
		int ns = (*staxa) ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, stax, sizeof(**stax), ns, (*staxa));
		(*staxa) = ns;
	}
	

	if(cc->l)
		cc->l--;

	finally : coda;
}

int var_push_alias(map * const restrict vmap, const char * const restrict s, int sticky, char * const restrict v)
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
	cc->v[cc->l].type = VV_AL;
	cc->v[cc->l].sticky = sticky;
	cc->l++;

	log(L_VAR | TAG(s), "%10s(%s, alias($%s))", "push", s, v); 

	finally : coda;
}

int var_push_list(map * const restrict vmap, const char * const restrict s, int sticky, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
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

	cc->v[cc->l].type = VV_LS;
	cc->v[cc->l].sticky = sticky;

	// claim this lstack instance
	cc->v[cc->l].ls = (*stax)[(*staxp)];
	(*stax)[(*staxp)] = 0;
/*
	memcpy(
		  &(*stax)[(*staxp)]
		, &(*stax)[(*staxp)+1]
		, ((*staxa) - (*staxp) - 1) * sizeof((*stax)[0])
	);
	(*staxa)--;
*/

	cc->l++;

	if(log_would(L_VAR | TAG(s)))
	{
		log_start(L_VAR | TAG(s), "%10s(%s, [ ", "push", s);

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

	finally : coda;
}

lstack * var_access(const map * const vmap, const char * s)
{
	var_container ** c = 0;

	while((c = map_get(vmap, s, strlen(s))))
	{
		if((*c)->l == 0)
			return listwise_identity;

		if((*c)->v[(*c)->l - 1].type == VV_LS)
			break;

		s = (*c)->v[(*c)->l - 1].alias;
	}

	if(c && (*c)->l)
		return (*c)->v[(*c)->l - 1].ls;

	return listwise_identity;
}

void var_container_free(var_container * const cc)
{
	if(cc)
		free(cc->v);

	free(cc);
}
