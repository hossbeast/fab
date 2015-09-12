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
#include "pstring.h"
#include "narrate.h"

#include "listwise.h"
#include "listwise/lstack.h"

#include "global.h"
#include "fabd_handler.h"
#include "params.h"
#include "lwutil.h"
#include "gn.h"
#include "hashblock.h"
#include "ff.h"
#include "bp.h"
#include "dsc.h"
#include "selector.h"
#include "list.h"
#include "gnlw.h"
#include "traverse.h"
#include "bs.h"
#include "dirutil.h"
#include "graph.h"
#define EXECUTOR_CONTEXT fabd_handler_context
#include "executor.h"
#include "ts.h"

#include "path.h"
#include "sigbank.h"

#include "parseint.h"
#include "macros.h"
#include "map.h"
#include "memblk.h"
#include "unitstring.h"
#include "cksum.h"

#define restrict __restrict

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
	for(x = 0; x < graph.l; x++)
	{
		if(graph.e[x]->mark != iteration)
		{
			if(graph.e[x]->type & GN_TYPE_HASFILE)
			{
				// load hash
				fatal(hashblock_stat, graph.e[x]->path->can, graph.e[x]->hb);

				if(graph.e[x]->hb->stathash[1] == 0)
				{
					if(!graph.e[x]->invalid)
						logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_NXFILE), graph.e[x]->idstring);

					// missing file
					graph.e[x]->invalid |= GN_INVALIDATION_NXFILE;
				}
				else
				{
					graph.e[x]->invalid &= ~GN_INVALIDATION_NXFILE;

					if(hashblock_cmp(graph.e[x]->hb))
					{
						if(!graph.e[x]->invalid)
							logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_CHANGED), graph.e[x]->idstring);

						// changed file
						graph.e[x]->invalid |= GN_INVALIDATION_CHANGED;
					}
				}

				// convert to actions
				if(GN_IS_INVALID(graph.e[x]))
				{
					if(graph.e[x]->type == GN_TYPE_PRIMARY && graph.e[x]->dscvsl)
					{
						// require discovery
						graph.e[x]->invalid |= GN_INVALIDATION_DISCOVERY;
					}
					else if(graph.e[x]->type != GN_TYPE_PRIMARY)
					{
						// require fabrication
						graph.e[x]->invalid |= GN_INVALIDATION_FABRICATE;
					}
				}
			}

			graph.e[x]->mark = iteration;
		}
	}

	finally : coda;
}

static void bp_log_exec(
    struct fabd_handler_context * const restrict ctx
  , const struct executor_logging * const restrict logging
  , const char * const restrict subdir
  , int num
  , int bad
  , int r
)
{
  uint64_t tag = L_BPCMD;
  if(bad)
    tag |= L_ERROR;

  int x = ctx->stage; // stage index
  int y = num;        // command index
  int z;              // product index
  for(z = 0; z < ctx->plan->stages[x].evals[y]->productsl; z++)
  {
    if(z == 0)
    {
      log_start(tag);
      logf(0, "[%2d,%3d] %s %s", x, y
        , GN_TYPE_STR(ctx->plan->stages[x].evals[y]->products[z]->type)
        , ctx->plan->stages[x].evals[y]->products[z]->idstring
      );

      if(WIFEXITED(r) && WEXITSTATUS(r))
        logf(0, ", exit status : %d", WEXITSTATUS(r));
      else if(WIFSIGNALED(r))
        logf(0, ", signal : %d", WTERMSIG(r));

      if(bad)
      {
        logf(0, ", details @ %s/%s/%d", g_params.ipcdir, subdir, y);
      }

      log_finish();
    }
    else
    {
      logf(tag, "%*s %s %s", 8, ""
        , GN_TYPE_STR(ctx->plan->stages[x].evals[y]->products[z]->type)
        , ctx->plan->stages[x].evals[y]->products[z]->idstring
      );
    }
  }
}

//
// public
//

int fabd_handler_context_mk(fabd_handler_context ** const restrict ctx)
{
	fatal(xmalloc, ctx, sizeof(**ctx));

	fatal(map_create, &(*ctx)->bakemap, 0);
	fatal(map_create, &(*ctx)->smap, 0);

  ff_mkparser(&(*ctx)->ffp);

	finally : coda;
}

void fabd_handler_context_reset(fabd_handler_context * ctx)
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

void fabd_handler_context_free(fabd_handler_context * ctx)
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

		psfree(ctx->tmp);
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
    ff_freeparser(ctx->ffp);
	}

	free(ctx);
}

void fabd_handler_context_xfree(fabd_handler_context ** const restrict ctx)
{
	fabd_handler_context_free(*ctx);
	*ctx = 0;
}

