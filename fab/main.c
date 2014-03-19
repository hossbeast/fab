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

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "listwise/object.h"
#include "listwise/xtra.h"

#include "xlinux.h"

#include "args.h"
#include "params.h"
#include "ff.h"
#include "gn.h"
#include "gnlw.h"
#include "fml.h"
#include "bp.h"
#include "log.h"
#include "map.h"
#include "var.h"
#include "list.h"
#include "dsc.h"
#include "dep.h"
#include "tmp.h"
#include "identity.h"
#include "bake.h"
#include "ffproc.h"
#include "traverse.h"
#include "lwutil.h"
#include "selector.h"
#include "error.h"

#include "log.h"
#include "global.h"
#include "macros.h"

// signal handling
static int o_signum;
static void signal_handler(int signum)
{
	exit(0);
	if(!o_signum)
	{
		o_signum = signum;
	}
}

int main(int argc, char** argv)
{
	char space[1024];

	ff_parser *					ffp = 0;				// fabfile parser
	bp *								bp = 0;					// buildplan 
	map * 							rmap = 0;				// root-level map
	map *								vmap = 0;				// init-level map
	map * 							bakemap = 0;		// bakedvars map
	gn *								first = 0;			// first dependency mentioned
	lwx **							stax = 0;				// listwise stacks
	int									staxa = 0;
	int 								staxp = 0;
	ts **								ts = 0;
	int									tsa = 0;
	int									tsw = 0;

	/* node selector produced lists */
	map *								smap = 0;							// selector map (must have lifetime >= lifetime of the selector lists)
	gn ***							fabrications = 0;
	int									fabricationsl = 0;
	gn ***							fabricationxs = 0;
	int									fabricationxsl = 0;
	gn ***							fabricationns = 0;
	int									fabricationnsl = 0;
	gn ***							invalidations = 0;
	int									invalidationsl = 0;
	gn ***							discoveries = 0;
	int									discoveriesl = 0;
	gn ***							inspections = 0;
	int									inspectionsl = 0;

	int x;
	int y;
	size_t tracesz = 0;

	// initialize error tables
	error_setup();

	// process parameter gathering
	params_setup();

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// assume identity of the executing user
	fatal(identity_assume_user);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);

	// parse cmdline arguments
	//  (args_parse also calls log_init with a default string)
	fatal(args_parse, argc, argv);

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	fatal(listwise_register_opdir, XQUOTE(FABLWOPDIR));

#if DEBUG || DEVEL || SANITY
	// configure liblistwise logging
	lw_configure_logging();
