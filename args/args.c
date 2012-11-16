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
// b
// c
// d
// e
/* f */   { "fabfile"						, required_argument	, 0			, 'f' }
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
/* v */  /* handled below because getopt sucks */											// graph node invalidation
// w
// x
// y
// z
		, { }
	};

	char* switches =
		// no-argument switches
		"p"

		// with-argument switches
		"f:"
	;

	// defaults
	g_args.pid						= getpid();
	g_args.mode						= MODE_DEFAULT;
	g_args.fabfile				= strdup(DEFAULT_FABFILE);
	g_args.execdir_base		= strdup(DEFAULT_EXECDIR);
	g_args.hashdir				= strdup(DEFAULT_HASHDIR);

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
		}
	}

	// unprocessed options - targets for fabrication
	for(x = optind; x < argc; x++)
	{
printf("notproc : %s\n", argv[x]);
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			g_args.targets = realloc(g_args.targets, sizeof(*g_args.targets) * (g_args.targets_len + 1));

			fatal(rpath, argv[x], &g_args.targets[g_args.targets_len]);
			g_args.targets_len++;
		}
		else if(strcmp(argv[x], "-v") == 0)
		{
			if(x < (argc - 1))
			{
				fatal(xrealloc, &g_args.invalid, sizeof(g_args.invalid[0]), g_args.invalid_len + 1, g_args.invalid_len);
				g_args.invalid[g_args.invalid_len] = strdup(argv[x+1]);
				g_args.invalid_len++;
			}
			else
			{
				printf("invalidate ALL\n");
			}
			break;
		}
	}

	// MODE_BUILDPLAN implies +BPDUMP
	if(g_args.mode == MODE_BUILDPLAN)
		log_parse("+BPDUMP", 0);

	// create execdir
	int l = snprintf(0, 0, "%s/%u", g_args.execdir_base, g_args.pid);
	g_args.execdir = malloc(l + 1);
	sprintf(g_args.execdir, "%s/%u", g_args.execdir_base, g_args.pid);

	// active logs
	char buf[256];
	log_active(buf, sizeof(buf));
	log(L_INFO, "%s", buf);

	// log cmdline args under args
	log(L_ARGS		, " %s (%c) fabfile            =%s", strcmp(g_args.fabfile, DEFAULT_FABFILE) == 0 ? " " : "*", 'f', g_args.fabfile);
	log(L_ARGS		, " %s (%c) mode               =%s", g_args.mode == MODE_DEFAULT ? " " : "*", 'p', MODE_STR(g_args.mode));
	log(L_ARGS		, " %s (%c) pid                =%u", " ", ' ', g_args.pid);
	log(L_ARGS		, " %s (%c) execdir-base       =%s", strcmp(g_args.execdir_base, DEFAULT_EXECDIR) == 0 ? " " : "*", ' ', g_args.execdir_base);
	log(L_ARGS		, " %s (%c) execdir            =%s", " ", ' ', g_args.execdir);
	log(L_ARGS		, " %s (%c) hashdir            =%s", strcmp(g_args.hashdir, DEFAULT_HASHDIR) == 0 ? " " : "*", ' ', g_args.hashdir);

	if(!g_args.invalid)
		log(L_ARGS	, " %s (%c) invalidate(s)      =", " ", 'v');
	for(x = 0; x < g_args.invalid_len; x++)
		log(L_ARGS	, " %s (%c) invalidate(s)      =%s", "*", 'v', g_args.invalid[x]);

	if(!g_args.targets)
		log(L_ARGS	, " %s (%c) target(s)          =", " ", ' ');
	for(x = 0; x < g_args.targets_len; x++)
		log(L_ARGS	, " %s (%c) target(s)          =%s", "*", ' ', g_args.targets[x]);

	log(L_ARGS		, "-------------------");

	return 1;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.targets_len; x++)
		free(g_args.targets[x]);

	free(g_args.fabfile);
	free(g_args.execdir_base);
	free(g_args.hashdir);
}