int fabd_handler_handle_request(fabd_handler_context * const restrict ctx, struct map * const restrict vmap, struct gn * const restrict first)
{
	int x;
	int y;

	int fd = -1;

	char space[2048];

	fabd_handler_context_reset(ctx);

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
		for(x = 0; x < graph.l; x++)
			fatal(lstack_obj_add, ctx->stax[pn], graph.e[x], LISTWISE_TYPE_GNLW);

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
			if(ctx->fabricationsl + ctx->fabricationxsl + ctx->fabricationnsl + ctx->invalidationsl + ctx->discoveriesl + ctx->inspectionsl == 0)
			{
				logf(L_LISTS, "empty");
			}
			
			for(x = 0; x < ctx->fabricationsl; x++)
				logf(L_LISTS, "ctx->fabrication(s)     =%s", ctx->fabrications[x]->idstring);

			for(x = 0; x < ctx->fabricationxsl; x++)
				logf(L_LISTS, "ctx->fabricationx(s)    =%s", ctx->fabricationxs[x]->idstring);

			for(x = 0; x < ctx->fabricationnsl; x++)
				logf(L_LISTS, "ctx->fabricationn(s)    =%s", ctx->fabricationns[x]->idstring);

			for(x = 0; x < ctx->invalidationsl; x++)
				logf(L_LISTS, "ctx->invalidation(s)    =%s", ctx->invalidations[x]->idstring);

			for(x = 0; x < ctx->discoveriesl; x++)
				logf(L_LISTS, "discover(y)(ies)   =%s", ctx->discoveries[x]->idstring);

			for(x = 0; x < ctx->inspectionsl; x++)
				logf(L_LISTS, "inspection(s)      =%s", ctx->inspections[x]->idstring);
		}
	}

	if(ctx->invalidationsl)
	{
		int repeat_discovery = 0;
		fatal(gn_process_invalidations, ctx->invalidations, ctx->invalidationsl, &repeat_discovery);

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
	for(x = 0; x < graph.l; x++)
	{
		if(graph.e[x]->invalid & (GN_INVALIDATION_CHANGED | GN_INVALIDATION_NXFILE | GN_INVALIDATION_USER))
		{
			// do not cross the nofile boundary
			fatal(traverse_depth_bynodes_feedsward_skipweak_usebridge_nonofile, graph.e[x], visit);

			graph.e[x]->invalid &= ~(GN_INVALIDATION_CHANGED | GN_INVALIDATION_USER);
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

		sayf(" %-40s | %-13s | %-11s | %-11s | %s\n", "id", "type", "degree", "invalidated", "reason");
		for(x = 0; x < ctx->queriesl; x++)
		{
			// i
			if(ctx->queries[x]->idstringl > 40)
			{
				sayf(" .. %-37s", ctx->queries[x]->idstring + (ctx->queries[x]->idstringl - 37));
			}
			else
			{
				sayf(" %-40s", ctx->queries[x]->idstring);
			}

			// type
			sayf(" | %-13s", GN_TYPE_STR(ctx->queries[x]->type));

			// degree
			sayf(" | %11s", CENTER(11, "%-d/%d", ctx->queries[x]->needs.l, ctx->queries[x]->feeds.l));

			// invalid
			sayf(" | %-11s", CENTER(11, "%s", ctx->queries[x]->invalid ? "x" : ""));

			// reason
			says(" | ");
			if(ctx->queries[x]->type & GN_TYPE_HASFILE)
			{
				gn_invalid_reasons_write(ctx->queries[x], space, sizeof(space));
				says(space);
			}
			sayf("\n");
		}
		sayf(" %d nodes\n", ctx->queriesl);
	}

	if(g_args->selectors_arediscovery)
	{
		fatal(log_parse_and_describe, "+DSCRES", 0, 0, L_INFO);
		fatal(dsc_exec_specific, ctx->discoveries, ctx->discoveriesl, vmap, ctx->ffp->gp, &ctx->stax, &ctx->staxa, ctx->staxp, &ctx->tsp, &ctx->tsa, &ctx->tsw);
		fatal(log_parse_pop);
	}

	if(g_args->selectors_areinspections)
	{
		fatal(log_parse_and_describe, "+NODE", 0, 0, L_INFO);

		for(x = 0; x < ctx->inspectionsl; x++)
			gn_dump(ctx->inspections[x]);

		fatal(log_parse_pop);
	}

	if(g_args->selectors_arequery == 0 && g_args->selectors_areinspections == 0 && g_args->selectors_arediscovery == 0)
	{
		//
		// construct a buildplan - use the first node if none was specified
		//
		if(ctx->fabricationsl + ctx->fabricationxsl + ctx->fabricationnsl == 0 && first)
		{
			fatal(xrealloc, &ctx->fabrications, sizeof(ctx->fabrications), 1, 0);
			ctx->fabrications[ctx->fabricationsl++] = first;
		}

		// mixing task and non-task as buildplan targets does not make sense
		for(x = 1; x < ctx->fabricationsl + ctx->fabricationxsl + ctx->fabricationnsl; x++)
		{
			int a = x - 1;
			int b = x;
			gn * A = 0;
			gn * B = 0;

			if(a < ctx->fabricationsl)
				A = ctx->fabrications[a];
			else if(a < (ctx->fabricationsl + ctx->fabricationxsl))
				A = ctx->fabricationxs[a - ctx->fabricationsl];
			else
				A = ctx->fabricationns[a - ctx->fabricationsl - ctx->fabricationxsl];

			if(b < ctx->fabricationsl)
				B = ctx->fabrications[b];
			else if(b < (ctx->fabricationsl + ctx->fabricationxsl))
				B = ctx->fabricationxs[b - ctx->fabricationsl];
			else
				B = ctx->fabricationns[b - ctx->fabricationsl - ctx->fabricationxsl];

			if((A->type == GN_TYPE_TASK) ^ (B->type == GN_TYPE_TASK))
				fails(FAB_BADPLAN, "cannot mix task and non-task targets");
		}

		fatal(bp_create, ctx->fabrications, ctx->fabricationsl, ctx->fabricationxs, ctx->fabricationxsl, ctx->fabricationns, ctx->fabricationnsl, &ctx->plan);

		if(ctx->plan)
		{
			if(g_args->mode_bplan == MODE_BPLAN_BUILDSCRIPT)
			{
				for(x = 0; x < g_args->bs_runtime_varsl; x++)
					fatal(map_set, ctx->bakemap, MMS(g_args->bs_runtime_vars[x]), 0, 0, 0);

				// dump buildplan, pending logging
				if(ctx->plan)
					bp_dump(ctx->plan);

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
					, g_params.ipcdir
				);
			}
			else
			{
				// incremental build - prune the buildplan
				fatal(bp_eval, ctx->plan);

				if(g_args->mode_bplan == MODE_BPLAN_GENERATE)
					fatal(log_parse_and_describe, "+BPDUMP", 0, 0, L_INFO);

				// dump buildplan, pending logging
				if(ctx->plan)
					bp_dump(ctx->plan);

				if(g_args->mode_bplan == MODE_BPLAN_EXEC)
				{
					if(ctx->plan && ctx->plan->stagesl)
					{
						// create tmp directory for the build
						fatal(psloadf, &ctx->tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp", g_params.fab_pid);
						fatal(mkdirp, ctx->tmp->s, FABIPC_DIR);
						
						// create symlink to the bp in ipcdir
						snprintf(space, sizeof(space), "%s/bp", g_params.ipcdir);
						fatal(uxunlink, space, 0);
						fatal(xsymlink, ctx->tmp->s, space);

#if 0
						// create file with the number of stages
						fatal(psloadf, &ctx->tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/stages", g_params.fab_pid);
						fatal(uxunlink, ctx->tmp->s, 0);
						fatal(ixclose, &fd);
						fatal(xopen_mode, ctx->tmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
						fatal(axwrite, fd, &ctx->plan->stagesl, sizeof(ctx->plan->stagesl));
						
						// create file with the number of commands
						fatal(psloadf, &ctx->tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/commands", g_params.fab_pid);
						fatal(uxunlink, ctx->tmp->s, 0);
						fatal(ixclose, &fd);
						fatal(xopen_mode, ctx->tmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
						int cmdsl = 0;
						for(x = 0; x < ctx->plan->stagesl; x++)
							cmdsl += ctx->plan->stages[x].evals_l;
						fatal(axwrite, fd, &cmdsl, sizeof(cmdsl));
#endif
            logf(L_BPEXEC, "buildplan @ %s/bp", g_params.ipcdir);

						// prepare and execute the build plan, one stage at a time
						int i;
						for(i = 0; i < ctx->plan->stagesl; i++)
						{
              // write the commands for the stage to the directory
							fatal(bp_prepare_stage, ctx->plan, i, ctx->ffp->gp, &ctx->stax, &ctx->staxa, ctx->staxp, &ctx->tsp, &ctx->tsl, &ctx->tsa);

							logf(L_BPINFO, "stage %2d of %2d executing %3d of %3d"
                , i
                , ctx->plan->stagesl
                , ctx->plan->stages[i].evalsl
                , ctx->plan->evalsl
              );

              // execute the stage
              int success;
              ctx->stage = i;
              fatal(executor_execute
                , ctx
                , (executor_logging[]) {{ cmd_cat : L_BPCMD, exec_cat : L_BPEXEC, log_exec : (void*)bp_log_exec }}
                , "bp/%d"
                , ctx->plan->stages[i].evalsl
                , &success
              );

							//if(sig == FABSIG_BPGOOD)
							if(success)
							{
								// plan was executed successfully ; update nodes for all products
								for(x = 0; x < ctx->tsl; x++)
								{
									for(y = 0; y < ctx->tsp[x]->fmlv->productsl; y++)
									{
										// mark as up-to-date
										ctx->tsp[x]->fmlv->products[y]->invalid = 0;

										// reload hashblock
										fatal(hashblock_stat, ctx->tsp[x]->fmlv->products[y]->path->can, ctx->tsp[x]->fmlv->products[y]->hb);
									}
								}
							}
							else if(0)//sig == FABSIG_BPBAD)
							{
								// failure ; harvest the results
								fatal(bp_harvest_stage, ctx->plan, i);
								break;
							}
						}
					}
				}
			}
		}
	}

finally:
	bp_xfree(&ctx->plan);
	fatal(ixclose, &fd);
coda;
}