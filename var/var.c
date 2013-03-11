#include <string.h>

#include <listwise/lstack.h>

#include "var.h"
#include "log.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

#define restrict __restrict

#define IMMUTABLE				1
#define WRITETHROUGH		2
#define WRITABLE				3

#define VAL_LS					1
#define VAL_AL					2

typedef struct
{
	int					inherit;			// whether child scopes will inherit this
	int					writes;				// one of { WRITABLE, IMMUTABLE, WRITETHROUGH }
	int					val;					// one of { VAL_LS, VAL_AL }

	union
	{
		lstack *	ls;

		struct
		{
			char *	al_key;
			map *		al_map;
		};
	};
} varval;

//
// static
//

static int getval(map * const restrict vmap, const char * const restrict s, varval ** restrict c)
{
	varval ** cc = 0;
	if((c = map_get(vmap, s, strlen(s))))
	{
		(*c) = (*cc);
	}
	else
	{
		fatal(xmalloc, c, sizeof(**c));
		fatal(map_set, vmap, s, strlen(s), MM(*c));
	}

	finally : coda;
}

void map_destructor(void* tk, void* tv)
{
}

///
/// public
///

// select logtag based on variable name
#define TAG(x) (((x[0] >= 'a' && x[0] <= 'z') || (x[0] >= 'A' && x[0] <= 'Z') || x[0] == '_') ? L_VARUSER : L_VARAUTO)

int var_set(map * const vmap, const char * const s, lstack * const ls, const ff_node * const src)
{
	varval * c = 0;
	fatal(getval, vmap, s, &c);

	if(c->writes == WRITABLE)
	{
		c->inherit = 0;
		c->writes = WRITABLE;
		c->val = VAL_LS;
		c->ls = ls;
			
		if(log_would(L_VAR | TAG(s)))
		{
			log_start(L_VAR | TAG(s), "%10s(%s) = [ ", "set", s);

			int i = 0;
			int j = 0;

			lstack * ls = c->ls;

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

			if(src)
			{
				log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
					, 70 - MIN(log_written(), 70)
					, ""
					, src->loc.ff->idstring
					, src->loc.f_lin + 1
					, src->loc.f_col + 1
					, src->loc.l_lin + 1
					, src->loc.l_col + 1
				);
			}

			log_finish("");
		}
	}
	else if(c->writes == WRITETHROUGH)
	{
		log_start(L_VAR | TAG(s), "%10s(%s) -> %s", "set", s, c->al_key);
		if(src)
		{
			log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
				, 70 - MIN(log_written(), 70)
				, ""
				, src->loc.ff->idstring
				, src->loc.f_lin + 1
				, src->loc.f_col + 1
				, src->loc.l_lin + 1
				, src->loc.l_col + 1
			);
		}
		log_finish("");

		fatal(var_set, c->al_map, c->al_key, ls, src);
	}
	else
	{
		log_start(L_VAR | TAG(s), "%10s(%s) blocked", "set", s);
		if(src)
		{
			log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
				, 70 - MIN(log_written(), 70)
				, ""
				, src->loc.ff->idstring
				, src->loc.f_lin + 1
				, src->loc.f_col + 1
				, src->loc.l_lin + 1
				, src->loc.l_col + 1
			);
		}
		log_finish("");
	}

	finally : coda;
}

int var_alias(map * const restrict amap, const char * const restrict as, map * const restrict bmap, const char * const restrict bs, const ff_node * const restrict src)
{
	varval * ac = 0;
	varval * bc = 0;
	fatal(getval, amap, as, &ac);
	fatal(getval, bmap, bs, &bc);

	bc->inherit = 1;
	bc->writes = IMMUTABLE;
	bc->val = VAL_AL;
	bc->al_key = strdup(as);
	bc->al_map = amap;

	log_start(L_VAR | TAG(as), "%10s(%s -> %s)", "link", as, bs);
	if(src)
	{
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
			, 70 - MIN(log_written(), 70)
			, ""
			, src->loc.ff->idstring
			, src->loc.f_lin + 1
			, src->loc.f_col + 1
			, src->loc.l_lin + 1
			, src->loc.l_col + 1
		);
	}
	log_finish("");

	finally : coda;
}

int var_link(map * const restrict amap, const char * const restrict as, map * const restrict bmap, const char * const restrict bs, const ff_node * const restrict src)
{
	varval * ac = 0;
	varval * bc = 0;
	fatal(getval, amap, as, &ac);
	fatal(getval, bmap, bs, &bc);

	bc->inherit = 1;
	bc->writes = WRITETHROUGH;
	bc->val = VAL_AL;
	bc->al_key = strdup(as);
	bc->al_map = amap;

	log_start(L_VAR | TAG(as), "%10s(%s <-> %s)", "link", as, bs);
	if(src)
	{
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"
			, 70 - MIN(log_written(), 70)
			, ""
			, src->loc.ff->idstring
			, src->loc.f_lin + 1
			, src->loc.f_col + 1
			, src->loc.l_lin + 1
			, src->loc.l_col + 1
		);
	}
	log_finish("");

	finally : coda;
}

int var_clone(map * const amap, map ** const bmap)
{
	char **		keys = 0;
	varval **	vals = 0;
	int				keysl = 0;

	fatal(map_create, bmap, map_destructor);
	if(amap)
	{
		fatal(map_keys, amap, &keys, &keysl);
		fatal(map_values, amap, &vals, &keysl);

		int x;
		for(x = 0; x < keysl; x++)
		{
			if(vals[x]->inherit)
			{
				varval * c = 0;
				fatal(getval, (*bmap), keys[x], &c);

				c->inherit	= vals[x]->inherit;
				c->ls				= vals[x]->ls;
			}
		}
	}

finally:
	free(keys);
	free(vals);
coda;
}

lstack * var_access(const map * vmap, const char * s)
{
	varval ** cc = 0;
	while((cc = map_get(vmap, MMS(s))))
	{
		if((*cc)->val == VAL_LS)
			return (*cc)->ls;

		vmap = (*cc)->al_map;
		s = (*cc)->al_key;
	}

	return listwise_identity;
}
