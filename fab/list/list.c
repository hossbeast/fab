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
#include "listwise/xtra.h"
#include "listwise/object.h"
#include "listwise/generator.h"

#include "list.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"
#include "lwutil.h"

#include "global.h"
#include "map.h"
#include "xlinux.h"
#include "macros.h"

///
/// static
///

static int render(lwx * const ls, pstring ** const ps)
{
	char * dm = " ";
	if(lwx_getflags(ls) == INTERPOLATE_ADJOIN)
		dm = "";
	else if(lwx_getflags(ls) == INTERPOLATE_DELIM_CUST)
		dm = lwx_getptr(ls);

	int k = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(k++)
			fatal(pscatf, ps, "%s", dm);

		char * rv;
		int rl;
		uint8_t rt;
		fatal(lstack_readrow, ls, 0, x, &rv, &rl, &rt, 0, 0, 0, 0);

		if(rt == LISTWISE_TYPE_LIST)
		{
			fatal(render, *(void**)rv, ps);
		}
		else if(rt == LISTWISE_TYPE_GNLW)
		{
			gn * A = *(void**)rv;

			if(g_args.mode_paths == MODE_ABSOLUTE)
			{
				fatal(pscatf, ps, "%.*s", A->path->absl, A->path->abs);
			}
			else if(g_args.mode_paths == MODE_RELATIVE_FABFILE_DIR)
			{
				fatal(pscatf, ps, "%.*s", A->path->rel_fabl, A->path->rel_fab);
			}
		}
		else
		{
			fatal(pscat, ps, rv, rl);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int resolve(ff_node * list, map* vmap, generator_parser * const gp, lwx *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	// resolved lstack goes here
	int pn = (*staxp)++;

	generator *	g = 0;
	pstring *		gps = 0;

	int x;
	for(x = 0; x < list->elementsl; x++)
	{
		if(list->elements[x]->type == FFN_WORD)
		{
			fatal(lstack_add, (*stax)[pn], list->elements[x]->text, strlen(list->elements[x]->text));
		}
		else if(list->elements[x]->type == FFN_NOFILE)
		{
			fatal(lstack_add, (*stax)[pn], list->elements[x]->text, strlen(list->elements[x]->text));
		}
		else if(list->elements[x]->type == FFN_LF)
		{
			fatal(lstack_add, (*stax)[pn], "\n", 1);
		}
		else if(list->elements[x]->type == FFN_VARREF)
		{
			lwx * vls = 0;
			if(rawmap)
			{
				lwx ** cc = 0;
				if((cc = map_get(vmap, MMS(list->elements[x]->name))))
					vls = *cc;
			}
			else
			{
				fatal(var_access, vmap, list->elements[x]->name, stax, staxa, staxp, &vls);
			}

			if(vls)
			{
				// EXCEPTION for bakevars alone in a list with a generator
				if(rawvars && map_get(rawvars, MMS(list->elements[x]->name)))
				{
					// save the list-value
					fatal(map_set, rawvars, MMS(list->elements[x]->name), MM(vls), 0);

					// render a reference to the variable by name
					fatal(lstack_addf, (*stax)[pn], "$%s", list->elements[x]->name);
				}
				else
				{
					fatal(lstack_obj_add, (*stax)[pn], vls, LISTWISE_TYPE_LIST);
				}
			}
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			int pr = (*staxp);
			fatal(lw_reset, stax, staxa, pr);
			fatal(resolve, list->elements[x], vmap, gp, stax, staxa, staxp, rawmap, rawvars);
			fatal(lstack_obj_add, (*stax)[pn], (*stax)[pr], LISTWISE_TYPE_LIST);
		}
	}

	// apply interpolation context from the FFN_LIST node to the lstack* instance
	if(list->flags & FFN_WSSEP)
		lwx_setflags((*stax)[pn], INTERPOLATE_ADJOIN);
	else if(list->flags & FFN_COMMASEP)
		lwx_setflags((*stax)[pn], INTERPOLATE_DELIM_WS);

	if(list->generator_node)
	{
		fatal(lw_exec, list->generator_node->generator, list->generator_node->text, &(*stax)[pn]);
	}
	else if(list->generator_list_node)
	{
		int pr = (*staxp);
		fatal(lw_reset, stax, staxa, pr);
		fatal(list_resolve, list->generator_list_node, vmap, gp, stax, staxa, staxp, rawmap, 0);

		pstring_wfree(&gps);
		fatal(render, (*stax)[pr], &gps);

		generator_xfree(&g);
		fatal(generator_parse, gp, gps->s, gps->l, &g);

		fatal(lw_exec, g, gps->s, &(*stax)[pn]);
	}
	else
	{
		fatal(lstack_sel_reset, (*stax)[pn]);
	}

finally:
	generator_free(g);
	pstring_free(gps);
coda;
}

///
/// public
///

int list_render(lwx * const ls, pstring ** const ps)
{
	if(!*ps)
		fatal(xmalloc, ps, sizeof(**ps));

	(*ps)->l = 0;

	fatal(render, ls, ps);

	finally : coda;
}

int list_renderto(lwx * const ls, pstring ** const ps)
{
	if(!*ps)
		fatal(xmalloc, ps, sizeof(**ps));

	fatal(render, ls, ps);

	finally : coda;
}

int list_resolveto(ff_node * list, map* vmap, generator_parser* const gp, lwx *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	fatal(lw_ensure, stax, staxa, (*staxp));
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);

	finally : coda;
}

int list_resolve(ff_node * list, map* vmap, generator_parser* const gp, lwx *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	fatal(lw_reset, stax, staxa, (*staxp));
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);

	finally : coda;
}

int list_resolveflat(ff_node * list, map* vmap, generator_parser* const gp, lwx *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	int pn = *staxp;
	fatal(lw_reset, stax, staxa, pn);
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);
	fatal(lw_flatten, (*stax)[pn]);

	finally : coda;
}

int list_resolvetoflat(ff_node * list, map* vmap, generator_parser* const gp, lwx *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	int pn = *staxp;
	fatal(lw_ensure, stax, staxa, pn);
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);
	fatal(lw_flatten, (*stax)[pn]);

	finally : coda;
}
