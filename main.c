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

#include <listwise/lstack.h>
#include <listwise/object.h>

#include "args.h"
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

#include "log.h"
#include "control.h"
#include "macros.h"
#include "xmem.h"

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
	ff_parser *					ffp = 0;				// fabfile parser
	bp *								bp = 0;					// buildplan 
	map * 							rmap = 0;				// root-level map
	map *								vmap = 0;				// init-level map
	map *								tmap = 0;				// temp map
	gn *								first = 0;			// first dependency mentioned
	lstack **						stax = 0;				// listwise stacks
	int									staxa = 0;
	int 								staxp = 0;
	ts **								ts = 0;
	int									tsa = 0;
	int									tsw = 0;
	gn **								list[2] = { };
	int									listl[2] = { };
	int									lista[2] = { };

	/* node selector produced lists */
	gn ***							fabrications = 0;
	int									fabricationsl = 0;
	gn ***							fabricationxs = 0;
	int									fabricationxsl = 0;
	gn ***							invalidations = 0;
	int									invalidationsl = 0;
	gn ***							discoveries = 0;
	int									discoveriesl = 0;
	gn ***							inspections = 0;
	int									inspectionsl = 0;
	gn ***							queries = 0;
	int									queriesl = 0;

	int x;
	int y;

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

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	for(x = 0; x < sizeof(FABLW_DIRS) / sizeof(FABLW_DIRS[0]); x++)
		fatal(listwise_register_opdir, FABLW_DIRS[x]);

	// parse cmdline arguments
	//  (args_parse also calls log_init with a default string)
	fatal(args_parse, argc, argv);

	// create/cleanup tmp 
	fatal(tmp_setup);

	// other initializations
	fatal(gn_init);
	fatal(traverse_init);
	fatal(ff_mkparser, &ffp);

	// unless LWVOCAL, arrange for liblistwise to write to /dev/null
	if(!log_would(L_LWVOCAL))
	{
		if((listwise_err_fd = open("/dev/null", O_WRONLY)) == -1)
			fail("open(/dev/null)=[%d][%s]", errno, strerror(errno));
	}
	
	// create the rootmap
	fatal(var_root, &rmap);

	// parse variable expression text from cmdline (the -v option)
	for(x = 0; x < g_args.rootvarsl; x++)
	{
		ff_file * vff = 0;
		fatal(ff_var_parse, ffp, g_args.rootvars[x], strlen(g_args.rootvars[x]), x, &vff);

		if(vff)
		{
			// process variable expressions
			int k;
			for(k = 0; k < vff->ffn->statementsl; k++)
			{
				ff_node * stmt = vff->ffn->statements[k];

				if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARXFM_ADD || stmt->type == FFN_VARXFM_SUB)
				{
					int pn = staxp;
					fatal(list_resolve, stmt->definition, rmap, ffp->gp, &stax, &staxa, &staxp, 0);
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
		else
		{
			qfail();
		}
	}

	// use up one list and populate the # variable (relative directory path to the initial fabfile)
	fatal(lw_reset, &stax, &staxa, staxp);
	fatal(lstack_add, stax[staxp], g_args.init_fabfile_path->rel_dir, g_args.init_fabfile_path->rel_dirl);
	fatal(var_set, rmap, "#", stax[staxp++], 1, 0, 0);

	// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
	fatal(var_clone, rmap, &vmap);

	// parse, starting with the initial fabfile, construct the graph
	fatal(ffproc, ffp, g_args.init_fabfile_path, vmap, &stax, &staxa, &staxp, &first, 0);

	// comprehensive upfront dependency discovery on the entire graph
	lista[0] = listl[0] = gn_nodes.l;
	fatal(xmalloc, &list[0], sizeof(*list[0]) * lista[0]);
	memcpy(list[0], gn_nodes.e, sizeof(*list[0]) * listl[0]);

	fatal(dsc_exec, list[0], listl[0], vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, 0);

	// apply flags and designations
	fatal(gn_finalize);

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
		fatal(map_create, &tmap, 0);
		fatal(map_set, tmap, MMS("!"), MM(stax[pn]));
		pn++;

		// process selectors
		for(x = 0; x < g_args.selectorsl; x++)
		{
			fatal(selector_process, &g_args.selectors[x], x, ffp, tmap, &stax, &staxa, pn);
		}

		fatal(selector_finalize
			, &fabrications, &fabricationsl
			, &fabricationxs, &fabricationxsl
			, &invalidations, &invalidationsl
			, &discoveries, &discoveriesl
			, &inspections, &inspectionsl
			, &queries, &queriesl
		);

		map_xfree(&tmap);

		if(log_would(L_LISTS))
		{
			if(fabricationsl + fabricationxsl + invalidationsl + discoveriesl + inspectionsl + queriesl == 0)
			{
				log(L_LISTS, "empty");
			}
			
			for(x = 0; x < fabricationsl; x++)
				log(L_LISTS, "fabrication(s)     =%s", " ", ' ', (*fabrications[x])->idstring);

			for(x = 0; x < fabricationxsl; x++)
				log(L_LISTS, "fabricationx(s)    =%s", " ", ' ', (*fabricationxs[x])->idstring);

			for(x = 0; x < invalidationsl; x++)
				log(L_LISTS, "invalidation(s)    =%s", " ", ' ', (*invalidations[x])->idstring);

			for(x = 0; x < discoveriesl; x++)
				log(L_LISTS, "discover(y)(ies)   =%s", " ", ' ', (*discoveries[x])->idstring);

			for(x = 0; x < inspectionsl; x++)
				log(L_LISTS, "inspection(s)      =%s", " ", ' ', (*inspections[x])->idstring);

			for(x = 0; x < queriesl; x++)
				log(L_LISTS, "quer(y)(ies)       =%s", " ", ' ', (*queries[x])->idstring);
		}
	}

exit(0);

	// process hashblocks for regular fabfiles which have changed
	for(x = 0; x < ff_files.l; x++)
	{
		if(ff_files.e[x]->type == FFT_REGULAR)
		{
			if(hashblock_cmp(ff_files.e[x]->hb))
			{
				fatal(ff_regular_rewrite, ff_files.e[x]);
			}
		}
	}

/*
	// dump graph nodes, pending logging
	if(g_args.mode_exec == MODE_EXEC_INSPECT)
	{
		for(x = 0; x < inspectionsl; x++)
			gn_dump((*inspections[x]));
	}
	else
	{
		// check for mixing task and non-task
		if(listl[0])
		{
			for(x = 1; x < listl[0]; x++)
			{
				gn_designate(list[0][x]);

				if((list[0][x-1]->designation == GN_DESIGNATION_TASK) ^ (list[0][x]->designation == GN_DESIGNATION_TASK))
					fail("cannot mix task and non-task targets");
			}
		}
		else if(first)
		{
			lista[0] = 1;
			fatal(xmalloc, &list[0], sizeof(*list[0]) * lista[0]);
			list[0][listl[0]++] = first;	// default target
		}

		if(listl[0])
		{
			if(g_args.mode_exec == MODE_EXEC_FABRICATE || g_args.mode_exec == MODE_EXEC_BUILDPLAN || g_args.mode_exec == MODE_EXEC_BAKE)
			{
				fatal(bp_create, list[0], listl[0], &bp);

				if(g_args.mode_exec == MODE_EXEC_BAKE)
				{
					// dump buildplan, pending logging
					if(bp)
					{
						bp_dump(bp);
					
						// create bakescript
						fatal(bake_bp, bp, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, g_args.bakescript_path);
					}
				}
				if(g_args.mode_exec == MODE_EXEC_FABRICATE || g_args.mode_exec == MODE_EXEC_BUILDPLAN)
				{
					// prune the buildplan of nodes which do not require updating - incremental build
					int poison = 0;
					qfatal(bp_eval, bp, &poison);

					if(poison)
						qfail();

					// dump buildplan, pending logging
					if(bp)
						bp_dump(bp);
					
					if(g_args.mode_exec == MODE_EXEC_FABRICATE)
					{
						if(!bp || bp->stages_l == 0)
						{
							log(L_INFO, "nothing to fabricate");
						}
						else
						{
							// execute the build plan, one stage at a time
							qfatal(bp_exec, bp, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);

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

*/

finally:
	ff_freeparser(ffp);
	bp_free(bp);
	map_free(rmap);
	map_free(vmap);
	map_free(tmap);

	for(x = 0; x < staxa; x++)
	{
		if(stax[x])
			free(stax[x]->ptr);
		lstack_free(stax[x]);
	}
	free(stax);

	for(x = 0; x < tsa; x++)
		ts_free(ts[x]);
	free(ts);

	for(x = 0; x < sizeof(list) / sizeof(list[0]); x++)
		free(list[x]);

	free(fabrications);
	free(fabricationxs);
	free(invalidations);
	free(inspections);
	free(queries);

	gn_teardown();
	fml_teardown();
	ff_teardown();
	args_teardown();
	traverse_teardown();
	selector_teardown();

	log(L_INFO, "exiting with status : %d", !_coda_r);
	log_teardown();
	return !_coda_r;
}
