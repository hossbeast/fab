#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <listwise/lstack.h>
#include <listwise/object.h>

#include "args.h"
#include "ff.h"
#include "gn.h"
#include "gnlw.h"
#include "fml.h"
#include "bp.h"
#include "log.h"
#include "map.h"
#include "var.h"

#include "control.h"

int dep_add(ff_node * ffn, map * vmap, lstack *** stax, int * lstax_l, int * stax_a, int p, gn ** first)
{
	// resolve the left-hand side
	fatal(list_resolve, ffn->statements[x]->needs, vmap, stax, stax_l, stax_a, p);

	if((*stax)[p]->l == 1)	// single-evaluation
	{
		// resolve the right-hand side
		fatal(list_resolve, ffn->statements[x]->feeds, vmap, stax, stax_l, stax_a, p + 1);

		// add edges, which are the cartesian product needs x feeds
		LSTACK_ITERATE((*stax)[p], i, goa);
		if(goa)
		{
			LSTACK_ITERATE((*stax)[p + 1], j, gob);
			if(gob)
			{
				if(!*first)
					fatal(gn_edge_add, ffn->statements[x]->ff_dir, (*stax)[p]->s[0].s[i].s, (*stax)[p + 1]->s[0].s[j].s, first);
				else
					fatal(gn_edge_add, ffn->statements[x]->ff_dir, (*stax)[p]->s[0].s[i].s, (*stax)[p + 1]->s[0].s[j].s, 0);
			}
			LSTACK_ITEREND;
		}
		LSTACK_ITEREND;
	}
	else	// multiple-evaluation
	{
		int x;
		for(x = 0; x < (*stax)[p]->l; x++)
		{
			// use up one list and populate the "<" variable (left-hand-side)
			if(stax_a <= p)
			{
				fatal(xrealloc, &(*stax), sizeof(*(*stax)), p + 1, stax_a);
				stax_a = p + 1;
			}
			if(!(*stax)[p])
				fatal(xmalloc, &(*stax)[p], sizeof(*(*stax)[p]));
			lstack_reset((*stax)[p]);

			int y;
			for(y = 0; y < (*stax)[p]->s[x].l; y++)
			{
int gn_add(char * cwd, char * A, gn ** r)
				gn * r = 0;
				fatal(gn_add, ffn->ff_dir, (*stax)[p]->s[x].s[y], 

				fatal(lstack_obj_add, (*stax)[p]->s[x], def, LISTWISE_TYPE_GNLW);
			}
			fatal(var_set, vmap, "*", (*stax)[p++]);
			star = 1;

			fatal(list_resolve, ffn->statements[x]->feeds, vmap, &(*stax), stax_l, stax_a, p + 1);

			// add edges, which are the cartesian product needs x feeds
			gn * first = 0;
			LSTACK_ITERATE((*stax)[p], i, goa);
			if(goa)
			{
				LSTACK_ITERATE((*stax)[p + 1], j, gob);
				if(gob)
				{
					if(!first)
						fatal(gn_edge_add, ffn->statements[x]->ff_dir, (*stax)[p]->s[0].s[i].s, (*stax)[p + 1]->s[0].s[j].s, &first);
					else
						fatal(gn_edge_add, ffn->statements[x]->ff_dir, (*stax)[p]->s[0].s[i].s, (*stax)[p + 1]->s[0].s[j].s, 0);
				}
				LSTACK_ITEREND;
			}
			LSTACK_ITEREND;
		}
	}

	return 1;
}
