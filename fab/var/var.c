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

#include "listwise.h"
#include "listwise/generator.h"
#include "listwise/lstack.h"

#include "var.h"

#include "logs.h"
#include "gnlw.h"
#include "ff.h"
#include "ffn.h"
#include "lwutil.h"

#include "xlinux.h"
#include "global.h"
#include "macros.h"

#define restrict __restrict

#define NOTINHERITED		0
#define INHERITED				1

#define ASSIGNABLE			0		// write : reassign the key
#define IMMUTABLE				1		// write : ignore
#define WRITETHROUGH		2		// write : backpropagate

#define VAL_LS					1
#define VAL_AL					2

#define XFM_ADD					1
#define XFM_SUB					2
#define XFM_LW					3

typedef struct
{
	int					inherit;			// whether child scopes inherit this value
	int					write;				// one of { ASSIGNABLE, IMMUTABLE, WRITETHROUGH }
	int					type;					// one of { VAL_LS, VAL_AL }

	union
	{
		struct					// VV_LS
		{
			lwx *	ls;
		};

		struct					// VV_AL
		{
			map *		smap;
			char *	skey;
		};
	};
} varval;

typedef struct
{
	int inherit;			// whether child scopes inherit this xfm
	int type;					// one of XFM_*

	union
	{
		void *			xfm;
		lwx *		add;	// XFM_ADD			( not owned here - all lstacks owned by main )
		lwx *		sub;	// XFM_SUB			( not owned here - all lstacks owned by main )

		struct {					// XFM_LW				( not owned here - all generators owned by FFN_NODEs )
			generator * gen;
			char *			tex;
		};
	};
} varxfm;

typedef struct
{
	varval			val;

	varxfm *		xfms;
	int					xfmsl;
	int					xfmsa;
	int					xfmsi;		// number of inherited xfms
} vardef;

//
// static
//

// select logtag based on variable name
#define TAG(x) (((x[0] >= 'a' && x[0] <= 'z') || (x[0] >= 'A' && x[0] <= 'Z') || x[0] == '_') ? L_VARUSER : L_VARAUTO)

#define KEYID(m, s) *(int*)map_get(m, MMS("?LVL")), *(int*)map_get(m, MMS("?NUM")), s

#define LOG_SRC(s) 														\
do {																					\
	if(s)																				\
	{																						\
		logf(0, "%*s @ (%s)[%3d,%3d - %3d,%3d]"		\
			, 70 - MIN(logged_characters(), 70)			\
			, ""																		\
			, s->loc.ff->idstring										\
			, s->loc.f_lin + 1											\
			, s->loc.f_col + 1											\
			, s->loc.l_lin + 1											\
			, s->loc.l_col + 1											\
		);																				\
	}																						\
} while(0)

static int getdef(map * const restrict vmap, const char * const restrict s, vardef ** restrict c)
{
	vardef ** cc = 0;
	if((cc = map_get(vmap, MMS(s))))
	{
		(*c) = (*cc);
	}
	else
	{
		fatal(xmalloc, c, sizeof(**c));
		fatal(map_set, vmap, s, strlen(s), c, sizeof(*c), 0);
	}

	finally : coda;
}

static void map_destructor(void* tk, void* tv)
{
	if(((char*)tk)[0] != '?')
	{
		vardef ** cc = tv;
		if((*cc)->val.type == VAL_AL)
			free((*cc)->val.skey);

		free((*cc)->xfms);
		free((*cc));
	}
}

