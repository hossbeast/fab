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
#include "list.h"
#include "traverse.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"
#include "map.h"

//
// static
//

static int count_dscv(gn * r, int * c)
{
	int logic(gn * gn, int d)
	{
		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			int x;
			for(x = 0; x < gn->dscvsl; x++)
			{
				if(gn->dscvs[x]->dscv_mark == 0)
				{
					(*c)++;
					gn->dscvs[x]->dscv_mark = 1;
				}
			}
		}

		return 1;
	};

	return traverse_depth_bynodes_needsward_useweak(r, logic);
}

static int assign_dscv(gn * r, ts ** ts, int * tsl, gn ** cache, int * cachel)
{
	int logic(gn * gn, int d)
	{
		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			int x;
			for(x = 0; x < gn->dscvsl; x++)
			{
				if(gn->dscvs[x]->dscv_mark == 1)
				{
					// determine if the node has suitable cached discovery results
					fatal(gn_primary_reload_dscv, gn);

					if(gn->dscv_block->block)
					{
						cache[(*cachel)++] = gn;

						// dscv results were loaded from cache so all of the dscvs for this node are skipped
						int y;
						for(y = 0; y < gn->dscvsl; y++)
						{
							gn->dscvs[y]->dscv_mark = 2;
						}
					}
					else
					{
						ts[(*tsl)++]->fmlv = gn->dscvs[x];
						gn->dscvs[x]->dscv_mark = 2;
					}
				}
			}
		}

		finally : coda;
	};

	return traverse_depth_bynodes_needsward_useweak(r, logic);
}

static int depblock_process(const depblock * const db, gn * const dscvgn, int * const newnp, int * const newrp)
{
	int i;
	int k;
	for(i = 0; i < db->block->setsl; i++)
	{
		gn * A = 0;

		int newa = 0;
		fatal(gn_add
			, db->block->sets[i].nbase
			, 0
			, db->block->sets[i].needs
			, 0
			, &A
			, &newa
		);

		if(newnp)
			(*newnp) += newa;

		for(k = 0; k < db->block->sets[i].feedsl; k++)
		{
			void * B = db->block->sets[i].feeds[k];

			int newa = 0;
			int newb = 0;
			int newr = 0;

			fatal(gn_edge_add
				, db->block->sets[i].fbase
				, 0
				, (void*)&A, 0, 1
				, &B, 0, 0
				, 0
				, dscvgn
				, db->block->sets[i].weak
				, 0
				, &newb
				, &newr
			);

			if(newnp)
				(*newnp) += newb;
			if(newrp)
				(*newrp) += newr;

			if(newr)
			{
				log(L_DG | L_DGDEPS | L_DSCNEW, "[%1s][%1s][%1s][%1s](DSC:%s)[%6s%s%6s] %s -> %s"
					, "S"
					, newa ? "x" : ""
					, newb ? "x" : ""
					, newr ? "x" : ""
					, dscvgn->idstring
					, "", "cache", ""
					, ((gn*)A)->idstring
					, ((gn*)B)->idstring
				);
			}
		}
	}

	finally : coda;
}

//
// public
//

int dsc_exec(gn ** roots, int rootsl, map * vmap, generator_parser * const gp, lstack *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw, int * new)
{
	gn ** cache = 0;
	int		cachel = 0;
	int		cachea = 0;

	int		dscvl = 0;
	int		tsl = 0;

	// apply user-specified invalidations, apply gn designations
	gn_invalidations();

	// count not-yet-executed discovery fmleval contexts
	int x;
	for(x = 0; x < rootsl; x++)
		fatal(count_dscv, roots[x], &dscvl);

	int i;
	for(i = 0; dscvl; i++)
	{
		// ensure enough threadspace as if all nodes require re-execution of discovery
		fatal(ts_ensure, ts, tsa, dscvl);

		// ensure enough cache space as if all nodes have cached discovery
		if(dscvl > cachea)
		{
			fatal(xrealloc, &cache, sizeof(*cache), dscvl, 0);
			cachea = dscvl;
		}

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

			// @ is a single element list containing the target of this dscv
			fatal(list_ensure, stax, staxa, staxp);
			fatal(lstack_obj_add, (*stax)[staxp], (*ts)[x]->fmlv->target, LISTWISE_TYPE_GNLW);

			// render the formula
			fatal(map_set, (*ts)[x]->fmlv->bag, MMS("@"), MM((*stax)[staxp]));
			fatal(fml_render, (*ts)[x], gp, stax, staxa, staxp + 1, 1);
			fatal(map_delete, (*ts)[x]->fmlv->bag, MMS("@"));
		}

		// execute all formulas in parallel
		int res = 0;
		fatal(ts_execwave, *ts, tsl, tsw, i, L_DSC | L_DSCEXEC, L_DSC, &res);

		// harvest the results - for a single PRIMARY node, all of its associated dscv
		// evals will have executed in the same wave, AND they will be contiguous in ts
		int newn = 0;
		int newr = 0;

		for(x = 0; x < tsl; )
		{
			// PRIMARY node for this discovery group
			gn * dscvgn = (*ts)[x]->fmlv->target;

			// allocate dependency block for this node
			fatal(depblock_allocate, dscvgn->dscv_block);

			// process all dscvs for the node
			for(; x < tsl && (*ts)[x]->fmlv->target == dscvgn; x++)
			{
				if((*ts)[x]->r_status == 0 && (*ts)[x]->r_signal == 0 && (*ts)[x]->stde_txt->l == 0)
				{
					// parse the generated DDISC fabfile
					ff_file * dff = 0;
					fatal(ff_dsc_parse
						, (*ts)[x]->ffp
						, (*ts)[x]->stdo_txt->s
						, (*ts)[x]->stdo_txt->l
						, (*ts)[x]->stdo_path->s
						, dscvgn
						, &dff
					);

					if(dff)
					{
						// process dependencies, attempt to populate the dependency block
						int k;
						for(k = 0; k < dff->ffn->statementsl; k++)
						{
							if(dff->ffn->statements[k]->type == FFN_DEPENDENCY)
							{
								fatal(dep_process, dff->ffn->statements[k], 0, vmap, gp, stax, staxa, staxp, 0, &newn, &newr, dscvgn->dscv_block);
							}
						}
					}
					else
					{
						break;
					}
				}
			}

			if(x < tsl && (*ts)[x]->fmlv->target == dscvgn)
			{
				// parse failure
				res = 0;
			}
			else
			{
				// all were parsed; rewrite the dependency block to disk, commit the dscv hashblock
				fatal(gn_primary_rewrite_dscv, dscvgn);
			}

			// advance to the next group
			for(; x < tsl && (*ts)[x]->fmlv->target == dscvgn; x++);
		}

		if(!res)
			qfail();

		// process cached results
		for(x = 0; x < cachel; x++)
		{
			log(L_DSC | L_DSCEXEC, "(cache) %-9s %s", gn_designate(cache[x]), cache[x]->idstring);

			fatal(depblock_process, cache[x]->dscv_block, cache[x], &newn, &newr);
			fatal(depblock_close, cache[x]->dscv_block);
		}

		// sum of discovered objects
		if(new)
			(*new) += newn + newr;

		log(L_DSC | L_DSCINFO, "DISCOVERY %d : %d nodes and %d edges", i, newn, newr);

		// apply user-specified invalidations, apply gn designations
		gn_invalidations();

		// recount - new nodes may need discovered
		dscvl = 0;
		for(x = 0; x < rootsl; x++)
			fatal(count_dscv, roots[x], &dscvl);
	}

finally:
	free(cache);
coda;
}
