#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "args.h"
#include "bf.h"
#include "gn.h"
#include "fml.h"
#include "bp.h"

#include "control.h"

// signal handling
static int o_signum;
void signal_handler(int signum)
{
	printf("shutdown signal: %d\n", signum);

	if(!o_signum)
	{
		o_signum = signum;
	}
}

int main(int argc, char** argv)
{
	int domain()
	{
		bf_parser *		bfp = 0;
		bf_node *			bfn = 0;
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

		// parse cmdline arguments
		fatal(parse_args, argc, argv);

		printf("parameters\n");
		printf(  " %s (%c) buildfile          =%s\n", strcmp(g_args.buildfile, DEFAULT_BUILDFILE) == 0 ? " " : "*", 'f', g_args.buildfile);

		if(!g_args.targets)
			printf(" %s (%c) goal               =\n", " ", ' ');
		for(x = 0; x < g_args.targets_len; x++)
			printf(" %s (%c) goal               =%s\n", "*", ' ', g_args.targets[x]);

		printf("-------------------\n");

		// parse the buildfile
		fatal(bf_mkparser, &bfp);
		fatal(bf_parse, bfp, g_args.buildfile, &bfn);

		// add each dependency to the graph
		bf_node* t = bfn;
		while(t)
		{
			if(t->type == BFN_DEPENDENCY)
			{
				for(x = 0; x < t->targets_l; x++)
				{
					int y;
					for(y = 0; y < t->prereqs_l; y++)
					{
						fatal(gn_add, t->bf_dir, t->targets[x]->name, t->prereqs[y]->name);
					}
				}
			}

			t = t->next;
		}

		// create formulas
		t = bfn;
		while(t)
		{
			if(t->type == BFN_FORMULA)
			{
				fml * fml = 0;
				fatal(fml_add, t, &fml);

				// attach to appropriate graph nodes
				for(x = 0; x < gn_nodes.l; x++)
				{
					int y;
					for(y = 0; y < t->targets_l; y++)
					{
printf("cmp(%s, %s) = %d\n"
	, gn_nodes.e[x]->dir
	, t->targets[y]->bf_dir
	, strcmp(gn_nodes.e[x]->dir, t->targets[y]->bf_dir)
);

						if(strcmp(gn_nodes.e[x]->dir, t->targets[y]->bf_dir) == 0)
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

		gn_dump(gn_root);

		// traverse the graph, construct the build plan
		fatal(bp_create, gn_root, &bp);

		bp_dump(bp);

		// execute the build plan, one stage at a time

		return 1;
	};

	int R = !domain() || o_signum;
	printf("exiting with status: %d\n", R);
	
	return R;
}
