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

int ensure(lstack *** stax, int * staxa, int staxp)
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

static int resolve(ff_node * list, map* vmap, lstack *** stax, int * staxa, int p, int raw)
{
	// additional lstacks go here
	int pn = p;

	int i;
	int x;
	for(x = 0; x < list->elementsl; x++)
	{
		if(list->elements[x]->type == FFN_WORD)
		{
			fatal(lstack_add, (*stax)[p], list->elements[x]->text, strlen(list->elements[x]->text));
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
			// an undefined variable, or a variable with no definition is equivalent to the empty list
				ITERATE(vls, i, go);
				if(go)
				{
					if(vls->s[0].s[i].type)
					{
						fatal(lstack_obj_add, (*stax)[p], *(void**)vls->s[0].s[i].s, LISTWISE_TYPE_GNLW);
					}
					else
					{
						fatal(lstack_add, (*stax)[p], vls->s[0].s[i].s, vls->s[0].s[i].l);
					}
				}
				ITEREND;
			}
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			fatal(list_resolve, list->elements[x], vmap, stax, staxa, ++pn, raw);

			ITERATE((*stax)[pn], i, go);
			if(go)
			{
				if((*stax)[pn]->s[0].s[i].type)
					fatal(lstack_obj_add, (*stax)[p], *(void**)(*stax)[pn]->s[0].s[i].s, LISTWISE_TYPE_GNLW);
				else
					fatal(lstack_add, (*stax)[p], (*stax)[pn]->s[0].s[i].s, (*stax)[pn]->s[0].s[i].l);
			}
			ITEREND;
		}
	}

	// run the list through listwise
	if(list->generator_node)
	{
		log(L_LWDEBUG, "%s", list->generator_node->text);
		fatal(lstack_exec_internal, list->generator_node->generator, 0, 0, 0, &(*stax)[p], log_would(L_LWDEBUG));
	}
	else
	{
		(*stax)[p]->sel.all = 1;
	}

	finally : coda;
}

///
/// public
///

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
