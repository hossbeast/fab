#include <stdio.h>

#include <listwise.h>
#include <listwise/lstack.h>

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "ff.h"
#include "gn.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"
#include "ts.h"
#include "dep.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"

//
// static
//

static int count_dscv(gn * r, int prune, int * c)
{
	void logic(gn * gn, int d)
	{
//printf("%s mark=%d dscv=%p prune=%d changed=%d rebuild=%d\n", gn_idstring(gn), gn->dscv_mark, gn->dscv, prune, gn->changed, gn->rebuild);
		if(gn->dscv_mark == 0 && gn->dscv)
		{
			if(!prune || gn->changed || gn->rebuild)
			{
				(*c)++;

				int x;
				for(x = 0; x < gn->dscv->products_l; x++)
					gn->dscv->products[x]->dscv_mark = 1;
			}
		}
	};

	return gn_depth_traversal_nodes_needsward(r, logic);
}

static int assign_dscv(gn * r, ts ** ts, int prune, int * c)
{
	void logic(gn * gn, int d)
	{
		if(gn->dscv_mark == 1)
		{
			ts[(*c)++]->fmlv = gn->dscv;

			int x;
			for(x = 0; x < gn->dscv->products_l; x++)
				gn->dscv->products[x]->dscv_mark = 2;
		}
	}

	return gn_depth_traversal_nodes_needsward(r, logic);
}

//
// public
//

int dsc_exec(gn ** roots, int rootsl, int prune, map * vmap, lstack *** stax, int * staxl, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw, int * new)
{
	ff_node * ffn = 0;
	int x;
	int i;
	int k;

	// count nodes having not yet participated in discovery
	//  (actually this counts discovery fml contexts)
	int tsl = 0;
	for(x = 0; x < rootsl; x++)
		fatal(count_dscv, roots[x], prune, &tsl);

	for(i = 0; tsl; i++)
	{
		fatal(ts_ensure, ts, tsa, tsl);

		// assign each threadspace a discovery formula evaluation context
		k = 0;
		for(x = 0; x < rootsl; x++)
			fatal(assign_dscv, roots[x], *ts, prune, &k);

		log(L_DSC | L_DSCEXEC, "DISCOVERY %d executes %d", i, tsl);

		// render formulas
		for(x = 0; x < tsl; x++)
		{
			(*ts)[x]->y = x;

			// prepare lstack(s) for variables resident in this context
			int pn = staxp;
			if((*staxa) <= pn)
			{
				fatal(xrealloc, stax, sizeof(**stax), pn + 1, (*staxa));
				(*staxa) = pn + 1;
			}
			if(!(*stax)[pn])
				fatal(xmalloc, &(*stax)[pn], sizeof(*(*stax)[0]));
			lstack_reset((*stax)[pn]);

			// @ is a list of expected products of this eval context
			// for a discovery formula, @ always contains exactly 1 element
			fatal(lstack_obj_add, (*stax)[pn], (*ts)[x]->fmlv->products[0], LISTWISE_TYPE_GNLW);
			fatal(var_set, vmap, "@", (*stax)[pn++]);

			// render the formula
			fatal(fml_render, (*ts)[x], vmap, stax, staxl, staxa, pn);
		}

		// execute all formulas in parallel
		fatal(ts_execwave, *ts, x, tsw, L_DSC | L_DSCEXEC, L_DSC);

		// harvest the results
		int newn = 0;
		int newr = 0;

		for(x = 0; x < tsl; x++)
		{
			fatal(ff_dsc_parse
				, (*ts)[x]->ffp
				, (*ts)[x]->stdo_txt->s
				, (*ts)[x]->stdo_txt->l
				, (*ts)[x]->stdo_path->s
				, (*ts)[x]->fmlv->products[0]
				, &ffn
			);

			// dump, pending logging
			ff_dump(ffn);

			// process dependencies
			for(k = 0; k < ffn->statements_l; k++)
			{
				if(ffn->statements[k]->type == FFN_DEPENDENCY)
				{
					fatal(dep_process, ffn->statements[k], (*ts)[x]->fmlv->products[0], vmap, stax, staxl, staxa, staxp, 0, &newn, &newr);
				}
			}
		}

		// sum of discovered objects
		if(new)
			(*new) += newn + newr;

		log(L_DSC | L_DSCEXEC, "discovered %d nodes and %d edges", newn, newr);

		// recount - new nodes may need discovered
		tsl = 0;
		for(x = 0; x < rootsl; x++)
			fatal(count_dscv, roots[x], prune, &tsl);
	}

	return 1;
}
