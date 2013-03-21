#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

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

#include "macros.h"
#include "control.h"
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
	strstack *					sstk = 0;				// scope stack
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

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// assume identity of the executing user
	fatal(identity_assume_user);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals
	int x;
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);

	// parse cmdline arguments
	//  (parse_args also calls log_init with a default string)
	fatal(parse_args, argc, argv);

	// create/cleanup tmp 
	fatal(tmp_setup);

	// other initializations
	fatal(traverse_init);

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	for(x = 0; x < sizeof(FABLW_DIRS) / sizeof(FABLW_DIRS[0]); x++)
		fatal(listwise_register_opdir, FABLW_DIRS[x]);

	// create stack for scope resolution
	fatal(strstack_create, &sstk);
	fatal(strstack_push, sstk, "..");

	// parse the initial fabfile
	fatal(ff_mkparser, &ffp);

	// create the rootmap
	fatal(var_root, &rmap);

	// seed the map identifier mechanism
	fatal(map_set, rmap, MMS("?LVL"), MM((int[1]){ 0 }));
	fatal(map_set, rmap, MMS("?NUM"), MM((int[1]){ 0 }));
	fatal(map_set, rmap, MMS("?CLD"), MM((int[1]){ 0 }));

	// cmdline-specified variables populate the rootmap
	for(x = 0; x < g_args.varkeysl; x++)
	{
		fatal(list_ensure, &stax, &staxa, staxp);
		fatal(lstack_add, stax[staxp], g_args.varvals[x], strlen(g_args.varvals[x]));
		fatal(var_set, rmap, g_args.varkeys[x], stax[staxp++], 1, 0, 0);
	}

	// use up one list and populate the # variable (relative directory path to the initial fabfile)
	fatal(list_ensure, &stax, &staxa, staxp);
	fatal(lstack_add, stax[staxp], g_args.init_fabfile_path->rel_dir, g_args.init_fabfile_path->rel_dirl);
	fatal(var_set, rmap, "#", stax[staxp++], 1, 0, 0);

	// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
	fatal(var_clone, rmap, &vmap);

	// parse, starting with the initial fabfile, construct the graph
	fatal(ffproc, ffp, g_args.init_fabfile_path, sstk, vmap, &stax, &staxa, &staxp, &first, 0);

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

	// comprehensive upfront dependency discovery on the entire graph
	if(g_args.mode_ddsc == MODE_DDSC_UPFRONT)
	{
		gn ** gnl = alloca(sizeof(*gnl) * gn_nodes.l);
		memcpy(gnl, gn_nodes.e, sizeof(*gnl) * gn_nodes.l);
		fatal(dsc_exec, gnl, gn_nodes.l, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, 0);
	}

	// dump graph nodes, pending logging
	if(g_args.mode_exec == MODE_EXEC_DUMP)
	{
		// process invalidations, update designations
		fatal(gn_invalidations);

		for(x = 0; x < g_args.dumpnodesl; x++)
		{
			int ll = listl[0];
			fatal(gn_match, g_args.init_fabfile_path->abs_dir, g_args.dumpnodes[x], &list[0], &listl[0], &lista[0]);

			if(ll == listl[0])
			{
				log(L_WARN, "dumpnode : %s not found", g_args.dumpnodes[x]);
			}
		}

		int i;
		for(i = 0; i < listl[0]; i++)
		{
			if(list[0][i]->designation == GN_DESIGNATION_PRIMARY)
				gn_primary_reload(list[0][i]);
			if(list[0][i]->designation == GN_DESIGNATION_SECONDARY)
				gn_secondary_reload(list[0][i]);

			gn_dump(list[0][i]);
		}
	}
	else
	{
		// target list
		for(x = 0; x < g_args.targetsl; x++)
		{
			int ll = listl[0];
			fatal(gn_match, g_args.init_fabfile_path->abs_dir, g_args.targets[x], &list[0], &listl[0], &lista[0]);

			if(ll == listl[0])
			{
				fail("target : %s not found", g_args.targets[x]);
			}
		}

		// check for mixing task and non-task
		if(listl[0])
		{
			gn_designate(list[0][0]);
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
			if(g_args.mode_exec == MODE_EXEC_DDSC)
			{
				// execute discovery
				fatal(dsc_exec, list[0], listl[0], vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, 0);
			}
			else if(g_args.mode_exec == MODE_EXEC_FABRICATE || g_args.mode_exec == MODE_EXEC_BUILDPLAN || g_args.mode_exec == MODE_EXEC_BAKE)
			{
				int new = 1;
				while(new)
				{
					// traverse the graph, construct the build plan that culminates in the given target(s)
					// bp_create also updates all node designations
					bp_xfree(&bp);
					fatal(bp_create, list[0], listl[0], &bp);

					new = 0;
					if(g_args.mode_ddsc == MODE_DDSC_DEFERRED)
					{
						// flat list of nodes in the buildplan
						fatal(bp_flatten, bp, &list[1], &listl[1], &lista[1]);

						// execute discovery
						fatal(dsc_exec, list[1], listl[1], vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, &new);
					}
				}

				if(g_args.mode_exec == MODE_EXEC_BAKE)
				{
					// dump buildplan, pending logging
					if(bp)
						bp_dump(bp);
					
					// create bakescript
					fatal(bake_bp, bp, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw, g_args.bakescript_path);
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

finally:
	ff_freeparser(ffp);
	bp_free(bp);
	map_free(rmap);
	map_free(vmap);
	strstack_free(sstk);

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

	gn_teardown();
	fml_teardown();
	ff_teardown();
	args_teardown();
	traverse_teardown();

	log(L_INFO, "exiting with status : %d", !_coda_r);
	log_teardown();
	return !_coda_r;
}
