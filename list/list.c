#include <listwise/lstack.h>
#include <listwise/object.h>

// in liblistwise.so
extern int lstack_exec_internal(generator* g, char** init, int* initls, int initl, lstack** ls, int dump);

#include "list.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"

#include "control.h"
#include "map.h"
#include "xmem.h"

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

	// reset the lstack we are using, advance staxp
	lstack_reset((*stax)[staxp]);

	finally : coda;
}

static int render(ff_node * list, map* vmap, lstack *** stax, int * staxa, int p)
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
			var_container * c = 0;
			var_container ** cc = 0;

			char * name = list->elements[x]->name;

			while((cc = map_get(vmap, name, strlen(name))))
			{
				c = (*cc);

				if(c->l == 0)
					break;
				
				if(c->v[c->l - 1].type == VV_LS)
					break;

				name = c->v[c->l - 1].alias;
			}

			lstack * vls = 0;

			if(c && c->l)
				vls = c->v[c->l - 1].ls;
			
			if(vls)
			{
				LSTACK_ITERATE(vls, i, go);
				if(go)
				{
					if(vls->s[0].s[i].type)
						fatal(lstack_obj_add, (*stax)[p], *(void**)vls->s[0].s[i].s, LISTWISE_TYPE_GNLW);
					else
						fatal(lstack_add, (*stax)[p], vls->s[0].s[i].s, vls->s[0].s[i].l);
				}
				LSTACK_ITEREND;
			}
			else
			{
				// an undefined variable, or a variable with no definition - equivalent to the empty list
			}
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			fatal(list_resolve, list->elements[x], vmap, stax, staxa, ++pn);

			LSTACK_ITERATE((*stax)[pn], i, go);
			if(go)
			{
				if((*stax)[pn]->s[0].s[i].type)
					fatal(lstack_obj_add, (*stax)[p], *(void**)(*stax)[pn]->s[0].s[i].s, LISTWISE_TYPE_GNLW);
				else
					fatal(lstack_add, (*stax)[p], (*stax)[pn]->s[0].s[i].s, (*stax)[pn]->s[0].s[i].l);
			}
			LSTACK_ITEREND;
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
	lstack_reset((*stax)[staxp]);
	fatal(ensure, stax, staxa, staxp);

	finally : coda;
}

int list_resolveto(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	fatal(render, list, vmap, stax, staxa, staxp);

	finally : coda;
}

int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	lstack_reset((*stax)[staxp]);
	fatal(render, list, vmap, stax, staxa, staxp);

	finally : coda;
}

int list_empty(lstack *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	lstack_reset((*stax)[staxp]);

	(*stax)[staxp]->sel.all = 1;

	finally : coda;
}
