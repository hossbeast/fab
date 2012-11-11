#include "list.h"

#include <listwise/lstack.h>

#include "list.h"

#include "control.h"
#include "map.h"

int list_resolver(ff_node * list, map* vmap, lstack *** ls, int * stax_l, int * stax_a, int p)
{
	// ensure enough lstacks are allocated
	if((*stax_a) <= p)
	{
		fatal(xrealloc, ls, sizeof(**ls), p + 1, (*stax_a));
		(*stax_a) = p + 1;
	}

	// ensure lstack at this spot is allocated
	if(!(*ls)[p])
		fatal(xmalloc, &(*ls)[p], sizeof(*(*ls)[p]));

	// reset the lstack we are using
	lstack_reset((*ls)[p]);

	// additional lstacks go here
	int pn = p;

	int i;
	int x;
	for(x = 0; x < list->elements_l; x++)
	{
		if(list->elements[x]->type == FFN_WORD)
		{
			fatal(lstack_add, (*ls)[p], list->elements[x]->text, strlen(list->elements[x]->text));
		}
		else if(list->elements[x]->type == FFN_VARNAME)
		{
			lstack ** vls = 0;
			if((vls = map_get(vmap, list->elements[x]->name, strlen(list->elements[x]->name))) == 0)
			{
				fail("reference to undefined variable : '%s'", list->elements[x]->name);
			}

			LSTACK_LOOP_ITER((*vls), i, go);
			if(go)
			{
				fatal(lstack_add, (*ls)[p], (*vls)->s[0].s[i].s, (*vls)->s[0].s[i].l);
			}
			LSTACK_LOOP_DONE;
		}
		else if(list->elements[x]->type == FFN_LIST)
		{
			fatal(list_resolver, list->elements[x], vmap, ls, stax_l, stax_a, ++pn);

			LSTACK_LOOP_ITER((*ls)[pn], i, go);
			if(go)
			{
				fatal(lstack_add, (*ls)[p], (*ls)[pn]->s[0].s[i].s, (*ls)[pn]->s[0].s[i].l);
			}
			LSTACK_LOOP_DONE;
		}
	}

	// run the list through listwise
	if(list->generator_node)
	{
		fatal(lstack_exec, list->generator_node->generator, 0, 0, 0, &(*ls)[p]);
	}

	return 1;
}

int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * stax_l, int * stax_a)
{
	return list_resolver(list, vmap, stax, stax_l, stax_a, 0);
}
