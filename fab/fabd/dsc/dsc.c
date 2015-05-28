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

#include "xlinux.h"
#include "listwise.h"
#include "listwise/transform.h"

#include "global.h"
#include "params.h"
#include "dsc.h"
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

#include "macros.h"
#include "map.h"
#include "unitstring.h"

#define restrict __restrict

//
// static
//

static int dsc_execwave(
	  map * vmap
	, transform_parser * const  restrict gp
	, lwx *** restrict stax
	, size_t * restrict staxa
	, size_t staxp
	, ts ** restrict ts
	, size_t tsl
	, size_t * restrict tsw
	, size_t * restrict newn
	, size_t * restrict newr
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
	fatal(ts_execwave, ts, tsl, tsw, i, L_DSCEXEC, 0);

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

int dsc_exec_specific(gn ** list, size_t listl, map * vmap, transform_parser * const gp, lwx *** stax, size_t * staxa, size_t staxp, ts *** ts, size_t * tsa, size_t * tsw)
{
	struct timespec	time_start;
	struct timespec	time_end;
	char space[64];

	int tsl = 0;
	int n = 0;
	int x;
	int y;

	// start measuring
	fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);
	
	// assign each threadspace a discovery formula evaluation context
	for(x = 0; x < listl; x++)
	{
		if(list[x]->type == GN_TYPE_PRIMARY)
		{
			for(y = 0; y < list[x]->dscvsl; y++)
			{
				fatal(ts_ensure, ts, tsa, tsl + 1);
				ts_reset((*ts)[tsl]);

				(*ts)[tsl++]->fmlv = list[x]->dscvs[y];
			}
			n++;
		}
	}

	logf(L_DSCEXEC, "discovery @ " XQUOTE(FABTMPDIR) "/pid/%d/dsc", g_params.pid);
	logf(L_DSCINFO, "discovery -- for %3d nodes", n);

	size_t newn = 0;
	size_t newr = 0;
	fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, 0);

	logf(L_DSCEXEC, "discovery -- for %3d nodes found %3d nodes and %3d edges", n, newn, newr);

	if(log_would(L_TIME))
	{
		fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_end);

		size_t z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
		logf(L_TIME, "elapsed : %.*s", (int)z, space);
	}

	finally : coda;
}

int dsc_exec_entire(map * vmap, transform_parser * const gp, lwx *** stax, size_t * staxa, size_t staxp, ts *** ts, size_t * tsa, size_t * tsw)
{
	struct timespec	time_start;
	struct timespec	time_end;
	char space[64];

	int x;
	int y;

	// start measuring
	fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);
	
	// assign each threadspace a discovery formula evaluation context
	int tsl = 0;
	int n = 0;
	for(x = 0; x < gn_nodes.l; x++)
	{
		if(gn_nodes.e[x]->invalid & GN_INVALIDATION_DISCOVERY)
		{
			for(y = 0; y < gn_nodes.e[x]->dscvsl; y++)
			{
				fatal(ts_ensure, ts, tsa, tsl + 1);
				ts_reset((*ts)[tsl]);

				(*ts)[tsl++]->fmlv = gn_nodes.e[x]->dscvs[y];
			}

			n++;
		}
	}

	if(n)
	{
		logf(L_DSCEXEC, "discovery @ " XQUOTE(FABTMPDIR) "/pid/%d/dsc", g_params.pid);
		logf(L_DSCINFO, "discovery %2d for %3d nodes", 0, n);

		size_t newn = 0;
		size_t newr = 0;
		fatal(dsc_execwave, vmap, gp, stax, staxa, staxp, *ts, tsl, tsw, &newn, &newr, 0);

		// sum discovered objects
		logf(L_DSCEXEC, "discovery %2d for %3d nodes found %3d nodes and %3d edges", 0, n, newn, newr);

		if(log_would(L_TIME))
		{
			fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_end);

			size_t z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
			logf(L_TIME, "elapsed : %.*s", (int)z, space);
		}
	}

	finally : coda;
}