#endif

	// other initializations
	fatal(tmp_setup);
	fatal(gn_init);
	fatal(traverse_init);
	fatal(ff_mkparser, &ffp);

	// create the rootmap
	fatal(var_root, &rmap);

	// parse variable expression text from cmdline (the -v option)
	for(x = 0; x < g_args.rootvarsl; x++)
	{
		ff_file * vff = 0;
		fatal(ff_var_parse, ffp, g_args.rootvars[x], strlen(g_args.rootvars[x]), x, &vff);

		// process variable expressions
		int k;
		for(k = 0; k < vff->ffn->statementsl; k++)
		{
			ff_node * stmt = vff->ffn->statements[k];

			if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARXFM_ADD || stmt->type == FFN_VARXFM_SUB)
			{
				int pn = staxp;
				fatal(list_resolve, stmt->definition, rmap, ffp->gp, &stax, &staxa, &staxp, 0, 0);
				staxp++;

				for(y = 0; y < stmt->varsl; y++)
				{
					if(stmt->type == FFN_VARASSIGN)
					{
						fatal(var_set, rmap, stmt->vars[y]->name, stax[pn], 1, 0, stmt);
					}
					else if(stmt->type == FFN_VARXFM_ADD)
					{
						fatal(var_xfm_add, rmap, stmt->vars[y]->name, stax[pn], 1, stmt);
					}
					else if(stmt->type == FFN_VARXFM_SUB)
					{
						fatal(var_xfm_sub, rmap, stmt->vars[y]->name, stax[pn], 1, stmt);
					}
				}
			}
			else if(stmt->type == FFN_VARXFM_LW)
			{
				for(y = 0; y < stmt->varsl; y++)
				{
					if(stmt->generator_node)
					{
						fatal(var_xfm_lw, rmap, stmt->vars[y]->name, stmt->generator_node->generator, stmt->generator_node->text, 1, stmt);
					}
					else if(stmt->generator_list_node)
					{

					}
				}
			}
		}
	}

	// use up one list and populate the # variable (relative directory path to the initial fabfile)
	fatal(lw_reset, &stax, &staxa, staxp);
	fatal(lstack_add, stax[staxp], g_params.init_fabfile_path->abs_dir, g_params.init_fabfile_path->abs_dirl);
	fatal(var_set, rmap, "#", stax[staxp++], 1, 0, 0);

	// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
	fatal(var_clone, rmap, &vmap);

	// parse, starting with the initial fabfile, construct the graph
	fatal(ffproc, ffp, g_params.init_fabfile_path, vmap, &stax, &staxa, &staxp, &first);

	// process hashblocks for regular fabfiles that have changed
	fatal(ff_regular_reconcile);

	// compute flags and designations, reload files and dscv cache
	fatal(gn_finalize, 0);

	// comprehensive upfront dependency discovery on the entire graph
	fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
	fatal(gn_reconcile);
	fatal(gn_finalize, 1);

	// process selectors
	if(g_args.selectorsl)
	{
		fatal(selector_init);

		int pn = staxp;

		// create $!
		fatal(lw_reset, &stax, &staxa, pn);
		for(x = 0; x < gn_nodes.l; x++)
			fatal(lstack_obj_add, stax[pn], gn_nodes.e[x], LISTWISE_TYPE_GNLW);

		// map for processing selectors
		fatal(map_create, &smap, 0);
		fatal(map_set, smap, MMS("!"), MM(stax[pn]), 0);
		pn++;

		if(g_args.selectors_arequery)
			fatal(log_parse_and_describe, "+SELECT", 0, L_INFO);

		// process selectors
		for(x = 0; x < g_args.selectorsl; x++)
			fatal(selector_process, &g_args.selectors[x], x, ffp, smap, &stax, &staxa, pn);

		if(g_args.selectors_arequery == 0)
		{
			fatal(selector_finalize
				, &fabrications, &fabricationsl
				, &fabricationxs, &fabricationxsl
				, &fabricationns, &fabricationnsl
				, &invalidations, &invalidationsl
				, &discoveries, &discoveriesl
				, &inspections, &inspectionsl
			);

			if(log_would(L_LISTS))
			{
				if(fabricationsl + fabricationxsl + fabricationnsl + invalidationsl + discoveriesl + inspectionsl == 0)
				{
					log(L_LISTS, "empty");
				}
				
				for(x = 0; x < fabricationsl; x++)
					log(L_LISTS, "fabrication(s)     =%s", (*fabrications[x])->idstring);

				for(x = 0; x < fabricationxsl; x++)
					log(L_LISTS, "fabricationx(s)    =%s", (*fabricationxs[x])->idstring);

				for(x = 0; x < fabricationnsl; x++)
					log(L_LISTS, "fabricationn(s)    =%s", (*fabricationns[x])->idstring);

				for(x = 0; x < invalidationsl; x++)
					log(L_LISTS, "invalidation(s)    =%s", (*invalidations[x])->idstring);

				for(x = 0; x < discoveriesl; x++)
					log(L_LISTS, "discover(y)(ies)   =%s", (*discoveries[x])->idstring);

				for(x = 0; x < inspectionsl; x++)
					log(L_LISTS, "inspection(s)      =%s", (*inspections[x])->idstring);
			}
		}
	}

	if(g_args.selectors_arequery == 0)
	{
		// dependency discovery list
		if(discoveriesl)
		{
			fatal(log_parse_and_describe, "+DSC", 0, L_INFO);
			fatal(dsc_exec_specific, discoveries, discoveriesl, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
		}
		else
		{
			// process invalidations
			fatal(gn_process_invalidations, invalidations, invalidationsl);
			fatal(gn_finalize, 1);

			// dependency discovery on invalidated primary nodes
			fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
			fatal(gn_finalize, 1);

			// process inspections
			if(inspectionsl)
			{
				// enable DGRAPH
				fatal(log_parse_and_describe, "+DGRAPH", 0, L_INFO);

				for(x = 0; x < inspectionsl; x++)
					gn_dump((*inspections[x]));
			}
			else
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

					if((A->designate == GN_DESIGNATION_TASK) ^ (B->designate == GN_DESIGNATION_TASK))
						fails(FAB_BADPLAN, "cannot mix task and non-task targets");
				}

				fatal(bp_create, fabrications, fabricationsl, fabricationxs, fabricationxsl, fabricationns, fabricationnsl, &bp);

				// selector lists not referenced again past this point
				map_xfree(&smap);

				if(bp)
				{
					if(g_args.mode_bplan == MODE_BPLAN_BAKE)
					{
						// prepare bakevars map
						fatal(map_create, &bakemap, 0);

						for(x = 0; x < g_args.bakevarsl; x++)
							fatal(map_set, bakemap, MMS(g_args.bakevars[x]), 0, 0, 0);

						// dump buildplan, pending logging
						if(bp)
							bp_dump(bp);

						// create bakescript
						fatal(bake_bp, bp, vmap, ffp->gp, &stax, &staxa, staxp, bakemap, &ts, &tsa, &tsw, g_args.bakescript_path);
					}
					else
					{
						// incremental build - prune the buildplan
						fatal(bp_eval, bp);

						if(g_args.mode_bplan == MODE_BPLAN_GENERATE)
							fatal(log_parse_and_describe, "+BPDUMP", 0, L_INFO);

						// dump buildplan, pending logging
						if(bp)
							bp_dump(bp);

						if(g_args.mode_bplan == MODE_BPLAN_EXEC)
						{
							if(bp && bp->stages_l)
							{
								// execute the build plan, one stage at a time
								fatal(bp_exec, bp, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);

								// commit regular fabfile hashblocks
								for(x = 0; x < ff_files.l; x++)
								{
									if(ff_files.e[x]->type == FFT_REGULAR)
									{
										fatal(hashblock_write, ff_files.e[x]->hb);
									}
								}
							}
						}
					}
				}
			}
		}
	}

