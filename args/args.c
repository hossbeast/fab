#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "xstring.h"
#include "rpath.h"

struct g_args_t g_args;

static void usage()
{
	printf(
		"fab\n"
		"Usage: fab [options] target .. target\n"
		"   --help|-h for this message\n"
		"\n"
	);
}

int parse_args(int argc, char** argv)
{
	struct option longopts[] = {
// a
/* b */   { "invalidate"				, required_argument	, 0			, 'v' }		// graph node invalidation
// c
// d
// e
/* f */ , { "fabfile"						, required_argument	, 0			, 'f' }
// g
/* h */
// i
// j
// k
// l 
// m
// n
// o
/* p */	, { "buildplan"					, no_argument				, 0			, 'p' } 	// implies BPDUMP
// q
// r
// s
// t
// u
// v
// w
// x
// y
// z
// A
/* B */ , { "invalidate-all"		, no_argument				, 0			, 'B' }		// graph node invalidation
// C
		, { }
	};

	char* switches =
		// no-argument switches
		"pB"

		// with-argument switches
		"f:b:"
	;

	// defaults
	g_args.pid						= getpid();
	g_args.mode						= MODE_DEFAULT;
	g_args.invalidate_all	= DEFAULT_INVALIDATE_ALL;
	g_args.fabfile				= strdup(DEFAULT_FABFILE);
	g_args.execdir_base		= strdup(DEFAULT_EXECDIR_BASE);
	g_args.hashdir				= strdup(DEFAULT_HASHDIR);

	// working directory
	getcwd(g_args.cwd, sizeof(g_args.cwd));
	g_args.cwdl = strlen(g_args.cwd);

	int x, indexptr;
	while((x = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		switch(x)
		{
			// directories which are normalized to terminate with a slash
			case 'h':
				usage(1);
				break;
			case 'f':
				xfree(&g_args.fabfile);
				g_args.fabfile = strdup(optarg);
				break;
			case 'p':
				g_args.mode = MODE_BUILDPLAN;
				break;
			case 'B':
				g_args.invalidate_all = 1;
				break;
			case 'b':
				fatal(xrealloc, &g_args.invalidate, sizeof(g_args.invalidate[0]), g_args.invalidate_len + 1, g_args.invalidate_len);
				if(optarg[0] == '/')
				{
					g_args.invalidate[g_args.invalidate_len] = strdup(optarg);
				}
				else
				{
					// path is relative to current working directory
					fatal(xmalloc, &g_args.invalidate[g_args.invalidate_len], g_args.cwdl + 1 + strlen(optarg) + 1);
					sprintf(g_args.invalidate[g_args.invalidate_len], "%s/%s", g_args.cwd, optarg);
				}
				g_args.invalidate_len++;
				break;
		}
	}

	// unprocessed options - targets for fabrication
	for(x = optind; x < argc; x++)
	{
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			g_args.targets = realloc(g_args.targets, sizeof(*g_args.targets) * (g_args.targets_len + 1));
			fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targets_len + 1, g_args.targets_len);
			if(argv[x][0] == '/')
			{
				g_args.targets[g_args.targets_len] = strdup(argv[x]);
			}
			else
			{
				// path is relative to current working directory
				fatal(xmalloc, &g_args.targets[g_args.targets_len], g_args.cwdl + 1 + strlen(argv[x]) + 1);
				sprintf(g_args.targets[g_args.targets_len], "%s/%s", g_args.cwd, argv[x]);
			}
			g_args.targets_len++;
		}
	}

	// MODE_BUILDPLAN implies +BPDUMP
	if(g_args.mode == MODE_BUILDPLAN)
		log_parse("+BPDUMP", 0);

	// create execdir
	int l = snprintf(0, 0, "%s/%u", g_args.execdir_base, g_args.pid);
	fatal(xmalloc, &g_args.execdir, l + 1);
	sprintf(g_args.execdir, "%s/%u", g_args.execdir_base, g_args.pid);

	// active logs
	char buf[256];
	log_active(buf, sizeof(buf));
	log(L_INFO, "%s", buf);

	// log cmdline args under args
	log(L_ARGS		, "---------------------------------------------------");
	log(L_ARGS		, " %s (%c) fabfile            =%s", strcmp(g_args.fabfile, DEFAULT_FABFILE) == 0 ? " " : "*", 'f', g_args.fabfile);
	log(L_ARGS		, " %s (%c) mode               =%s", g_args.mode == MODE_DEFAULT ? " " : "*", 'p', MODE_STR(g_args.mode));
	log(L_ARGS		, " %s (%c) pid                =%u", " ", ' ', g_args.pid);
	log(L_ARGS		, " %s (%c) cwd                =%s", " ", ' ', g_args.cwd);
	log(L_ARGS		, " %s (%c) execdir-base       =%s", strcmp(g_args.execdir_base, DEFAULT_EXECDIR_BASE) == 0 ? " " : "*", ' ', g_args.execdir_base);
	log(L_ARGS		, " %s (%c) execdir            =%s", " ", ' ', g_args.execdir);
	log(L_ARGS		, " %s (%c) hashdir            =%s", strcmp(g_args.hashdir, DEFAULT_HASHDIR) == 0 ? " " : "*", ' ', g_args.hashdir);
	log(L_ARGS		, " %s (%c) invalidate-all     =%s", g_args.invalidate_all == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidate_all ? "yes" : "no");

	if(!g_args.invalidate_all)
	{
		if(!g_args.invalidate)
			log(L_ARGS	, " %s (%c) invalidate(s)      =", " ", 'b');
		for(x = 0; x < g_args.invalidate_len; x++)
			log(L_ARGS	, " %s (%c) invalidate(s)      =%s", "*", 'b', g_args.invalidate[x]);
	}

	if(!g_args.targets)
		log(L_ARGS	, " %s (%c) target(s)          =", " ", ' ');
	for(x = 0; x < g_args.targets_len; x++)
		log(L_ARGS	, " %s (%c) target(s)          =%s", "*", ' ', g_args.targets[x]);
	log(L_ARGS		, "---------------------------------------------------");

	return 1;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.targets_len; x++)
		free(g_args.targets[x]);

	for(x = 0; x < g_args.invalidate_len; x++)
		free(g_args.invalidate[x]);

	free(g_args.targets);
	free(g_args.invalidate);
	free(g_args.fabfile);
	free(g_args.execdir_base);
	free(g_args.hashdir);
}
