/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "logs.h"
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
	, transform_parser * const  restrict gp
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
		fatal(map_set, ts[x]->fmlv->ctx->bag, MMS("@"), MM((*stax)[staxp]), 0);
		fatal(fml_render, ts[x], gp, stax, staxa, staxp + 1, 0, 1);
		map_delete(ts[x]->fmlv->ctx->bag, MMS("@"));
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

		// process all dscvs for the node
		int wasgood = 1;
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
							fatal(dep_process, dff->ffn->statements[k], 0, vmap, gp, stax, staxa, staxp, 0, newn, newr);
						}
					}
				}
				else
				{
					wasgood = 0;
				}
			}
			else
			{
				wasgood = 0;
			}
		}

		if(wasgood)
		{
			// discovery succesful
			dscvgn->invalid &= ~GN_INVALIDATION_DISCOVERY;
		}
		else
			bad++;

		// advance to the next group
		for(; x < tsl && ts[x]->fmlv->target == dscvgn; x++);
	}

	if(bad)
		fail(FAB_DSCPARSE);

	finally : coda;
}

//
// public
//

int dsc_exec_specific(gn *** list, int listl, map * vmap, transform_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
{
	int tsl = 0;
	int x;
	int y;

	// assign each threadspace a discovery formula evaluation context
	for(x = 0; x < listl; x++)
	{
		if((*list[x])->type == GN_TYPE_PRIMARY)
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

	logf(L_DSC | L_DSCINFO, "discovery --- executes %3d cached ---", tsl);

	int newn = 0;
	int newr = 0;
	fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, 0);

	logf(L_DSC | L_DSCEXEC, "discovery --- : %3d nodes and %3d edges", newn, newr);

	finally : coda;
}

int dsc_exec_entire(map * vmap, transform_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
{
	static int dsc_instance;

	int x;
	int y;
	
	// increases monotonically with each dsc_exec
	dsc_instance++;

	int i = 0;
	while(1)
	{
		// assign each threadspace a discovery formula evaluation context
		int tsl = 0;
		int n = 0;
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(gn_nodes.e[x]->type == GN_TYPE_PRIMARY && gn_nodes.e[x]->mark != dsc_instance)
			{
				gn_nodes.e[x]->mark = dsc_instance;

				fatal(hashblock_stat, gn_nodes.e[x]->path->can, gn_nodes.e[x]->hb);
				if(gn_nodes.e[x]->hb->stathash[1] == 0)
				{
					gn_nodes.e[x]->invalid |= GN_INVALIDATION_NXFILE;
				}
				else if(hashblock_cmp(gn_nodes.e[x]->hb))
				{
					gn_nodes.e[x]->invalid |= GN_INVALIDATION_CHANGED;
				}

				// require discovery
				if(GN_IS_INVALID(gn_nodes.e[x]) && gn_nodes.e[x]->dscvsl)
				{
					gn_nodes.e[x]->invalid |= GN_INVALIDATION_DISCOVERY;

					for(y = 0; y < gn_nodes.e[x]->dscvsl; y++)
					{
						fatal(ts_ensure, ts, tsa, tsl + 1);
						ts_reset((*ts)[tsl]);

						(*ts)[tsl++]->fmlv = gn_nodes.e[x]->dscvs[y];
					}

					n++;
				}
			}
		}

		if(!n)
			break;

		logf(L_DSC | L_DSCINFO, "discovery %2d for %3d nodes", i, n);

		int newn = 0;
		int newr = 0;
		fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, i);

		// sum discovered objects
		logf(L_DSC | L_DSCEXEC, "discovery %2d for %3d nodes found %3d nodes and %3d edges", i, n, newn, newr);
		i++;
	}

	finally : coda;
}
