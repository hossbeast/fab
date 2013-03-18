#include <string.h>

#include <listwise/lstack.h>

#include "var.h"
#include "log.h"
#include "gnlw.h"

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

static void map_destructor(void* tk, void* tv)
{
	if(((char*)tk)[0] != '?')
	{
		varval ** cc = tv;
		if((*cc)->val == VAL_AL)
			free((*cc)->skey);
		free((*cc));
	}
}

static void dumplist(lstack * const ls)
{
	int i = 0;
	int j = 0;

	LSTACK_ITERATE(ls, i, go);
	if(go)
	{
		if(j++)
			log_add(" ");

		if(ls->s[0].s[i].type == LISTWISE_TYPE_GNLW)
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
		else if(ls->s[0].s[i].type == LISTWISE_TYPE_LIST)
		{
			log_add("[ ");
			dumplist(*(void**)ls->s[0].s[i].s);
			log_add(" ]");
		}
		else
		{
			log_add("%.*s", ls->s[0].s[i].l, ls->s[0].s[i].s);
		}
	}
	LSTACK_ITEREND;
}

///
/// public
///

// select logtag based on variable name
#define TAG(x) (((x[0] >= 'a' && x[0] <= 'z') || (x[0] >= 'A' && x[0] <= 'Z') || x[0] == '_') ? L_VARUSER : L_VARAUTO)

#define KEYID(m, s) *(int*)map_get(m, MMS("?LVL")), *(int*)map_get(m, MMS("?NUM")), s

#define LOG_SRC(s) 														\
do {																					\
	if(s)																				\
	{																						\
		log_add("%*s @ (%s)[%3d,%3d - %3d,%3d]"		\
			, 70 - MIN(log_written(), 70)						\
			, ""																		\
			, s->loc.ff->idstring										\
			, s->loc.f_lin + 1											\
			, s->loc.f_col + 1											\
			, s->loc.l_lin + 1											\
			, s->loc.l_col + 1											\
		);																				\
	}																						\
} while(0)
	

int var_set(map * vmap, const char * s, lstack * const ls, int inherit, int mutable, const ff_node * const src)
{
	int * inh = (int[1]) { inherit };
	int * wrt = (int[1]) { mutable ? ASSIGNABLE : IMMUTABLE };

	varval * c = 0;
	fatal(getval, vmap, s, &c);
	while(c->write == WRITETHROUGH)
	{
		// writethrough does not modify properties
		inh = 0;
		wrt = 0;

		log_start(L_VAR | TAG(s), "%10s(%d:%d:%s -> %d:%d:%s)", "redir", KEYID(vmap, s), KEYID(c->smap, c->skey));
		LOG_SRC(src);
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

		if(ls->l && ls->s[0].l)
			c->ls				= ls;
		else
			c->ls				= listwise_identity;
			
		if(log_would(L_VAR | TAG(s)))
		{
			log_start(L_VAR | TAG(s), "%10s(%d:%d:%s) = [ ", "set", KEYID(vmap, s));
			dumplist(c->ls);
			log_add(" ] )");
			LOG_SRC(src);
			log_finish("");
		}
	}
	else
	{
		log_start(L_VAR | TAG(s), "%10s(%d:%d:%s) blocked", "set", KEYID(vmap, s));
		LOG_SRC(src);
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

		log_start(L_VAR | TAG(ss), "%10s(%d:%d:%s -> %d:%d:%s)", "alias", KEYID(tmap, ts), KEYID(smap, ss));
		LOG_SRC(src);
		log_finish("");
	}
	else
	{
		/* this could potentially merit a warning */
		log_start(L_VAR | TAG(ss), "%10s(%d:%d:%s -> %d:%d:%s) blocked", "alias", KEYID(tmap, ts), KEYID(smap, ss));
		LOG_SRC(src);
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

		log_start(L_VAR | TAG(ss), "%10s(%d:%d:%s <-> %d:%d:%s)", "link", KEYID(tmap, ts), KEYID(smap, ss));
		LOG_SRC(src);
		log_finish("");
	}
	else
	{
		/* this could potentially merit a warning */
		log_start(L_VAR | TAG(ss), "%10s(%d:%d:%s <-> %d:%d:%s) blocked", "link", KEYID(tmap, ts), KEYID(smap, ss));
		LOG_SRC(src);
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
		if(keys[x][0] != '?')
		{
			if((*vals[x])->inherit)
			{
				varval * c = 0;
				fatal(getval, (*bmap), keys[x], &c);

				memcpy(c, *vals[x], sizeof(*c));
				if(c->val == VAL_AL)
					c->skey = strdup(c->skey);
			}
		}
	}

	int * ALVL = map_get(amap, MMS("?LVL"));
	int * ACLD = map_get(amap, MMS("?CLD"));

	int BLVL = (*ALVL) + 1;
	int BNUM = (*ACLD)++;
	int BCLD = 0;

	fatal(map_set, (*bmap), MMS("?LVL"), MM(BLVL));
	fatal(map_set, (*bmap), MMS("?NUM"), MM(BNUM));
	fatal(map_set, (*bmap), MMS("?CLD"), MM(BCLD));

finally:
	free(keys);
	free(vals);
coda;
}

lstack * var_access(const map * vmap, const char * vs)
{
	const map * m = vmap;
	const char * s = vs;
	lstack * ls = listwise_identity;

	varval ** cc = 0;
	while((cc = map_get(m, MMS(s))))
	{
		if((*cc)->val == VAL_LS)
		{
			ls = (*cc)->ls;
			break;
		}
		else if((*cc)->val == VAL_AL)
		{
			m = (*cc)->smap;
			s = (*cc)->skey;
		}
		else
		{
			// aliased to a var which was never subsequently defined
			break;
		}
	}

/*
	if(log_would(L_VAR | TAG(vs)))
	{
		log_start(L_VAR | TAG(s), "%10s(%d:%d:%s) = [ ", "access", KEYID(vmap, vs));
		dumplist(ls);
		log_add(" ] ");
		log_finish("");
	}
*/

	return ls;
}
