#include <string.h>

#include <listwise/lstack.h>

#include "var.h"
#include "log.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

#define restrict __restrict

#define NOTINHERITED		0
#define INHERITED				1

#define ASSIGNABLE			0		// write reassign the key
#define IMMUTABLE				1		// write are ignored
#define WRITETHROUGH		2		// write backpropagate

#define VAL_LS					1
#define VAL_AL					2

typedef struct
{
	int					inherit;			// whether child scopes will inherit this
	int					write;				// one of { ASSIGNABLE, IMMUTABLE, WRITETHROUGH }
	int					val;					// one of { VAL_LS, VAL_AL }

	union
	{
		struct					// VV_LS
		{
			lstack *	ls;
		};

		struct					// VV_AL
		{
			map *		smap;
			char *	skey;
		};
	};
} varval;

//
// static
//

static int getval(map * const restrict vmap, const char * const restrict s, varval ** restrict c)
{
	varval ** cc = 0;
	if((cc = map_get(vmap, s, strlen(s))))
	{
		(*c) = (*cc);
	}
	else
	{
		fatal(xmalloc, c, sizeof(**c));
		fatal(map_set, vmap, s, strlen(s), c, sizeof(*c));
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

int var_set(map * vmap, const char * s, lstack * const ls, int inherit, int mutable, const ff_node * const src)
{
	int * inh = (int[1]) { inherit };
	int * wrt = (int[1]) { mutable ? ASSIGNABLE : IMMUTABLE };

	varval * c = 0;
	fatal(getval, vmap, s, &c);
	while(c && c->write == WRITETHROUGH)
	{
		// writethrough does not modify properties
		inh = 0;
		wrt = 0;

		log_start(L_VAR | TAG(s), "%10s(%s -> %s)", "redir", s, c->skey);
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

		vmap = c->smap;
		s = c->skey;
		fatal(getval, vmap, s, &c);
	}

	if(c->write == ASSIGNABLE)
	{
		if(inh)
			c->inherit	= *inh;
		if(wrt)
			c->write		= *wrt; 

		c->val			= VAL_LS;
		c->ls				= ls;
			
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

int var_alias(map * const restrict smap, const char * const restrict ss, map * const restrict tmap, const char * const restrict ts, const ff_node * const restrict src)
{
	varval * sc = 0;
	varval * tc = 0;
	fatal(getval, smap, ss, &sc);
	fatal(getval, tmap, ts, &tc);

	if(tc->write == ASSIGNABLE)
	{
		tc->inherit	= 1;
		tc->write		= IMMUTABLE;
		tc->val			= VAL_AL;
		tc->skey		= strdup(ss);
		tc->smap		= smap;

		log_start(L_VAR | TAG(ss), "%10s(%s -> %s)", "alias", ss, ts);
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
	else
	{
		log_start(L_WARN | L_VAR | TAG(ss), "%10s(%s -> %s) blocked", "alias", ss, ts);
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

int var_link(map * const restrict smap, const char * const restrict ss, map * const restrict tmap, const char * const restrict ts, const ff_node * const restrict src)
{
	varval * sc = 0;
	varval * tc = 0;
	fatal(getval, smap, ss, &sc);
	fatal(getval, tmap, ts, &tc);

	if(tc->write == ASSIGNABLE)
	{
		tc->inherit	= 1;
		tc->write		= WRITETHROUGH;
		tc->val			= VAL_AL;
		tc->skey		= strdup(ss);
		tc->smap		= smap;

		log_start(L_VAR | TAG(ss), "%10s(%s <-> %s)", "link", ss, ts);
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
	else
	{
		log_start(L_WARN | L_VAR | TAG(ss), "%10s(%s <-> %s) blocked", "link", ss, ts);
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

int var_root(map ** const map)
{
	fatal(map_create, map, map_destructor);

	finally : coda;
}

int var_clone(map * const amap, map ** const bmap)
{
	char **			keys = 0;
	varval ***	vals = 0;
	int					keysl = 0;

	fatal(map_create, bmap, map_destructor);
	fatal(map_keys, amap, &keys, &keysl);
	fatal(map_values, amap, &vals, &keysl);

	int x;
	for(x = 0; x < keysl; x++)
	{
		if((*vals[x])->inherit)
		{
			varval * c = 0;
			fatal(getval, (*bmap), keys[x], &c);

			memcpy(c, *vals[x], sizeof(*c));
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

		vmap = (*cc)->smap;
		s = (*cc)->skey;
	}

	return listwise_identity;
}
