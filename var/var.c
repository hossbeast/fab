#include <string.h>

#include <listwise/lstack.h>

#include "var.h"
#include "log.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

#define restrict __restrict

static int getcontainer(map * const restrict vmap, const char * const restrict s, var_container ** const restrict con)
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

int var_undef(map * const vmap, const char * const s, int * r, const ff_node * const o)
{
	var_container * cc = 0;
	fatal(getcontainer, vmap, s, &cc);

	if(cc->l && cc->v[cc->l - 1].sticky)
	{
		*r = 0;	// denied
		log_start(L_VAR | TAG(s), "%10s(%s) prevented", "undef", s);
	}
	else
	{
		*r = 1;
		cc->l = 0;
		log_start(L_VAR | TAG(s), "%10s(%s)", "undef", s);
	}

	if(o)
	{
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
			, 70 - MIN(log_written(), 70)
			, ""
			, o->loc.ff->idstring
			, o->loc.f_lin + 1
			, o->loc.f_col + 1
			, o->loc.l_lin + 1
			, o->loc.l_col + 1
		);
	}
	log_finish("");

	finally : coda;
}

int var_pop(map * const vmap, const char * const s, const ff_node * const o)
{
	var_container * cc = 0;
	fatal(getcontainer, vmap, s, &cc);
	
	log_start(L_VAR | TAG(s), "%10s(%s)", "pop", s);
	if(o)
	{
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
			, 70 - MIN(log_written(), 70)
			, ""
			, o->loc.ff->idstring
			, o->loc.f_lin + 1
			, o->loc.f_col + 1
			, o->loc.l_lin + 1
			, o->loc.l_col + 1
		);
	}
	log_finish("");

	if(cc->l)
		cc->l--;

	finally : coda;
}

int var_push_alias(map * const vmap, const char * const s, int sticky, char * const v, const ff_node * const o)
{
	var_container * cc = 0;
	fatal(getcontainer, vmap, s, &cc);
	
	if(cc->l == cc->a)
	{
		int ns = cc->a ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &cc->v, sizeof(*cc->v), ns, cc->a);
		cc->a = ns;
	}

	cc->v[cc->l].alias = v;
	cc->v[cc->l].type = VV_AL;
	cc->v[cc->l].sticky = sticky;
	cc->l++;

	log_start(L_VAR | TAG(s), "%10s(%s, alias($%s))", "push", s, v);
	if(o)
	{
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
			, 70 - MIN(log_written(), 70)
			, ""
			, o->loc.ff->idstring
			, o->loc.f_lin + 1
			, o->loc.f_col + 1
			, o->loc.l_lin + 1
			, o->loc.l_col + 1
		);
	}
	log_finish("");

	finally : coda;
}

int var_push_list(map * const vmap, const char * const s, int sticky, lstack * const ls, const ff_node * const o)
{
	var_container * cc = 0;
	fatal(getcontainer, vmap, s, &cc);
	
	if(cc->l == cc->a)
	{
		int ns = cc->a ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &cc->v, sizeof(*cc->v), ns, cc->a);
		cc->a = ns;
	}

	cc->v[cc->l].type = VV_LS;
	cc->v[cc->l].sticky = sticky;

	if(ls->l && ls->s[0].l)
		cc->v[cc->l++].ls = ls;
	else
		cc->v[cc->l++].ls = listwise_identity;

	if(log_would(L_VAR | TAG(s)))
	{
		log_start(L_VAR | TAG(s), "%10s(%s, [ ", "push", s);

		int i = 0;
		int j = 0;

		lstack * ls = cc->v[cc->l - 1].ls;

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

		log_add(" ] )");

		if(o)
		{
			log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
				, 70 - MIN(log_written(), 70)
				, ""
				, o->loc.ff->idstring
				, o->loc.f_lin + 1
				, o->loc.f_col + 1
				, o->loc.l_lin + 1
				, o->loc.l_col + 1
			);
		}

		log_finish("");
	}

	finally : coda;
}

lstack * var_access(const map * const vmap, const char * s)
{
	var_container ** c = 0;

	while((c = map_get(vmap, s, strlen(s))))
	{
		if((*c)->v[(*c)->l - 1].type == VV_LS)
			return (*c)->v[(*c)->l - 1].ls;

		s = (*c)->v[(*c)->l - 1].alias;
	}

	return listwise_identity;
}

void var_container_free(var_container * const cc)
{
	if(cc)
		free(cc->v);

	free(cc);
}
