/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>

#include "listwise.h"

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
#include "lwutil.h"
#include "traverse.h"

#include "log.h"
#include "global.h"
#include "xlinux.h"
#include "macros.h"
#include "map.h"

#define restrict __restrict

//
// static
//

static int dsc_execwave(
	  map * vmap
	, generator_parser * const  restrict gp
	, lwx *** restrict stax
	, int * restrict staxa
	, int staxp
	, ts ** restrict ts
	, int tsl
	, int * restrict tsw
	, int * restrict newn
	, int * restrict newr
	, int i
)
{
	int x;

	// render formulas
	for(x = 0; x < tsl; x++)
	{
		ts[x]->y = x;

		// @ is a single element list containing the target of this dscv
		fatal(lw_reset, stax, staxa, staxp);
		fatal(lstack_obj_add, (*stax)[staxp], ts[x]->fmlv->target, LISTWISE_TYPE_GNLW);

		// render the formula
		fatal(map_set, ts[x]->fmlv->bag, MMS("@"), MM((*stax)[staxp]), 0);
		fatal(fml_render, ts[x], gp, stax, staxa, staxp + 1, 0, 1);
		map_delete(ts[x]->fmlv->bag, MMS("@"));
	}

	// execute all formulas in parallel
	fatal(ts_execwave, ts, tsl, tsw, i, L_DSC | L_DSCEXEC, L_DSC, 0);

	// harvest the results - for a single PRIMARY node, all of its associated dscv
	// evals will have executed in the same wave, AND they will be contiguous in ts
	int bad = 0;
	for(x = 0; x < tsl;)
	{
		// PRIMARY node for this discovery group
		gn * dscvgn = ts[x]->fmlv->target;

		// allocate dependency block for this node
		fatal(depblock_allocate, dscvgn->dscv_block);

		// process all dscvs for the node
		int reconcile = 1;
		for(; x < tsl && ts[x]->fmlv->target == dscvgn; x++)
		{
			if(ts[x]->pid && ts[x]->r_status == 0 && ts[x]->r_signal == 0 && ts[x]->stde_txt->l == 0)
			{
				// parse the generated DDISC fabfile
				ff_file * dff = 0;
				fatal(ff_dsc_parse
					, ts[x]->ffp
					, ts[x]->stdo_txt->s
					, ts[x]->stdo_txt->l
					, ts[x]->stdo_path->s
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
							fatal(dep_process, dff->ffn->statements[k], 0, vmap, gp, stax, staxa, staxp, 0, newn, newr, dscvgn->dscv_block);
						}
					}
				}
				else
				{
					reconcile = 0;
				}
			}
			else
			{
				reconcile = 0;
			}
		}

		if(reconcile)
			fatal(gn_reconcile_dsc, dscvgn);
		else
			bad++;

		// advance to the next group
		for(; x < tsl && ts[x]->fmlv->target == dscvgn; x++);
	}

	if(bad)
		fail(FAB_DSCPARSE, 0);

	finally : coda;
}

