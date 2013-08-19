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

#include <listwise.h>
#include <listwise/object.h>
#include <listwise/lstack.h>
#include <listwise/generator.h>

#include "list.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"
#include "lwutil.h"

#include "fab_control.h"
#include "map.h"
#include "xmem.h"
#include "macros.h"

///
/// static
///

static int render(lstack * const ls, pstring ** const ps)
{
	char * dm = " ";
	if(ls->flags == INTERPOLATE_ADJOIN)
		dm = "";
	else if(ls->flags == INTERPOLATE_DELIM_CUST)
		dm = ls->ptr;

	int k = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(k++)
		{
			fatal(pscatf, ps, "%s", dm);
		}

		if(ls->s[0].s[x].type == LISTWISE_TYPE_LIST)
		{
			fatal(render, *(void**)ls->s[0].s[x].s, ps);
		}
		else if(ls->s[0].s[x].type == LISTWISE_TYPE_GNLW)
		{
			gn * A = *(void**)ls->s[0].s[x].s;

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
			char * s;
			int l;
			fatal(lstack_string, ls, 0, x, &s, &l);
			fatal(pscat, ps, s, l);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int resolve(ff_node * list, map* vmap, generator_parser * const gp, lstack *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
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
			lstack * vls = 0;
			if(rawmap)
			{
				lstack ** cc = 0;
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
		(*stax)[pn]->flags = INTERPOLATE_ADJOIN;
	else if(list->flags & FFN_COMMASEP)
		(*stax)[pn]->flags = INTERPOLATE_DELIM_WS;

	if(list->generator_node)
	{
		fatal(lw_exec, list->generator_node->generator, list->generator_node->text, &(*stax)[pn]);
	}
	else if(list->generator_list_node)
	{
		int pr = (*staxp);
		fatal(lw_reset, stax, staxa, pr);
		fatal(list_resolve, list->generator_list_node, vmap, gp, stax, staxa, staxp, rawmap, 0);

		pstring_xfree(&gps);
		fatal(render, (*stax)[pr], &gps);

		generator_xfree(&g);
		if(generator_parse(gp, gps->s, gps->l, &g) != 0)
		{
			fail("failed to parse '%.*s'", gps->l, gps->s);
		}

		fatal(lw_exec, g, gps->s, &(*stax)[pn]);
	}
	else
	{
		(*stax)[pn]->sel.all = 1;
	}

finally:
	generator_free(g);
	pstring_free(gps);
coda;
}

///
/// public
///

int list_render(lstack * const ls, pstring ** const ps)
{
	if(!*ps && xmalloc(ps, sizeof(**ps)) != 0)
		return 1;
		
	(*ps)->l = 0;

	return render(ls, ps);
}

int list_renderto(lstack * const ls, pstring ** const ps)
{
	if(!*ps && xmalloc(ps, sizeof(**ps)) != 0)
		return 1;
		
	return render(ls, ps);
}

int list_resolveto(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	fatal(lw_ensure, stax, staxa, (*staxp));
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);

	finally : coda;
}

int list_resolve(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	fatal(lw_reset, stax, staxa, (*staxp));
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);

	finally : coda;
}

int list_resolveflat(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	int pn = *staxp;
	fatal(lw_reset, stax, staxa, pn);
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);
	fatal(lw_flatten, (*stax)[pn]);

	finally : coda;
}

int list_resolvetoflat(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int rawmap, map * rawvars)
{
	int pn = *staxp;
	fatal(lw_ensure, stax, staxa, pn);
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, rawmap, rawvars);
	fatal(lw_flatten, (*stax)[pn]);

	finally : coda;
}