static int dumplist(lwx * const ls)
{
	int i = 0;
	int j = 0;

	LSTACK_ITERATE(ls, i, go);
	if(go)
	{
		if(j++)
			logs(0, " ");

		char * rv;
		int rl;
		uint8_t rt;
		fatal(lstack_readrow, ls, 0, i, &rv, &rl, &rt, 0, 0, 0, 0);

		if(rt == LISTWISE_TYPE_GNLW)
		{
			char * zs = 0;
			fatal(lstack_string, ls, 0, i, &zs);
			logf(0, "[%hhu]%p (%.*s)"
				, rt
				, *(void**)rv
				, zs
			);
		}
		else if(rt == LISTWISE_TYPE_LIST)
		{
			logs(0, "[ ");
			fatal(dumplist, *(void**)rv);
			logs(0, " ]");
		}
		else
		{
			logf(0, "%.*s", rl, rv);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int xfm_add(map * restrict vmap, const char * restrict s, int type, void * const restrict xfm, char * const restrict tex, int inherit, const struct ff_node * const restrict src, vardef ** c)
{
	fatal(getdef, vmap, s, c);
	while((*c)->val.write == WRITETHROUGH)
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s -> %d:%d:%s)", "redir", KEYID(vmap, s), KEYID((*c)->val.smap, (*c)->val.skey));
			LOG_SRC(src);
			log_finish();
		}

		vmap = (*c)->val.smap;
		s = (*c)->val.skey;
		fatal(getdef, vmap, s, &(*c));
	}

	if((*c)->val.write == ASSIGNABLE)
	{
		if((*c)->xfmsl == (*c)->xfmsa)
		{
			int newa = (*c)->xfmsa ?: 3;
			newa = newa * 2 + newa / 2;
			fatal(xrealloc, &(*c)->xfms, sizeof(*(*c)->xfms), newa, (*c)->xfmsa);
			(*c)->xfmsa = newa;
		}

		varxfm * vx = &(*c)->xfms[(*c)->xfmsl++];

		vx->inherit = inherit;
		vx->type = type;
		vx->xfm = xfm;
		vx->tex = tex;
	}

	finally : coda;
}

///
/// public
///

