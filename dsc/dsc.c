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

static int count_dscv(gn * r, int * c)
{
	int logic(gn * gn, int d)
	{
		if(gn->dscv_mark == 0 && gn->dscv)
		{
			(*c)++;

			int x;
			for(x = 0; x < gn->dscv->products_l; x++)
				gn->dscv->products[x]->dscv_mark = 1;
		}

		return 1;
	};

	return gn_depth_traversal_nodes_needsward(r, logic);
}

static int assign_dscv(gn * r, ts ** ts, int * tsl, gn ** cache, int * cachel)
{
	int logic(gn * gn, int d)
	{
		if(gn->dscv_mark == 1)
		{
			// determine if the node has suitable cached discovery results
			fatal(gn_primary_reload_dscv, gn);

			if(gn->dscv_block->block)
			{
				cache[(*cachel)++] = gn;
			}
			else
			{
				ts[(*tsl)++]->fmlv = gn->dscv;
			}

			int x;
			for(x = 0; x < gn->dscv->products_l; x++)
				gn->dscv->products[x]->dscv_mark = 2;
		}

		finally : coda;
	};

	return gn_depth_traversal_nodes_needsward(r, logic);
}

//
// public
//

int dsc_exec(gn ** roots, int rootsl, map * vmap, lstack *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw, int * new)
{
	ff_node * ffn = 0;
	int x;
	int i;
	int k;

	gn ** cache = 0;
	int		cachel = 0;
	int		cachea = 0;

	int		dscvl = 0;

	int		tsl = 0;

	// count nodes having not yet participated in discovery
	//  (actually this counts discovery fml contexts)
	for(x = 0; x < rootsl; x++)
		fatal(count_dscv, roots[x], &dscvl);

	for(i = 0; dscvl; i++)
	{
		// ensure enough threadspace if all nodes require re-execution of discovery
		fatal(ts_ensure, ts, tsa, dscvl);

		// ensure enough cache space if all nodes have cached discovery
		if(dscvl > cachea)
		{
			fatal(xrealloc, &cache, sizeof(*cache), dscvl, 0);
			cachea = dscvl;
		}

		gn_invalidations();

		// assign each threadspace a discovery formula evaluation context
		tsl = 0;
		cachel = 0;
		for(x = 0; x < rootsl; x++)
			fatal(assign_dscv, roots[x], *ts, &tsl, cache, &cachel);

		log(L_DSC | L_DSCEXEC, "DISCOVERY %d executes %d cached %d", i, tsl, cachel);

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
				fatal(lstack_create, &(*stax)[pn]);
			lstack_reset((*stax)[pn]);

			// @ is a list of expected products of this eval context
			// for a discovery formula, @ always contains exactly 1 element
			fatal(lstack_obj_add, (*stax)[pn], (*ts)[x]->fmlv->products[0], LISTWISE_TYPE_GNLW);
			fatal(var_set, vmap, "@", (*stax)[pn++], 0);

			// render the formula
			fatal(fml_render, (*ts)[x], vmap, stax, staxa, pn);
		}

		// execute all formulas in parallel
		int res = 0;
		fatal(ts_execwave, *ts, x, tsw, i, L_DSC | L_DSCEXEC, L_DSC, &res);

		if(!res)
			qfail();

		// harvest the results
		int newn = 0;
		int newr = 0;

		for(x = 0; x < tsl; x++)
		{
			// PRIMARY node for this discovery
			gn * dscvgn = (*ts)[x]->fmlv->products[0];

			fatal(ff_dsc_parse
				, (*ts)[x]->ffp
				, (*ts)[x]->stdo_txt->s
				, (*ts)[x]->stdo_txt->l
				, (*ts)[x]->stdo_path->s
				, dscvgn
				, &ffn
			);

			if(!ffn)
				qfail();

			// dump, pending logging
			ff_dump(ffn);

			// allocate the dependency block
			fatal(depblock_allocate, dscvgn->dscv_block);

			// process dependencies, attempt to populate the dependency block
			for(k = 0; k < ffn->statements_l; k++)
			{
				if(ffn->statements[k]->type == FFN_DEPENDENCY)
				{
					fatal(dep_process, ffn->statements[k], vmap, stax, staxa, staxp, 0, &newn, &newr, dscvgn->dscv_block);
				}
			}
		}

		// process cached results
		for(x = 0; x < cachel; x++)
		{
			log(L_DSC | L_DSCEXEC, "(cache) %-9s %s", gn_designate(cache[x]), cache[x]->idstring);

			fatal(depblock_process, cache[x], cache[x]->dscv_block, &newn, &newr);
			fatal(depblock_close, cache[x]->dscv_block);
		}

		// sum of discovered objects
		if(new)
			(*new) += newn + newr;

		log(L_DSC | L_DSCINFO, "DISCOVERY %d : %d nodes and %d edges", i, newn, newr);

		// recount - new nodes may need discovered
		dscvl = 0;
		for(x = 0; x < rootsl; x++)
			fatal(count_dscv, roots[x], &dscvl);
	}

finally:
	free(cache);
coda;
}
