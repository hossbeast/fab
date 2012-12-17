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

static int reset_edges(gn * r)
{
	void logic(relation * rel)
	{
		rel->mark = 0;
	};

	return gn_traverse_relations_needsward(r, logic);
}

static int reset_nodes(gn * r)
{
	void logic(gn * gn)
	{
		gn->mark = 0;
	};

	return gn_traverse_needsward(r, logic);
}

static int newnodes(gn * r, int * c)
{
	void logic(gn * gn)
	{
		if(gn->mark == 0)
			(*c)++;

		gn->mark = 1;
	};

	return gn_traverse_needsward(r, logic);
}

static int newedges(gn * r, int * c)
{
	void logic(relation * rel)
	{
		if(rel->mark == 0)
			(*c)++;

		rel->mark = 1;
	};

	return gn_traverse_relations_needsward(r, logic);
}

static int count_dscv(gn * r, int * c)
{
	void logic(gn * gn)
	{
		if(gn->mark == 0 && gn->dscv)
		{
			(*c)++;

			int x;
			for(x = 0; x < gn->dscv->products_l; x++)
				gn->dscv->products[x]->mark = 1;
		}
	};

	return gn_traverse_needsward(r, logic);
}

static int assign_dscv(gn * r, ts ** ts, int * c)
{
	void logic(gn * gn)
	{
		if(gn->mark == 1)
		{
			ts[(*c)++]->fmlv = gn->dscv;

			int x;
			for(x = 0; x < gn->dscv->products_l; x++)
				gn->dscv->products[x]->mark = 2;
		}
	}

	return gn_traverse_needsward(r, logic);
}

//
// public
//

int dsc_exec(gn ** gn, int gnl, map * vmap, lstack *** stax, int * staxl, int * staxa, int p, ts *** ts, int * tsa, int * tsw)
{
	ff_node * ffn = 0;
	int x;
	int i;
	int k;

	// graph nodes reset
	for(x = 0; x < gnl; x++)
		fatal(reset_nodes, gn[x]);

	// graph edges reset
	for(x = 0; x < gnl; x++)
		fatal(reset_edges, gn[x]);

	// count nodes having not yet participated in discovery
	//  (actually this counts discovery fml contexts)
	int tsl = 0;
	for(x = 0; x < gnl; x++)
		fatal(count_dscv, gn[x], &tsl);

	if(tsl)
	{
		// create new threadspaces as needed
		fatal(ts_ensure, ts, tsa, tsl);

		// assign each threadspace a discovery formula evaluation context
		k = 0;
		for(x = 0; x < gnl; x++)
			fatal(assign_dscv, gn[x], *ts, &k);

		for(i = 0; 1; i++)
		{
			log(L_DSC | L_DSCEXEC, "DISCOVERY %d executes %d", i, tsl);

			// render formulas
			for(x = 0; x < tsl; x++)
			{
				(*ts)[x]->y = x;

				// prepare lstack(s) for variables resident in this context
				int pn = p;
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

				ff_dump(ffn);

				for(k = 0; k < ffn->statements_l; k++)
				{
					if(ffn->statements[k]->type == FFN_DEPENDENCY)
					{
						fatal(dep_process, ffn->statements[k], (*ts)[x]->fmlv->products[0], vmap, stax, staxl, staxa, p, (void*)0, &newn, &newr);
					}
				}
			}

			log(L_DSC | L_DSCEXEC, "%d new nodes", newn);
			log(L_DSC | L_DSCEXEC, "%d new edges", newr);

			// nodes newly discovered
			tsl = 0;
			for(x = 0; x < gnl; x++)
				fatal(count_dscv, gn[x], &tsl);

			// terminate when there are no nodes having not yet participated in discovery
			if(tsl == 0)
				break;

			// assign each threadspace a discovery formula evaluation context
			k = 0;
			for(x = 0; x < gnl; x++)
				fatal(assign_dscv, gn[x], *ts, &k);
		}
	}

	return 1;
}