finally:
	ff_freeparser(ffp);
	bp_free(bp);
	map_free(rmap);
	map_free(vmap);
	map_free(smap);
	map_free(bakemap);

	for(x = 0; x < staxa; x++)
	{
		if(stax[x])
			free(lwx_getptr(stax[x]));
		lwx_free(stax[x]);
	}
	free(stax);

	for(x = 0; x < tsa; x++)
		ts_free(ts[x]);
	free(ts);

	free(fabrications);
	free(fabricationxs);
	free(fabricationns);
	free(invalidations);
	free(discoveries);
	free(inspections);

	gn_teardown();
	fml_teardown();
	ff_teardown();
	args_teardown();
	params_teardown();
	traverse_teardown();
	selector_teardown();

	if(XAPI_UNWINDING)
	{
#if DEBUG
		if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
		{
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEBUG
		}
		else
		{
			tracesz = xapi_trace_full(space, sizeof(space));
		}
#endif
	}

	const etable * etab = 0;
	int code = 0;
conclude2(&etab, &code);
	
	if(tracesz)
	{
		log_write(L_ERROR, space, tracesz);

		if(etab == perrtab_FAB && code == FAB_BADARGS)
		{
//			usage(0);
		}
	}
	else
	{
		log(L_INFO, "exiting with status : %d", code);
	}

	log_teardown();
	return code;
}
