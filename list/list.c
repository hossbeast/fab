#include "list.h"

#include <listwise/lstack.h>

#include "control.h"

static int listwise_execute()
{

}

int list_resolve(ff_node * list, lstack *** ls, int * stax_l, int * stax_a, int p)
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
	fatal(lstack_reset, (*ls)[p]);

	// additional lstacks go here
	int pn = p;

	int x;
	for(x = 0; x < list->items_l; x++)
	{
		if(list->items[x]->type == FFN_WORD)
		{
			fatal(lstack_add, ls, list->items[x]->text, 0);
		}
		else if(list->items[x]->type == FFN_VARNAME)
		{
			
		}
		else if(list->items[x]->type == FFN_LIST)
		{
			fatal(list_resolver, list->items[x], ls, stax_l, stax_a, ++pn);

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
		fatal(listwise_exec, 
	}
}

int list_resolve(ff_node * list, lstack *** stax, int * stax_l, int * stax_a)
{
	fatal(list_resolver, list, stax, stax_l, stax_a, 0);
}
