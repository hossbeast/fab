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

// vmap destructor
static void vmap_destructor(void*, void*);

int main(int argc, char** argv)
{
	int domain()
	{
		ff_parser *					ffp = 0;
		ff_node *						ffn = 0;
		bp *								bp = 0;
		map *								vmap = 0;				// variable lookup map
		gn *								first = 0;			// first dependency mentioned
		lstack **						stax = 0;
		int									staxa = 0;
		int 								staxp = 0;
		ts **								ts = 0;
		int									tsa = 0;
		int									tsw = 0;
		gn **								list = 0;
		int									listl = 0;
		int									lista = 0;
		gn **								node_list = 0;
		int									node_list_len = 0;
		strstack *					scope = 0;

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

		fatal(ff_mkparser, &ffp);

		// parse the initial fabfile
		fatal(ff_parse_path, ffp, g_args.init_fabfile_path, &ffn);

		if(!ffn)
			qfail();

		// register object types with liblistwise
		fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);

		// create map for variable definitions
		fatal(map_create, &vmap, vmap_destructor);
		fatal(strstack_create, &scope);

		// populate cmdline-specified variables with sticky definitions
		for(x = 0; x < g_args.varkeysl; x++)
		{
			fatal(list_ensure, &stax, &staxa, staxp);
			fatal(lstack_add, stax[staxp], g_args.varvals[x], strlen(g_args.varvals[x]));
			fatal(var_push, vmap, g_args.varkeys[x], stax[staxp++], VV_LS, 1);
		}

		// use up one list and populate the # variable (relative directory path to the initial fabfile)
		fatal(list_ensure, &stax, &staxa, staxp);
		fatal(lstack_add, stax[staxp], g_args.init_fabfile_path->rel_dir, g_args.init_fabfile_path->rel_dirl);
		fatal(var_push, vmap, "#", stax[staxp++], VV_LS, 0);

		// process the fabfile tree, construct the graph
		fatal(ffproc, ffn, ffp, g_args.init_fabfile_path->rel_dir, scope, vmap, &stax, &staxa, staxp, &first);

		// process hashblocks for fabfiles which have changed
		if(hashblock_cmp(ffn->loc.ff->hb))
		{
			fatal(ff_regular_rewrite, ffn->loc.ff);
		}

		// comprehensive upfront dependency discovery on the entire graph
		if(g_args.mode_ddsc == MODE_DDSC_UPFRONT)
		{
			gn ** gnl = alloca(sizeof(*gnl) * gn_nodes.l);
			memcpy(gnl, gn_nodes.e, sizeof(*gnl) * gn_nodes.l);
			fatal(dsc_exec, gnl, gn_nodes.l, vmap, &stax, &staxa, staxp, &ts, &tsa, &tsw, 0);
		}

		// dump graph nodes, pending logging
		if(g_args.mode_exec == MODE_EXEC_DUMP)
		{
			// process invalidations, update designations
			fatal(gn_invalidations);

			for(x = 0; x < g_args.dumpnodesl; x++)
			{
				listl = 0;
				fatal(gn_lookup_match, g_args.dumpnodes[x], &list, &listl, &lista);

				if(listl == 0)
				{
					log(L_WARN, "dumpnodes : %s not found", g_args.dumpnodes[x]);
				}
				else
				{
					int i;
					for(i = 0; i < listl; i++)
					{
						if(list[i]->designation == GN_DESIGNATION_PRIMARY)
							gn_primary_reload(list[i]);
						if(list[i]->designation == GN_DESIGNATION_SECONDARY)
							gn_secondary_reload(list[i]);

						gn_dump(list[i]);
					}
				}
			}
		}
		else
		{
			// lookup gn for each target
			int			aretasks = 0;
			int			istask = 0;

			fatal(xmalloc, &node_list, sizeof(node_list[0]) * MAX(g_args.targetsl, 1));

			// add gn's for each target
			if(g_args.targetsl)
			{
				gn * gn = 0;
				fatal(gn_lookup_nofile, g_args.targets[0], 0, g_args.init_fabfile_path->abs_dir, &gn);

				if(gn == 0)
					fail("target : %s not found", g_args.targets[0]);

				gn_designate(gn);
				aretasks = gn->designation == GN_DESIGNATION_TASK;
				node_list[node_list_len++] = gn;

				for(x = 1; x < g_args.targetsl; x++)
				{
					fatal(gn_lookup_nofile, g_args.targets[x], 0, g_args.init_fabfile_path->abs_dir, &gn);

					if(gn == 0)
						fail("target : %s not found", g_args.targets[x]);

					gn_designate(gn);
					istask = gn->designation == GN_DESIGNATION_TASK;
					if(aretasks ^ istask)
						fail("cannot mix task and non-task fabrication targets");

					node_list[node_list_len++] = gn;
				}
			}

			// default target is the first dependency processed
			if(node_list_len == 0 && first)
			{
				node_list[node_list_len++] = first;
			}

			if(node_list_len)
			{
				if(g_args.mode_exec == MODE_EXEC_DDSC)
				{
					// execute discovery
					fatal(dsc_exec, node_list, node_list_len, vmap, &stax, &staxa, staxp, &ts, &tsa, &tsw, 0);
				}
				else if(g_args.mode_exec == MODE_EXEC_FABRICATE || g_args.mode_exec == MODE_EXEC_BUILDPLAN || g_args.mode_exec == MODE_EXEC_BAKE)
				{
					int new = 1;
					while(new)
					{
						// traverse the graph, construct the build plan that culminates in the given target(s)
						// bp_create also updates all node designations
						bp_xfree(&bp);
						fatal(bp_create, node_list, node_list_len, &bp);

						new = 0;
						if(g_args.mode_ddsc == MODE_DDSC_DEFERRED)
						{
							// flat list of nodes in the buildplan
							fatal(bp_flatten, bp, &list, &listl, &lista);

							// execute discovery
							fatal(dsc_exec, list, listl, vmap, &stax, &staxa, staxp, &ts, &tsa, &tsw, &new);
						}
					}

					if(g_args.mode_exec == MODE_EXEC_BAKE)
					{
						// dump buildplan, pending logging
						if(bp)
							bp_dump(bp);
						
						// create bakescript
						fatal(bake_bp, bp, vmap, &stax, &staxa, staxp, &ts, &tsa, &tsw, g_args.bakescript_path);
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
								qfatal(bp_exec, bp, vmap, &stax, &staxa, staxp, &ts, &tsa, &tsw);

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
		map_free(vmap);

		for(x = 0; x < staxa; x++)
			lstack_free(stax[x]);
		free(stax);

		for(x = 0; x < tsa; x++)
			ts_free(ts[x]);
		free(ts);

		free(list);
		free(node_list);

		gn_teardown();
		fml_teardown();
		ff_teardown();
		args_teardown();
	coda;
	};

	int R = !domain() || o_signum;
	log(L_INFO, "exiting with status: %d", R);

	log_teardown();
	
	return R;
}

void vmap_destructor(void* tk, void* tv)
{
	var_container_free(*(var_container**)tv);
}

/*

		for(x = 0; x < ffn->statements_l; x++)
		{
			ff_node* stmt = ffn->statements[x];

			if(ffn->statements[x]->type == FFN_INVOCATION)
			{
				
			}
			else if(ffn->statements[x]->type == FFN_DEPENDENCY)
			{
				fatal(dep_process, ffn->statements[x], vmap, &stax, &staxa, staxp, first ? 0 : &first, 0, 0, 0);
			}
			else if(ffn->statements[x]->type == FFN_FORMULA)
			{
				// add the formula, attach to graph nodes
				fatal(fml_add, ffn->statements[x], vmap, &stax, &staxa, staxp);
			}
			else if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARPUSH || stmt->type == FFN_VARPOP)
			{
				// get the value, if any
				void * v = 0;
				uint8_t t = 0;

				if(stmt->definition)
				{
					if(stmt->definition->type == FFN_LIST)
					{
						fatal(list_resolve, stmt->definition, vmap, &stax, &staxa, staxp);
						v = stax[staxp++];
						t = VV_LS;
					}
					if(stmt->definition->type == FFN_WORD)
					{
						v = stmt->definition->text;
						t = VV_AL;
					}
				}
				else
				{
					if(stmt->type == FFN_VARPUSH)
					{
						fatal(list_empty, &stax, &staxa, staxp);
						v = stax[staxp++];
						t = VV_LS;
					}
				}

				// apply to all referenced vars
				for(y = 0; y < stmt->vars_l; y++)
				{
					char * var = stmt->vars[y]->text;

					if(stmt->type == FFN_VARASSIGN)
					{
						// clear the stack for this variable
						int r;
						fatal(var_undef, vmap, var, &r);

						// r is whether it was actually cleared, which is prevented by a sticky value on top
						if(r && v)
						{
							fatal(var_push, vmap, var, v, t, 0);
						}
					}
					if(stmt->type == FFN_VARPUSH)
					{
						fatal(var_push, vmap, var, v, t, 0);
					}
					if(stmt->type == FFN_VARPOP)
					{
						if(v)
						{
							fatal(var_push, vmap, var, v, t, 0);
						}
					}
				}
			}
		}
*/