static int depblock_process(
	  const depblock * const restrict db
	, gn * const restrict dscvgn
	, int * const restrict newnp
	, int * const restrict newrp
)
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
				, db->block->sets[i].bridge
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
				log(L_DG | L_DGDEPS | L_DSC | L_DSCNEW, "[%1s][%1s][%1s][%1s](DSC:%s)[%6s%s%6s] %s -> %s"
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

static int count_dscv(int use_invalid)
{
	int c = 0;
	int x;
	for(x = 0; x < gn_nodes.l; x++)
	{
		int y;
		for(y = 0; y < gn_nodes.e[x]->dscvsl; y++)
		{
			if(gn_nodes.e[x]->dscvs[y]->dscv_mark == 0 || (use_invalid && GN_IS_INVALID(gn_nodes.e[x])))
			{
				c++;
				gn_nodes.e[x]->dscvs[y]->dscv_mark = 1;
			}
		}
	}

	return c;
}

//
// public
//

int dsc_exec_specific(gn *** list, int listl, map * vmap, generator_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
{
	int tsl = 0;
	int x;
	int y;

	// assign each threadspace a discovery formula evaluation context
	for(x = 0; x < listl; x++)
	{
		if((*list[x])->designate == GN_DESIGNATION_PRIMARY)
		{
			for(y = 0; y < (*list[x])->dscvsl; y++)
			{
				fatal(ts_ensure, ts, tsa, tsl + 1);
				ts_reset((*ts)[tsl]);

				(*ts)[tsl++]->fmlv = (*list[x])->dscvs[y];
			}
		}
		else
		{
			// warn
		}
	}

	log(L_DSC | L_DSCINFO, "DISCOVERY --- executes %3d cached ---", tsl);

	int newn = 0;
	int newr = 0;
	fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, 0);

	log(L_DSC | L_DSCEXEC, "DISCOVERY --- : %3d nodes and %3d edges", newn, newr);

	finally : coda;
}

int dsc_exec_entire(map * vmap, generator_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
{
	gn ** cache = 0;
	int		cachel = 0;
	int		cachea = 0;

	int		dscvl = 0;
	int		tsl = 0;

	int x;
	int i;
	int y;

	// count not-yet-executed discovery fmleval contexts
	dscvl = count_dscv(1);

	for(i = 0; dscvl; i++)
	{
		// ensure enough cache space as if all nodes have cached discovery
		if(dscvl > cachea)
		{
			fatal(xrealloc, &cache, sizeof(*cache), dscvl, 0);
			cachea = dscvl;
		}

		// assign each threadspace a discovery formula evaluation context
		tsl = 0;
		cachel = 0;
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(gn_nodes.e[x]->designate == GN_DESIGNATION_PRIMARY)
			{
				for(y = 0; y < gn_nodes.e[x]->dscvsl; y++)
				{
					if(gn_nodes.e[x]->dscvs[y]->dscv_mark == 1)
					{
						if(gn_nodes.e[x]->dscv_block->block == 0)
						{
							if(gn_nodes.e[x]->primary_hb->stathash[1] == 0)
							{
								log(L_ERROR, "%-9s file %s not found", "PRIMARY", gn_nodes.e[x]->idstring);
								break;
							}
						}
					}
				}
 
				if(y == gn_nodes.e[x]->dscvsl)
				{
					for(y = 0; y < gn_nodes.e[x]->dscvsl; y++)
					{
						if(gn_nodes.e[x]->dscvs[y]->dscv_mark == 1)
						{
							if(gn_nodes.e[x]->dscv_block->block)
							{
								cache[cachel++] = gn_nodes.e[x];

								// dscv results were loaded from cache so all of the dscvs for this node are skipped
								int k;
								for(k = 0; k < gn_nodes.e[x]->dscvsl; k++)
								{
									gn_nodes.e[x]->dscvs[k]->dscv_mark = 2;
								}

								break;
							}
							else
							{
								fatal(ts_ensure, ts, tsa, tsl + 1);
								ts_reset((*ts)[tsl]);

								(*ts)[tsl++]->fmlv = gn_nodes.e[x]->dscvs[y];
								gn_nodes.e[x]->dscvs[y]->dscv_mark = 2;
							}
						}
					}
				}
			}
		}

		log(L_DSC | L_DSCINFO, "DISCOVERY %3d executes %3d cached %3d", i, tsl, cachel);

		int newn = 0;
		int newr = 0;
		fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, i);

		// process cached results
		for(x = 0; x < cachel; x++)
		{
			log(L_DSC | L_DSCEXEC, "(cache) %-9s %s", gn_designation(cache[x]), cache[x]->idstring);

			fatal(depblock_process, cache[x]->dscv_block, cache[x], &newn, &newr);
			fatal(depblock_close, cache[x]->dscv_block);
		}

		// sum discovered objects
		log(L_DSC | L_DSCEXEC, "DISCOVERY %3d : %3d nodes and %3d edges", i, newn, newr);

		// recount - new nodes may need discovered
		dscvl = count_dscv(0);
	}

finally:
	free(cache);
coda;
}
