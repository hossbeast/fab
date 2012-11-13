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
		lstack **						astax = 0;
		int									astax_l = 0;
		int									astax_a = 0;
		lstack **						bstax = 0;
		int									bstax_l = 0;
		int									bstax_a = 0;

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

		// register object types with liblistwise
		fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);

		// create map for variable definitions
		fatal(map_create, &vmap, 0);

		// default variables
		lstack* ls = 0;
		fatal(xmalloc, &ls, sizeof(*ls));
		ls->sel.all = 1;
		
		fatal(lstack_add, ls, ffn->ff_dir, strlen(ffn->ff_dir));

		// included directories
		fatal(var_set, vmap, "#", ls);

		if(g_args.targets_len)
		{
			fatal(xmalloc, &ls, sizeof(*ls));

			// add gn's for each target, and add those to the vmap
			for(x = 0; x < g_args.targets_len; x++)
			{
				gn * gn = 0;
				fatal(gn_add, ffn->ff_dir, g_args.targets[x], &gn);
				fatal(lstack_obj_add, ls, &gn, LISTWISE_TYPE_GNLW);
			}
			fatal(var_set, vmap, "*", ls);
		}

		// the first target is the default
		gn * def = 0;

		// process the fabfile tree, constructing the graph
		for(x = 0; x < ffn->statements_l; x++)
		{
			if(ffn->statements[x]->type == FFN_DEPENDENCY)
			{
				// resolve both lists
				fatal(list_resolve, ffn->statements[x]->needs, vmap, &astax, &astax_l, &astax_a);
				fatal(list_resolve, ffn->statements[x]->feeds, vmap, &bstax, &bstax_l, &bstax_a);

				// add edges, which are the cartesian product needs x feeds
				LSTACK_LOOP_ITER(astax[0], i, goa);
				if(goa)
				{
					LSTACK_LOOP_ITER(bstax[0], j, gob);
					if(gob)
					{
						if(!def)
						{
							fatal(gn_edge_add, ffn->statements[x]->ff_dir, astax[0]->s[0].s[i].s, bstax[0]->s[0].s[j].s, &def);

							// add the gn for the default target to the vmap
							fatal(xmalloc, &ls, sizeof(*ls));
							fatal(lstack_obj_add, ls, def, LISTWISE_TYPE_GNLW);
							ls->sel.all = 1;
							fatal(var_set, vmap, "*", ls);
						}
						else
							fatal(gn_edge_add, ffn->statements[x]->ff_dir, astax[0]->s[0].s[i].s, bstax[0]->s[0].s[j].s, 0);
					}
					LSTACK_LOOP_DONE;
				}
				LSTACK_LOOP_DONE;
			}
			else if(ffn->statements[x]->type == FFN_VARDECL)
			{
				// resolve the list associated to the variable name
				fatal(list_resolve, ffn->statements[x]->definition, vmap, &astax, &astax_l, &astax_a);

				// save the resultant list
				fatal(var_set, vmap, ffn->statements[x]->name, astax[0]);

				astax_l--;
				astax_a--;
			}
			else if(ffn->statements[x]->type == FFN_FORMULA)
			{
				// create the formula
				fml * fml = 0;
				fatal(fml_add, ffn->statements[x], &fml);

				// resolve the list of targets
				fatal(list_resolve, ffn->statements[x]->targets, vmap, &astax, &astax_l, &astax_a);

				// attach formula to graph nodes
				for(j = 0; j < gn_nodes.l; j++)
				{
					LSTACK_LOOP_ITER(astax[0], i, go);
					if(go)
					{
						if(strcmp(gn_nodes.e[j]->path, astax[0]->s[0].s[i].s) == 0)
						{
							gn_nodes.e[j]->fml = fml;
							break;
						}
					}
					LSTACK_LOOP_DONE;
				}
			}
		}

		// dump graph nodes, pending logging
		gn_dumpall();

		// lookup gn for each target
		gn ** list = 0;
		int list_len = 0;

		if(g_args.targets_len)
		{
			list_len = g_args.targets_len;
			list = calloc(sizeof(*list), list_len);

			for(x = 0; x < list_len; x++)
			{
				if((list[x] = idx_lookup_val(gn_nodes.by_path, &g_args.targets[x], 0)) == 0)
				{
					fail("unknown target : %s", g_args.targets[x]);
				}
			}
		}
		else if(def)
		{
			list_len = 1;
			list = calloc(sizeof(*list), list_len);

			if((list[0] = idx_lookup_val(gn_nodes.by_path, &def->path, 0)) == 0)
			{
				fail("unknown target : %s", g_args.targets[x]);
			}
		}

		// traverse the graph, construct the build plan that culminates in the given targets
		fatal(bp_create, list, list_len, &bp);

		// prune the buildplan of nodes which do not require updating
		fatal(bp_prune, bp);

		if(bp->stages_l == 0)
			log(L_INFO, "No targets require fabrication");

		// dump buildplan, pending logging
		bp_dump(bp);

		if(g_args.mode == MODE_FABRICATE)
		{
			// execute the build plan, one stage at a time
			fatal(bp_exec, bp);
		}

		return 1;
	};

	int R = !domain() || o_signum;
	log(L_INFO, "exiting with status: %d", R);
	
	return R;
}
