#include "list.h"

#include <listwise/lstack.h>

#include "control.h"

static int listwise_execute()
{

}

static int listwise_render(ff_node * list, generator_parser ** p, lstack ** ls)
{
	if(!*ls)
		fatal(xmalloc, ls, sizeof(**ls));

	fatal(lstack_reset, *ls);

	int x;
	for(x = 0; x < list->items_l; x++)
	{
		if(ls->items[x]->type == FFN_WORD)
		{
			fatal(lstack_add, ls, ls->items[x]->text, 0);
		}
		else if(ls->items[x]->type == FFN_VARNAME)
		{
			
		}
		else if(ls->items[x]->type == FFN_LIST)
		{
			
		}
	}

	if(!*p)
		fatal(generator_mkparser, p);

	if(!list->generator_node->generator)
		fatal(generator_parse, *p, list->generator_node->text, 0, &list->generator_node->generator);
}

int list_render(ff_node * list, generator_parser ** p, lstack *** ls, int * ls_l, int * ls_a)
{
	if(list->generator_node)
	{
		fatal(execute_listwise, list, p, ls);
	}
	else
	{

	}

	return 1;
}
