#include <stdio.h>

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "ff.h"
#include "gnlw.h"
#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"

//
// static
//

static int newedges(gn * gn)
{
	int c = 0;

	int x;
	for(x = 0; x < gn->needs.l; x++)
	{
		if(gn->needs.e[x]->mark == 0)
		{
			gn->needs.e[x]->mark = 1;
printf("%s %p->needs[%d]=%p : %s(%p) -> %s(%p)\n", gn->needs.e[x]->ffn->loc.ff->path, gn, x, gn->needs.e[x], gn->needs.e[x]->A->path, gn->needs.e[x]->A, gn->needs.e[x]->B->path, gn->needs.e[x]->B);
			c++;
		}

		c += newedges(gn->needs.e[x]->B);
	}

	return c;
}

static void reset(gn * gn)
{
	gn->mark = 0;

	int x;
	for(x = 0; x < gn->needs.l; x++)
	{
		// mark all existing relations
		gn->needs.e[x]->mark = 1;

		reset(gn->needs.e[x]->B);
	}
}

static int count(gn * gn)
{
	int c = 0;

	if(gn->mark == 0)
	{
		int x;
		for(x = 0; x < gn->needs.l; x++)
			c += count(gn->needs.e[x]->B);

		if(gn->dscv)
		{
			c++;
			for(x = 0; x < gn->dscv->products_l; x++)
			{
				gn->dscv->products[x]->mark = 1;
			}
		}

		gn->mark = 1;
	}

	return c;
}

static void assign(gn * gn, ts ** ts, int * c)
{
	if(gn->mark == 1)
	{
		int x;
		for(x = 0; x < gn->needs.l; x++)
			assign(gn->needs.e[x]->B, ts, c);

		if(gn->dscv)
		{
			ts[(*c)++]->fmlv = gn->dscv;

			for(x = 0; x < gn->dscv->products_l; x++)
			{
				gn->dscv->products[x]->mark = 2;
			}
		}

		gn->mark = 2;
	}
}

//
// public
//

int dsc_exec(gn ** gn, int gnl, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p, ts *** ts, int * tsa, int * tsw)
{
	ff_node * ffn = 0;
	int x;
	int i;
	int k;

	// graph reset
	for(x = 0; x < gnl; x++)
		reset(gn[x]);

	// count nodes having not yet participated in discovery
	//  (actually this counts discovery fml contexts)
	int tsl = 0;
	for(x = 0; x < gnl; x++)
		tsl += count(gn[x]);

	if(tsl)
	{
		// create new threadspaces as needed
		fatal(ts_ensure, ts, tsa, tsl);

		// assign each threadspace a discovery formula evaluation context
		k = 0;
		for(x = 0; x < gnl; x++)
			assign(gn[x], *ts, &k);

		for(i = 0; 1; i++)
		{
			log(L_DSC | L_DSCEXEC, "DISCOVERY %d executes %d", i, tsl);

			// render formulas
			for(x = 0; x < tsl; x++)
			{
				(*ts)[x]->y = x;

				// prepare lstack(s) for variables resident in this context
				int pn = p;
				if((*stax_a) <= pn)
				{
					fatal(xrealloc, stax, sizeof(**stax), pn + 1, (*stax_a));
					(*stax_a) = pn + 1;
				}
				if(!(*stax)[pn])
					fatal(xmalloc, &(*stax)[pn], sizeof(*(*stax)[0]));
				lstack_reset((*stax)[pn]);

				// @ is a list of expected products of this eval context
				// for a discovery formula, @ always contains exactly 1 element
				fatal(lstack_obj_add, (*stax)[pn], (*ts)[x]->fmlv->products[0], LISTWISE_TYPE_GNLW);
				fatal(var_set, vmap, "@", (*stax)[pn++]);

				// render the formula
				fatal(fml_render, (*ts)[x], vmap, stax, stax_l, stax_a, pn);
			}

			// execute all formulas in parallel
			fatal(ts_execwave, *ts, x, tsw, L_DSC | L_DSCEXEC, L_DSC);

			// harvest the results
			for(x = 0; x < tsl; x++)
			{
				fatal(ff_dsc_parse
					, (*ts)[x]->ffp
					, (*ts)[x]->stdo_txt->s
					, (*ts)[x]->stdo_txt->l
					, (*ts)[x]->stdo_path->s
					, &ffn
				);

				ff_dump(ffn);

				for(k = 0; k < ffn->statements_l; k++)
				{
					if(ffn->statements[x]->type == FFN_DEPENDENCY)
					{
						fatal(dep_add_bare, (*ts)[x]->fmlv->products[0], ffn->statements[x]);
					}
				}
			}

			// edges newly added
			k = 0;
			for(x = 0; x < gnl; x++)
				k += newedges(gn[x]);

			printf("%d new edges\n", k);

			// nodes newly discovered
			tsl = 0;
			for(x = 0; x < gnl; x++)
				tsl += count(gn[x]);

			// terminate when there are no nodes having not yet participated in discovery
			if(tsl == 0)
				break;

			// assign each threadspace a discovery formula evaluation context
			k = 0;
			for(x = 0; x < gnl; x++)
				assign(gn[x], *ts, &k);
		}
	}

	ff_xfreenode(&ffn);
}
