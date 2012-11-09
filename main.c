#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "args.h"
#include "ff.h"
#include "gn.h"
#include "fml.h"
#include "bp.h"
#include "log.h"

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
		ff_parser *		ffp = 0;
		ff_node *			ffn = 0;
		bp *					bp = 0;

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

		// initialize logger
		fatal(log_init, "+L_ERROR +L_WARN +L_INFO +L_BPEXEC");

		// parse cmdline arguments
		fatal(parse_args, argc, argv);

		// create/cleanup tmp 
		fatal(tmp_setup);

		// parse the fabfile
		fatal(ff_mkparser, &ffp);
		fatal(ff_parse, ffp, g_args.fabfile, &ffn);

		// the first target is the default
		gn * def = 0;

		// process the graph
		for(x = 0; x < ffn->statements_l; x++)
		{
			if(ffn->statements[x]->type == FFN_DEPENDENCY)
			{
				// the number of edges is the cartesian product needs x feeds
				ff_node * n = ffn->statements[x]->needs;
				ff_node * f = ffn->statements[x]->feeds;

				for()	
				{
					for()
					{
						if(!def)
							fatal(gn_add, t->ff_dir, t->targets[x]->name, t->prereqs[y]->name, &def);
						else
							fatal(gn_add, t->ff_dir, t->targets[x]->name, t->prereqs[y]->name, 0);
					}
				}
			}
			else if(ffn->statements[x]->type == FFN_VARDECL)
			{
				
			}
			else if(ffn->statements[x]->type == FFN_FORMULA)
			{
				
			}
		}

		ff_node* t = ffn;
		while(t)
		{
			if(t->type == FFN_DEPENDENCY)
			{
				for(x = 0; x < t->targets_l; x++)
				{
					int y;
					for(y = 0; y < t->prereqs_l; y++)
					{
						if(!def)
							fatal(gn_add, t->ff_dir, t->targets[x]->name, t->prereqs[y]->name, &def);
						else
							fatal(gn_add, t->ff_dir, t->targets[x]->name, t->prereqs[y]->name, 0);
					}
				}
			}

			t = t->next;
		}

		// create formulas
		t = ffn;
		while(t)
		{
			if(t->type == FFN_FORMULA)
			{
				fml * fml = 0;
				fatal(fml_add, t, &fml);

				// attach to appropriate graph nodes
				for(x = 0; x < gn_nodes.l; x++)
				{
					int y;
					for(y = 0; y < t->targets_l; y++)
					{
						if(strcmp(gn_nodes.e[x]->dir, t->targets[y]->ff_dir) == 0)
						{
							if(strcmp(gn_nodes.e[x]->name, t->targets[y]->name) == 0)
							{
								gn_nodes.e[x]->fml = fml;
							}
						}
					}
				}
			}

			t = t->next;
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