int var_set(map * restrict vmap, const char * restrict s, lwx * const restrict ls, int inherit, int mutable, const struct ff_node * const restrict src)
{
	int * inh = (int[1]) { inherit };
	int * wrt = (int[1]) { mutable ? ASSIGNABLE : IMMUTABLE };

	vardef * c = 0;
	fatal(getdef, vmap, s, &c);
	while(c->val.write == WRITETHROUGH)
	{
		// writethrough does not modify properties
		inh = 0;
		wrt = 0;

		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s -> %d:%d:%s)", "redir", KEYID(vmap, s), KEYID(c->val.smap, c->val.skey));
			LOG_SRC(src);
			log_finish();
		}

		vmap = c->val.smap;
		s = c->val.skey;
		fatal(getdef, vmap, s, &c);
	}

	if(c->val.write == ASSIGNABLE)
	{
		if(inh)
			c->val.inherit	= *inh;
		if(wrt)
			c->val.write		= *wrt; 

		c->val.type				= VAL_LS;

		if(lwx_lists(ls) && lwx_rows(ls, 0))
			c->val.ls				= ls;
		else
			c->val.ls				= listwise_identity;

		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) = [ ", "set", KEYID(vmap, s));
			fatal(dumplist, c->val.ls);
			logs(0, " ] )");
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) blocked", "set", KEYID(vmap, s));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_xfm_add(map * restrict vmap, const char * restrict s, lwx * const restrict ls, int inherit, const struct ff_node * const restrict src)
{
	vardef * c = 0;
	fatal(xfm_add, vmap, s, XFM_ADD, ls, 0, inherit, src, &c);

	if(c->val.write == ASSIGNABLE)
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) = [ ", "xfm-add", KEYID(vmap, s));
			fatal(dumplist, ls);
			logs(0, " ] )");
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) blocked", "xfm-add", KEYID(vmap, s));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_xfm_sub(map * restrict vmap, const char * restrict s, lwx * const restrict ls, int inherit, const struct ff_node * const restrict src)
{
	vardef * c = 0;
	fatal(xfm_add, vmap, s, XFM_SUB, ls, 0, inherit, src, &c);

	if(c->val.write == ASSIGNABLE)
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) = [ ", "xfm-sub", KEYID(vmap, s));
			fatal(dumplist, ls);
			logs(0, " ] )");
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) blocked", "xfm-sub", KEYID(vmap, s));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_xfm_lw(map * restrict vmap, const char * restrict s, generator * const restrict gen, char * const restrict tex, int inherit, const struct ff_node * const restrict src)
{
	vardef * c = 0;
	fatal(xfm_add, vmap, s, XFM_LW, gen, tex, inherit, src, &c);

	if(c->val.write == ASSIGNABLE)
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) ~= %s", "xfm-lw", KEYID(vmap, s), tex);
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		if(log_start(L_VAR | TAG(s)))
		{
			logf(0, "%10s(%d:%d:%s) blocked", "xfm-lw", KEYID(vmap, s));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_alias(map * const restrict smap, const char * const restrict ss, map * const restrict tmap, const char * const restrict ts, const ff_node * const restrict src)
{
	vardef * tc = 0;
	fatal(getdef, tmap, ts, &tc);

	if(tc->val.write == ASSIGNABLE)
	{
		tc->val.inherit	= 1;
		tc->val.write		= IMMUTABLE;
		tc->val.type		= VAL_AL;
		tc->val.skey		= strdup(ss);
		tc->val.smap		= smap;

		if(log_start(L_VAR | TAG(ss)))
		{
			logf(L_VAR | TAG(ss), "%10s(%d:%d:%s -> %d:%d:%s)", "alias", KEYID(tmap, ts), KEYID(smap, ss));
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		// If it was cloned into this map as immutable, this is a no-op
		// However if it was cloned into this map as writethrough, then aliasing it has the necessary effect of disabling writethrough
		// and causing the def to revert to a normal immutable mapping
		tc->val.write = IMMUTABLE;

		if(log_start(L_VAR | TAG(ss)))
		{
			/* this could potentially merit a warning */
			logf(L_VAR | TAG(ss), "%10s(%d:%d:%s -> %d:%d:%s) blocked", "alias", KEYID(tmap, ts), KEYID(smap, ss));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_link(map * const restrict smap, const char * const restrict ss, map * const restrict tmap, const char * const restrict ts, const ff_node * const restrict src)
{
	vardef * tc = 0;
	fatal(getdef, tmap, ts, &tc);

	if(tc->val.write == ASSIGNABLE)
	{
		tc->val.inherit	= 1;
		tc->val.write		= WRITETHROUGH;
		tc->val.type		= VAL_AL;
		tc->val.skey		= strdup(ss);
		tc->val.smap		= smap;

		if(log_start(L_VAR | TAG(ss)))
		{
			logf(L_VAR | TAG(ss), "%10s(%d:%d:%s <-> %d:%d:%s)", "link", KEYID(tmap, ts), KEYID(smap, ss));
			LOG_SRC(src);
			log_finish();
		}
	}
	else
	{
		if(log_start(L_VAR | TAG(ss)))
		{
			/* this could potentially merit a warning */
			logf(L_VAR | TAG(ss), "%10s(%d:%d:%s <-> %d:%d:%s) blocked", "link", KEYID(tmap, ts), KEYID(smap, ss));
			LOG_SRC(src);
			log_finish();
		}
	}

	finally : coda;
}

int var_root(map ** const map)
{
	fatal(map_create, map, map_destructor);

	// seed the map identifier mechanism
	fatal(map_set, *map, MMS("?LVL"), MM((int[]) { 0 }), 0);
	fatal(map_set, *map, MMS("?NUM"), MM((int[]) { 0 }), 0);
	fatal(map_set, *map, MMS("?CLD"), MM((int[]) { 0 }), 0);

	finally : coda;
}

int var_clone(map * const amap, map ** const bmap)
{
	char **			keys = 0;
	vardef ***	defs = 0;
	int					keysl = 0;

	fatal(map_create, bmap, map_destructor);
	fatal(map_keys, amap, &keys, &keysl);
	fatal(map_values, amap, &defs, &keysl);

	int x;
	for(x = 0; x < keysl; x++)
	{
		if(keys[x][0] != '?')
		{
			vardef * c = 0;

			// inherit the value
			if((*defs[x])->val.inherit)
			{
				fatal(getdef, (*bmap), keys[x], &c);

				memcpy(&c->val, &(*defs[x])->val, sizeof(c->val));
				if(c->val.type == VAL_AL)
					c->val.skey = strdup(c->val.skey);
			}

			// inherit the xfms
			int y;
			for(y = 0; y < (*defs[x])->xfmsl; y++)
			{
				if((*defs[x])->xfms[y].inherit)
				{
					fatal(getdef, (*bmap), keys[x], &c);

					if(c->xfmsl == c->xfmsa)
					{
						int newa = c->xfmsa ?: 3;
						newa = newa * 2 + newa / 2;
						fatal(xrealloc, &c->xfms, sizeof(*c->xfms), newa, c->xfmsa);
						c->xfmsa = newa;
					}

					varxfm * xfm = &c->xfms[c->xfmsl++];
					memcpy(xfm, &(*defs[x])->xfms[y], sizeof(*xfm));

					// track inherited
					c->xfmsi++;
				}
			}
		}
	}

	int * ALVL = map_get(amap, MMS("?LVL"));
	int * ACLD = map_get(amap, MMS("?CLD"));
	int * ANUM = map_get(amap, MMS("?NUM"));

	int BLVL = (*ALVL) + 1;
	int BNUM = (*ANUM) + (*ACLD) + 1;
	int BCLD = 0;

	(*ACLD) = (*ACLD) + 1;

	fatal(map_set, (*bmap), MMS("?LVL"), MM(BLVL), 0);
	fatal(map_set, (*bmap), MMS("?NUM"), MM(BNUM), 0);
	fatal(map_set, (*bmap), MMS("?CLD"), MM(BCLD), 0);

finally:
	free(keys);
	free(defs);
coda;
}

int var_access(const map * const restrict vmap, const char * restrict vs, lwx *** const restrict stax, int * const restrict staxa, int * const restrict staxp, lwx ** const restrict ls)
{
	const map * m = vmap;
	const char * s = vs;
	(*ls) = listwise_identity;

	vardef ** cc = 0;
	while((cc = map_get(m, MMS(s))))
	{
		if((*cc)->val.type == VAL_LS)
		{
			(*ls) = (*cc)->val.ls;
			break;
		}
		else if((*cc)->val.type == VAL_AL)
		{
			m = (*cc)->val.smap;
			s = (*cc)->val.skey;
		}
		else
		{
			// aliased to a var which was never subsequently defined
			break;
		}
	}

	if(cc)		// apply xfms
	{
		int x;
		for(x = 0; x < (*cc)->xfmsl; x++)
		{
			if((*cc)->xfms[x].type == XFM_ADD || (*cc)->xfms[x].type == XFM_LW)
			{
				int pn = (*staxp)++;
				fatal(lw_reset, stax, staxa, pn);

				if((*cc)->xfms[x].type == XFM_ADD)
				{
					fatal(lstack_obj_add, (*stax)[pn], (*ls), LISTWISE_TYPE_LIST);
					fatal(lstack_obj_add, (*stax)[pn], (*cc)->xfms[x].add, LISTWISE_TYPE_LIST);
				}
				else if((*cc)->xfms[x].type == XFM_LW)
				{
					fatal(lstack_obj_add, (*stax)[pn], (*ls), LISTWISE_TYPE_LIST);
					fatal(lw_exec, (*cc)->xfms[x].gen, &(*stax)[pn]);
				}

				(*ls) = (*stax)[pn];
			}
			else if((*cc)->xfms[x].type == XFM_SUB)
			{

			}
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

	finally : coda;
}
