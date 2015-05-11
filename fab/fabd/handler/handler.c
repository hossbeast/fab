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

#include <stdlib.h>
#include <string.h>

#include "xlinux.h"

#include "listwise.h"
#include "listwise/lstack.h"

#include "global.h"

#include "parseint.h"
#include "macros.h"
#include "map.h"
#include "memblk.h"
#include "unitstring.h"
#include "say.h"
#include "cksum.h"

#define restrict __restrict

// errors to report and continue, otherwise exit
static int fab_swallow[] = {
	  [ FAB_SYNTAX ] = 1
	, [ FAB_ILLBYTE ] = 1
	, [ FAB_UNSATISFIED ] = 1
	, [ FAB_CMDFAIL ] = 1
	, [ FAB_NOINVOKE ] = 1
	, [ FAB_BADPLAN ] = 1
	, [ FAB_CYCLE ] = 1
	, [ FAB_NOSELECT ] = 1
	, [ FAB_DSCPARSE ] = 1
};

//
// static
//

/// hashfiles
//
// SUMMARY
//  load hashes for backing files
//
static int hashfiles(int iteration)
{
	int x;

	// stat backing files
	for(x = 0; x < gn_nodes.l; x++)
	{
		if(gn_nodes.e[x]->mark != iteration)
		{
			if(gn_nodes.e[x]->type & GN_TYPE_HASFILE)
			{
				// load hash
				fatal(hashblock_stat, gn_nodes.e[x]->path->can, gn_nodes.e[x]->hb);

				if(gn_nodes.e[x]->hb->stathash[1] == 0)
				{
					if(!gn_nodes.e[x]->invalid)
						logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_NXFILE), gn_nodes.e[x]->idstring);

					// missing file
					gn_nodes.e[x]->invalid |= GN_INVALIDATION_NXFILE;
				}
				else
				{
					gn_nodes.e[x]->invalid &= ~GN_INVALIDATION_NXFILE;

					if(hashblock_cmp(gn_nodes.e[x]->hb))
					{
						if(!gn_nodes.e[x]->invalid)
							logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_CHANGED), gn_nodes.e[x]->idstring);

						// changed file
						gn_nodes.e[x]->invalid |= GN_INVALIDATION_CHANGED;
					}
				}

				// convert to actions
				if(GN_IS_INVALID(gn_nodes.e[x]))
				{
					if(gn_nodes.e[x]->type == GN_TYPE_PRIMARY && gn_nodes.e[x]->dscvsl)
					{
						// require discovery
						gn_nodes.e[x]->invalid |= GN_INVALIDATION_DISCOVERY;
					}
					else if(gn_nodes.e[x]->type != GN_TYPE_PRIMARY)
					{
						// require fabrication
						gn_nodes.e[x]->invalid |= GN_INVALIDATION_FABRICATE;
					}
				}
			}

			gn_nodes.e[x]->mark = iteration;
		}
	}

	finally : coda;
}

//
// public
//

int handler_context_mk(handler_context ** const restrict ctx)
{
	fatal(xmalloc, ctx, sizeof(**ctx));

	fatal(map_create, &(*ctx)->bakemap, 0);
	fatal(map_create, &(*ctx)->smap, 0);

	finally : coda;
}

void handler_context_reset(handler_context * ctx)
{
	// per-request reset
	ctx->tsl = 0;
	ctx->fabricationsl = 0;
	ctx->fabricationxsl = 0;
	ctx->fabricationnsl = 0;
	ctx->invalidationsl = 0;
	ctx->discoveriesl = 0;
	ctx->inspectionsl = 0;
	ctx->queriesl = 0;

	map_clear(ctx->bakemap);
	map_clear(ctx->smap);
}

void handler_context_free(handler_context * ctx)
{
	if(ctx)
	{
		free(ctx->fabrications);
		free(ctx->fabricationxs);
		free(ctx->fabricationns);
		free(ctx->invalidations);
		free(ctx->discoveries);
		free(ctx->inspections);
		free(ctx->queries);

		psfree(ctx->ptmp);
		map_free(ctx->smap);
		bp_free(ctx->plan);

		int x;
		for(x = 0; x < ctx->staxa; x++)
		{
			if(ctx->stax[x])
				free(lwx_getptr(ctx->stax[x]));
			lwx_free(ctx->stax[x]);
		}
		free(ctx->stax);

		for(x = 0; x < ctx->tsa; x++)
			ts_free(ctx->tsp[x]);
		free(ctx->tsp);

		map_free(ctx->bakemap);
	}

	free(ctx);
}

