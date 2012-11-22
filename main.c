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
#include "dep.h"

#include "macros.h"
#include "control.h"

// signal handling
static int o_signum;
void signal_handler(int signum)
{
	log(L_INFO, "shutdown signal: %d", signum);
exit(0);
	if(!o_signum)
	{
		o_signum = signum;
	}
}

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
		int									stax_l = 0;
		int									stax_a = 0;
		int 								p = 0;

		int x;
		int i;
		int j;
		int k;

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

		// initialize logger
		fatal(log_init, "+ERROR +WARN +INFO +BPEXEC");

		// parse cmdline arguments
		fatal(parse_args, argc, argv);

		// create/cleanup tmp 
		fatal(tmp_setup);

		// parse the fabfile
		fatal(ff_mkparser, &ffp);
		fatal(ff_parse, ffp, g_args.fabfile, &ffn);

		if(!ffn)
			return 0;

		if(log_would(L_FF | L_FFTREE))
			ff_dump(ffn);

		// register object types with liblistwise
		fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);

		// create map for variable definitions
		fatal(map_create, &vmap, 0);

		// use up one list and populate the # variable (all directories)
		if(stax_a <= p)
		{
			fatal(xrealloc, &stax, sizeof(*stax), p + 1, stax_a);
			stax_a = p + 1;
		}
		if(!stax[p])
			fatal(xmalloc, &stax[p], sizeof(*stax[p]));
		lstack_reset(stax[p]);

		fatal(lstack_add, stax[p], ffn->ff_dir, strlen(ffn->ff_dir));
		fatal(var_set, vmap, "#", stax[p++]);

		// process the fabfile tree, construct the graph
		for(x = 0; x < ffn->statements_l; x++)
		{
			if(ffn->statements[x]->type == FFN_DEPENDENCY)
			{
				fatal(dep_add, ffn->statements[x], vmap, &stax, &stax_l, &stax_a, p, first ? 0 : &first);
			}
			else if(ffn->statements[x]->type == FFN_VARDECL)
			{
				// resolve the list associated to the variable name
				fatal(list_resolve, ffn->statements[x]->definition, vmap, &stax, &stax_l, &stax_a, p);

				// save the resultant list
				fatal(var_set, vmap, ffn->statements[x]->name, stax[p++]);
			}
			else if(ffn->statements[x]->type == FFN_FORMULA)
			{
				// add the formula
				fatal(fml_add, ffn->statements[x], vmap, &stax, &stax_l, &stax_a, p);
			}
		}

		// dump graph nodes, pending logging
		if(g_args.dumpnode_all)
		{
			for(x = 0; x < gn_nodes.l; x++)
				gn_dump(gn_nodes.e[x]);
		}
		else if(g_args.dumpnode)
		{
			for(x = 0; x < g_args.dumpnode_len; x++)
			{
				gn * gn = 0;
				if(gn_nodes.by_path && (gn = idx_lookup_val(gn_nodes.by_path, &g_args.dumpnode[x], 0)))
				{
					gn_dump(gn);
				}
				else
				{
					fail("not found : %s", g_args.dumpnode[x]);
				}
			}
		}
		else
		{
			// lookup gn for each target
			gn **		node_list = 0;
			int			node_list_len = 0;

			char *	space = 0;
			int			aretasks = 0;
			int			istask = 0;

			fatal(xmalloc, &node_list, sizeof(node_list[0]) * MAX(g_args.targets_len, 1));

			// add gn's for each target
			for(x = 0; x < g_args.targets_len; x++)
			{
				gn * gn = 0;
				if(gn_nodes.by_path)
				{
					char * sp = g_args.targets[x];
					while(gn == 0)
					{
						if(gn = idx_lookup_val(gn_nodes.by_path, &sp, 0))
						{
							istask = strcmp(gn->dir, "/..") == 0;
							if(aretasks && !istask)
								fail("cannot mix tasks and file-backed-nodes");
							aretasks |= istask;

							node_list[node_list_len++] = gn;
						}
						else if(sp != space)
						{
							fatal(xrealloc, &space, 1, strlen(g_args.targets[x]) + 5, 0);
							sprintf(space, "/../%s", g_args.targets[x]);
							sp = space;
						}
						else
						{
							break;
						}
					}
				}

				if(gn == 0)
				{
					fail("unknown : %s", g_args.targets[x]);
				}
			}

			// default target is the first dependency processed
			if(node_list_len == 0 && first)
			{
				node_list[node_list_len++] = first;
			}

			if(node_list_len)
			{
				// traverse the graph, construct the build plan that culminates in the given targets
				fatal(bp_create, node_list, node_list_len, &bp);

				// prune the buildplan of nodes which do not require updating
				if(bp_prune(bp) == 0)
					return 0;
			}

			// dump buildplan, pending logging
			if(bp)
				bp_dump(bp);

			if(!bp || bp->stages_l == 0)
				log(L_INFO, "nothing to fabricate");

			if(bp && g_args.mode == MODE_FABRICATE)
			{
				// execute the build plan, one stage at a time
				if(bp_exec(bp, vmap, &stax, &stax_l, &stax_a, p) == 0)
					return 0;
			}
		}

		return 1;
	};

	int R = !domain() || o_signum;
	log(L_INFO, "exiting with status: %d", R);
	
	return R;
}
