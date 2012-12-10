#include <stdio.h>

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "gnlw.h"
#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"

//
// static
//

static void reset(gn * gn)
{
	gn->mark = 0;

	int x;
	for(x = 0; x < gn->needs.l; x++)
		reset(gn->needs.e[x].gn);
}

static int count(gn * gn)
{
	int c = 0;

	if(gn->mark == 0)
	{
		int x;
		for(x = 0; x < gn->needs.l; x++)
			c += count(gn->needs.e[x].gn);

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
			assign(gn->needs.e[x].gn, ts, c);

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
	int	tsl = 0;

	int x;
	int i;
	int k;

	// graph reset
	for(x = 0; x < gnl; x++)
		reset(gn[x]);

	for(i = 0; 1; i++)
	{
		// count nodes having not yet participated in discovery
		//  (actually this counts discovery fml contexts)
		tsl = 0;
		for(x = 0; x < gnl; x++)
			tsl += count(gn[x]);

		// create new threadspaces as needed
		fatal(ts_ensure, ts, tsa, tsl);

		// assign each threadspace a dscv
		k = 0;
		for(x = 0; x < gnl; x++)
			assign(gn[x], *ts, &k);

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
			int k;
			for(k = 0; k < (*ts)[x]->fmlv->products_l; k++)
				fatal(lstack_obj_add, (*stax)[pn], (*ts)[x]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			fatal(var_set, vmap, "@", (*stax)[pn++]);

			// render the formula
			fatal(fml_render, (*ts)[x], vmap, stax, stax_l, stax_a, pn);
		}

		// execute all formulas in parallel
		fatal(ts_execwave, *ts, x, tsw, L_DSC | L_DSCEXEC, L_DSC);

		// harvest the results
		for(x = 0; x < tsl; x++)
		{
//			printf("%.*s\n", (int)(*ts)[x]->stdo_txt->l, (*ts)[x]->stdo_txt->s);
		}

		break;
	}
}