void handler_context_xfree(handler_context ** const restrict ctx)
{
	handler_context_free(*ctx);
	*ctx = 0;
}

/// handler
//
// SUMMARY
//  handles a request
//
int handler(handler_context * const restrict ctx, struct map * const restrict vmap, struct gn * const restrict first)
{
	int x;
	int y;

	int fd = -1;

	char space[2048];

	handler_context_reset(ctx);

	// track start time
	g_params.starttime = time(0);

	static int iteration;
	iteration++;

	// load hashes for backing files : set up actions
	fatal(hashfiles, iteration);

	// comprehensive dependency discovery
	fatal(dsc_exec_entire, vmap, ctx->ffp->gp, &ctx->stax, &ctx->staxa, ctx->staxp, &ctx->tsp, &ctx->tsa, &ctx->tsw);

	// load hashes for newly found backing files (header files)
	fatal(hashfiles, iteration);

	// process selectors
	if(g_args->selectorsl)
	{
		fatal(selector_init);

		int pn = ctx->staxp;

		// create $!
		fatal(lw_reset, &ctx->stax, &ctx->staxa, pn);
		for(x = 0; x < gn_nodes.l; x++)
			fatal(lstack_obj_add, ctx->stax[pn], gn_nodes.e[x], LISTWISE_TYPE_GNLW);

		// map for processing selectors
		fatal(map_set, ctx->smap, MMS("!"), MM(ctx->stax[pn]), 0);
		pn++;

		// execute selectors
		for(x = 0; x < g_args->selectorsl; x++)
			fatal(selector_process, &g_args->selectors[x], x, ctx->ffp, ctx->smap, &ctx->stax, &ctx->staxa, pn);

		// get result lists
		fatal(selector_finalize, ctx);

		if(log_would(L_LISTS))
		{
			if(fabricationsl + fabricationxsl + fabricationnsl + invalidationsl + discoveriesl + inspectionsl == 0)
			{
				logf(L_LISTS, "empty");
			}
			
			for(x = 0; x < fabricationsl; x++)
				logf(L_LISTS, "fabrication(s)     =%s", (*fabrications[x])->idstring);

			for(x = 0; x < fabricationxsl; x++)
				logf(L_LISTS, "fabricationx(s)    =%s", (*fabricationxs[x])->idstring);

			for(x = 0; x < fabricationnsl; x++)
				logf(L_LISTS, "fabricationn(s)    =%s", (*fabricationns[x])->idstring);

			for(x = 0; x < invalidationsl; x++)
				logf(L_LISTS, "invalidation(s)    =%s", (*invalidations[x])->idstring);

			for(x = 0; x < discoveriesl; x++)
				logf(L_LISTS, "discover(y)(ies)   =%s", (*discoveries[x])->idstring);

			for(x = 0; x < inspectionsl; x++)
				logf(L_LISTS, "inspection(s)      =%s", (*inspections[x])->idstring);
		}
	}

	if(invalidationsl)
	{
		int repeat_discovery = 0;
		fatal(gn_process_invalidations, invalidations, invalidationsl, &repeat_discovery);

		if(repeat_discovery)
		{
			// repeat dependency discovery if any primary nodes that have dscvs were invalidated
			fatal(dsc_exec_entire, vmap, ctx->ffp->gp, &ctx->stax, &ctx->staxa, ctx->staxp, &ctx->tsp, &ctx->tsa, &ctx->tsw);
		}
	}

	// propagate invalidations and clear changes from this iteration
	int visit(gn * gn, int d)
	{
		if(d == 0)
		{
			// this will include the node itself and nodes with a weak dependency on it
		}
		else
		{
			if(!gn->invalid)
			{
				logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_SOURCES), gn->idstring);
			}

			gn->invalid |= GN_INVALIDATION_SOURCES;

			if(gn->type != GN_TYPE_PRIMARY)
			{
				// require fabrication
				gn->invalid |= GN_INVALIDATION_FABRICATE;
			}
		}

		finally : coda;
	};
	for(x = 0; x < gn_nodes.l; x++)
	{
		if(gn_nodes.e[x]->invalid & (GN_INVALIDATION_CHANGED | GN_INVALIDATION_NXFILE | GN_INVALIDATION_USER))
		{
			// do not cross the nofile boundary
			fatal(traverse_depth_bynodes_feedsward_skipweak_usebridge_nonofile, gn_nodes.e[x], visit);

			gn_nodes.e[x]->invalid &= ~(GN_INVALIDATION_CHANGED | GN_INVALIDATION_USER);
		}
	}

	if(g_args->selectors_arequery)
	{
#define CENTER(w, fmt, ...) ({																		\
	int act = snprintf(0, 0, fmt, ##__VA_ARGS__);										\
	snprintf(space, sizeof(space), "%*s"fmt"%*s"										\
		, (MAX(w - act, 0) / 2) + ((MAX(w - act, 0) % 2) ? 1 : 0), ""	\
		, ##__VA_ARGS__																								\
		, (MAX(w - act, 0) / 2), ""																		\
	);																															\
	space;																													\
})

		SAYF(" %-40s | %-13s | %-11s | %-11s | %s\n", "id", "type", "degree", "invalidated", "reason");
		for(x = 0; x < queriesl; x++)
		{
			// id
			if((*queries[x])->idstringl > 40)
			{
				SAYF(" .. %-37s", (*queries[x])->idstring + ((*queries[x])->idstringl - 37));
			}
			else
			{
				SAYF(" %-40s", (*queries[x])->idstring);
			}

			// type
			SAYF(" | %-13s", GN_TYPE_STR((*queries[x])->type));

			// degree
			SAYF(" | %11s", CENTER(11, "%-d/%d", (*queries[x])->needs.l, (*queries[x])->feeds.l));

			// invalid
			SAYF(" | %-11s", CENTER(11, "%s", (*queries[x])->invalid ? "x" : ""));

			// reason
			SAYS(" | ");
			if((*queries[x])->type & GN_TYPE_HASFILE)
			{
				gn_invalid_reasons_write((*queries[x]), space, sizeof(space));
				SAYS(space);
			}
			SAYF("\n");
		}
		SAYF(" %d nodes\n", queriesl);
	}

	if(g_args->selectors_arediscovery)
	{
		fatal(log_parse_and_describe, "+DSCRES", 0, 0, L_INFO);
		fatal(dsc_exec_specific, discoveries, discoveriesl, vmap, ctx->ffp->gp, &ctx->stax, &ctx->staxa, ctx->staxp, &ctx->tsp, &ctx->tsa, &ctx->tsw);
		fatal(log_parse_pop);
	}

	if(g_args->selectors_areinspections)
	{
		fatal(log_parse_and_describe, "+NODE", 0, 0, L_INFO);

		for(x = 0; x < inspectionsl; x++)
			gn_dump((*inspections[x]));

		fatal(log_parse_pop);
	}

	if(g_args->selectors_arequery == 0 && g_args->selectors_areinspections == 0 && g_args->selectors_arediscovery == 0)
	{
		//
		// construct a buildplan - use the first node if none was specified
		//
		if(fabricationsl + fabricationxsl + fabricationnsl == 0 && first)
		{
			fatal(xrealloc, &fabrications, sizeof(*fabrications), 1, 0);
			fabrications[fabricationsl++] = &first;
		}

		// mixing task and non-task as buildplan targets does not make sense
		for(x = 1; x < fabricationsl + fabricationxsl + fabricationnsl; x++)
		{
			int a = x - 1;
			int b = x;
			gn * A = 0;
			gn * B = 0;

			if(a < fabricationsl)
				A = *fabrications[a];
			else if(a < (fabricationsl + fabricationxsl))
				A = *fabricationxs[a - fabricationsl];
			else
				A = *fabricationns[a - fabricationsl - fabricationxsl];

			if(b < fabricationsl)
				B = *fabrications[b];
			else if(b < (fabricationsl + fabricationxsl))
				B = *fabricationxs[b - fabricationsl];
			else
				B = *fabricationns[b - fabricationsl - fabricationxsl];

			if((A->type == GN_TYPE_TASK) ^ (B->type == GN_TYPE_TASK))
				fails(FAB_BADPLAN, "cannot mix task and non-task targets");
		}

		fatal(bp_create, fabrications, fabricationsl, fabricationxs, fabricationxsl, fabricationns, fabricationnsl, &plan);

		if(plan)
		{
			if(g_args->mode_bplan == MODE_BPLAN_BUILDSCRIPT)
			{
				// prepare bs_runtime_vars map
				fatal(map_create, &bakemap, 0);

				for(x = 0; x < g_args->bs_runtime_varsl; x++)
					fatal(map_set, bakemap, MMS(g_args->bs_runtime_vars[x]), 0, 0, 0);

				// dump buildplan, pending logging
				if(plan)
					bp_dump(plan);

				// write buildscript to the IPC dir
				fatal(buildscript_mk
					, ctx->plan
					, g_args->argvs
					, vmap
					, ctx->ffp->gp
					, &ctx->stax
					, &ctx->staxa
					, ctx->staxp
					, ctx->bakemap
					, &ctx->tsp
					, &ctx->tsa
					, &ctx->tsw
					, g_params.ipcstem
				);
			}
			else
			{
				// incremental build - prune the buildplan
				fatal(bp_eval, plan);

				if(g_args->mode_bplan == MODE_BPLAN_GENERATE)
					fatal(log_parse_and_describe, "+BPDUMP", 0, 0, L_INFO);

				// dump buildplan, pending logging
				if(plan)
					bp_dump(plan);

				if(g_args->mode_bplan == MODE_BPLAN_EXEC)
				{
					if(plan && plan->stages_l)
					{
						// create tmp directory for the build
						fatal(psloadf, &ptmp, XQUOTE(FABTMPDIR) "/pid/%d/bp", g_params.fab_pid);
						fatal(mkdirp, ptmp->s, FABIPC_DIR);
						
						// create symlink to the bp in hashdir
						snprintf(space, sizeof(space), "%s/bp", g_params.ipcstem);
						fatal(uxunlink, space, 0);
						fatal(xsymlink, ptmp->s, space);

						// create file with the number of stages
						fatal(psloadf, &ptmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/stages", g_params.fab_pid);
						fatal(uxunlink, ptmp->s, 0);
						fatal(ixclose, &fd);
						fatal(xopen_mode, ptmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
						fatal(axwrite, fd, &plan->stages_l, sizeof(plan->stages_l));
						
						// create file with the number of commands
						fatal(psloadf, &ptmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/commands", g_params.fab_pid);
						fatal(uxunlink, ptmp->s, 0);
						fatal(ixclose, &fd);
						fatal(xopen_mode, ptmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
						int cmdsl = 0;
						for(x = 0; x < plan->stages_l; x++)
							cmdsl += plan->stages[x].evals_l;
						fatal(axwrite, fd, &cmdsl, sizeof(cmdsl));

						// prepare and execute the build plan, one stage at a time
						int i;
						for(i = 0; i < plan->stages_l; i++)
						{
							tsl = 0;
							fatal(bp_prepare_stage, plan, i, ctx->ffp->gp, &stax, &staxa, staxp, &tsp, &tsl, &tsa);

							// block signals
							fatal(xsigprocmask, SIG_SETMASK, &all, 0);

							// work required ; notify fab
							fatal(uxkill, g_params.fab_pid, FABSIG_BPSTART, 0);

							// await response
							o_signum = 0;
							sigsuspend(&none);
							fatal(xsigprocmask, SIG_SETMASK, &none, 0);

							if(o_signum == FABSIG_BPGOOD)
							{
								// plan was executed successfully ; update nodes for all products
								for(x = 0; x < tsl; x++)
								{
									for(y = 0; y < tsp[x]->fmlv->productsl; y++)
									{
										// mark as up-to-date
										tsp[x]->fmlv->products[y]->invalid = 0;

										// reload hashblock
										fatal(hashblock_stat, tsp[x]->fmlv->products[y]->path->can, tsp[x]->fmlv->products[y]->hb);
									}
								}
							}
							else if(o_signum == FABSIG_BPBAD)
							{
								// failure ; harvest the results
								fatal(bp_harvest_stage, plan, i);
								break;
							}
							else
							{
								failf(FAB_BADIPC, "expected signal %d or %d, actual %d", FABSIG_BPGOOD, FABSIG_BPBAD, o_signum);
							}
						}
					}
				}
			}
		}
	}

finally:
	bp_xfree(&plan);
	fatal(ixclose, &fd);
coda;
}
