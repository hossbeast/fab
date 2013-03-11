#include <listwise.h>
#include <listwise/object.h>
#include <listwise/lstack.h>

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

	// iterate the outer list
	LSTACK_ITERREV(lso, x, goo);
	if(goo)
	{
		if(lso->s[0].s[x].type == LISTWISE_TYPE_LIST)
		{
			// get reference to the inner list
			lstack * lsi = *(void**)lso->s[0].s[x].s;

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
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int resolve(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp, int raw)
{
	// additional lstacks go here
	int pn = staxp + 1;

	int x;
	for(x = 0; x < list->elementsl; x++)
	{
		if(list->elements[x]->type == FFN_WORD)
		{
			fatal(lstack_add, (*stax)[staxp], list->elements[x]->text, strlen(list->elements[x]->text));
		}
		else if(list->elements[x]->type == FFN_LF)
		{
			fatal(lstack_add, (*stax)[staxp], "\n", 1);
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
				fatal(lstack_obj_add, (*stax)[staxp], &vls, LISTWISE_TYPE_LIST);
			}
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			fatal(resolve, list->elements[x], vmap, stax, staxa, pn, raw);
			fatal(lstack_obj_add, (*stax)[staxp], &(*stax)[pn++], LISTWISE_TYPE_LIST);
		}
	}

	if(list->generator_node)
	{
		// latten first
		fatal(flatten, (*stax)[staxp]);

		// pass through listwise
		log(L_LWDEBUG, "%s", list->generator_node->text);
		fatal(lstack_exec_internal, list->generator_node->generator, 0, 0, 0, &(*stax)[staxp], log_would(L_LWDEBUG));
	}
	else
	{
		(*stax)[staxp]->sel.all = 1;
	}

	(*stax)[staxp]->flags = list->flags;

	finally : coda;
}

static int render(lstack * const ls, pstring ** const ps)
{
	char * dm = "";
	if(ls->flags & FFN_COMMASEP)
		dm = " ";

	int k = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(k++)
		{
			fatal(pscatf, ps, "%s", dm);
		}

		if(ls->s[0].s[x].type)
		{
			fatal(list_render, *(void**)ls->s[0].s[x].s, ps);
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

///
/// public
///

int list_render(lstack * const ls, pstring ** const ps)
{
	if(ps)
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

int list_resolveto(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp, int raw)
{
	fatal(ensure, stax, staxa, staxp);
	fatal(resolve, list, vmap, stax, staxa, staxp, raw);

	finally : coda;
}

int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp, int raw)
{
	fatal(ensure, stax, staxa, staxp);
	lstack_reset((*stax)[staxp]);
	fatal(resolve, list, vmap, stax, staxa, staxp, raw);

	finally : coda;
}
