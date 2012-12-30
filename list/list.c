#include <listwise/lstack.h>
#include <listwise/object.h>

// in liblistwise.so
extern int lstack_exec_internal(generator* g, char** init, int* initls, int initl, lstack** ls, int dump);

#include "list.h"
#include "gnlw.h"
#include "args.h"

#include "control.h"
#include "map.h"
#include "xmem.h"

int list_ensure(lstack *** stax, int * staxl, int * staxa, int p)
{
	// ensure enough lstacks are allocated
	if((*staxa) <= p)
	{
		fatal(xrealloc, stax, sizeof(**stax), p + 1, (*staxa));
		(*staxa) = p + 1;
	}

	// ensure lstack at this spot is allocated
	if(!(*stax)[p])
		fatal(xmalloc, &(*stax)[p], sizeof(*(*stax)[p]));

	// reset the lstack we are using
	lstack_reset((*stax)[p]);

	return 1;
}

int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * staxl, int * staxa, int p)
{
	// ensure stax allocation, reset p'th stack
	fatal(list_ensure, stax, staxl, staxa, p);

	// additional lstacks go here
	int pn = p;

	int i;
	int x;
	for(x = 0; x < list->elements_l; x++)
	{
		if(list->elements[x]->type == FFN_WORD)
		{
			fatal(lstack_add, (*stax)[p], list->elements[x]->text, strlen(list->elements[x]->text));
		}
		else if(list->elements[x]->type == FFN_VARNAME)
		{
			lstack ** vls = 0;
			if((vls = map_get(vmap, list->elements[x]->name, strlen(list->elements[x]->name))) == 0)
			{
				fail("reference to undefined variable : '%s' @ [%3d,%3d - %3d,%3d]"
					, list->elements[x]->name
					, list->loc.f_lin + 1
					, list->loc.f_col + 1
					, list->loc.l_lin + 1
					, list->loc.l_col + 1
				);
			}

			LSTACK_ITERATE((*vls), i, go);
			if(go)
			{
				if((*vls)->s[0].s[i].type)
					fatal(lstack_obj_add, (*stax)[p], *(void**)(*vls)->s[0].s[i].s, LISTWISE_TYPE_GNLW);
				else
					fatal(lstack_add, (*stax)[p], (*vls)->s[0].s[i].s, (*vls)->s[0].s[i].l);
			}
			LSTACK_ITEREND;
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			fatal(list_resolve, list->elements[x], vmap, stax, staxl, staxa, ++pn);

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

	return 1;
}
