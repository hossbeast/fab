#include <listwise.h>
#include <listwise/object.h>
#include <listwise/lstack.h>
#include <listwise/generator.h>

// in liblistwise.so
extern int lstack_exec_internal(generator* g, char** init, int* initls, int initl, lstack** ls, int dump);

#include "list.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"

#include "control.h"
#include "map.h"
#include "xmem.h"
#include "macros.h"

///
/// static
///

static int ensure(lstack *** stax, int * staxa, int staxp)
{
	// ensure enough lstacks are allocated
	if((*staxa) <= staxp)
	{
		int ns = (*staxa) ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, stax, sizeof(**stax), ns, (*staxa));
		(*staxa) = ns;
	}

	// ensure lstack at this spot is allocated
	if(!(*stax)[staxp])
		fatal(lstack_create, &(*stax)[staxp]);

	finally : coda;
}

static int flatten(lstack * lso)
{
	int x;
	int c = 0;

	lstack * lsi = 0;

	// iterate the outer list
	LSTACK_ITERREV(lso, x, goo);
	if(goo)
	{
		if(lso->s[0].s[x].type == LISTWISE_TYPE_LIST)
		{
			// get reference to the inner list
			lsi = *(void**)lso->s[0].s[x].s;

			// flatten the inner list
			fatal(flatten, lsi);

			// delete the inner list from the outer list
			fatal(lstack_delete, lso, 0, x);

			int rl = LSTACK_COUNT(lsi);

			// displace enough entries to insert the inner list
			fatal(lstack_displace, lso, 0, x, rl);

			int i = 0;
			int y;
			LSTACK_ITERATE(lsi, y, goi)
			if(goi)
			{
				if(lsi->s[0].s[y].type)
				{
					fatal(lstack_obj_write_alt, lso, 0, x + i, *(void**)lsi->s[0].s[y].s, lsi->s[0].s[y].type);
				}
				else
				{
					fatal(lstack_write_alt, lso, 0, x + i, lsi->s[0].s[y].s, lsi->s[0].s[y].l);
				}
				i++;
			}
			LSTACK_ITEREND;
		}

		c++;
	}
	LSTACK_ITEREND;

	// as a special case, a list with a single entry which is itself a list inherits
	// the interpolation context of the inner list
	if(c == 1 && lsi)
		lso->flags = lsi->flags;

	finally : coda;
}

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
			fatal(pscatf, ps, "%.*s", A->path->rell, A->path->rel);
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

static int resolve(ff_node * list, map* vmap, generator_parser * const gp, lstack *** stax, int * staxa, int * staxp, int raw)
{
	generator * g = 0;
	pstring * gps = 0;

	// resolved lstack goes here
	int pn = (*staxp)++;

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
			if(raw)
			{
				lstack ** cc = 0;
				if((cc = map_get(vmap, MMS(list->elements[x]->name))))
					vls = *cc;
			}
			else
			{
				vls = var_access(vmap, list->elements[x]->name);
			}

			if(vls)
			{
				fatal(lstack_obj_add, (*stax)[pn], vls, LISTWISE_TYPE_LIST);
			}
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			int pr = (*staxp);
			fatal(ensure, stax, staxa, pr);
			lstack_reset((*stax)[pr]);
			fatal(resolve, list->elements[x], vmap, gp, stax, staxa, staxp, raw);
			fatal(lstack_obj_add, (*stax)[pn], (*stax)[pr], LISTWISE_TYPE_LIST);
		}
	}

	// apply interpolation context from the FFN_LIST node to the lstack* instance
	if(list->flags & FFN_WSSEP)
		(*stax)[pn]->flags = INTERPOLATE_ADJOIN;
	else if(list->flags & FFN_COMMASEP)
		(*stax)[pn]->flags = INTERPOLATE_DELIM_WS;

	if(list->generator_node || list->generator_list_node)
	{
		// flatten first
		fatal(flatten, (*stax)[pn]);

		generator * gen = 0;
		char * tex = 0;

		if(list->generator_node)
		{
			gen = list->generator_node->generator;
			tex = list->generator_node->text;
		}
		else if(list->generator_list_node)
		{
			int pr = (*staxp);
			fatal(ensure, stax, staxa, pr);
			lstack_reset((*stax)[pr]);
			fatal(list_resolve, list->generator_list_node, vmap, gp, stax, staxa, staxp, raw);

			pstring_xfree(&gps);
			fatal(render, (*stax)[pr], &gps);
			tex = gps->s;

			generator_xfree(&g);
			if(generator_parse(gp, gps->s, gps->l, &g) == 0)
			{
				fail("failed to parse '%.*s'", gps->l, gps->s);
			}
			gen = g;
		}

		// pass through listwise
		log(L_LWDEBUG, "%s", tex);
		fatal(lstack_exec_internal, gen, 0, 0, 0, &(*stax)[pn], log_would(L_LWDEBUG));
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
	if(*ps)
		(*ps)->l = 0;

	return render(ls, ps);
}

int list_renderto(lstack * const ls, pstring ** const ps)
{
	return render(ls, ps);
}

int list_flatten(lstack * ls)
{
	fatal(flatten, ls);

	finally : coda;
}

int list_ensure(lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	lstack_reset((*stax)[staxp]);

	finally : coda;
}

int list_resolveto(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int raw)
{
	fatal(ensure, stax, staxa, (*staxp));
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, raw);

	finally : coda;
}

int list_resolve(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int * staxp, int raw)
{
	fatal(ensure, stax, staxa, (*staxp));
	lstack_reset((*stax)[(*staxp)]);
	fatal(resolve, list, vmap, gp, stax, staxa, staxp, raw);

	finally : coda;
}

int list_resolveflat(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	lstack_reset((*stax)[staxp]);

	int pn = staxp;
	fatal(resolve, list, vmap, gp, stax, staxa, &pn, 0);
	fatal(flatten, (*stax)[staxp]);

	finally : coda;
}

int list_resolvetoflat(ff_node * list, map* vmap, generator_parser* const gp, lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);

	int pn = staxp;
	fatal(resolve, list, vmap, gp, stax, staxa, &pn, 0);
	fatal(flatten, (*stax)[staxp]);

	finally : coda;
}
