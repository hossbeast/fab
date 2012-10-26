#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "args.h"

#include "common/xmem.h"

struct g_args_t g_args;

static void usage()
{
	printf(
		"build\n"
		"Usage: build [options] target .. target\n"
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
// d  { "data-dir",							required_argument,	0,	'd' }
// e
/* f */  { "buildfile",						required_argument,	0,	'f' }
// g
// h,	{ "help",									no_argument,				0,	'h' }
// i
// j
// k
// l
// m
// n
// o
// p
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
		, { }
	};

	char* switches =
		// no-argument switches
		""	// "t"

		// with-argument switches
		"f:" // "d:i:m:p:q:r:u:v:w:"
	;

	// defaults
	g_args.buildfile = strdup(DEFAULT_BUILDFILE);

	int x, indexptr;
	while((x = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		switch(x)
		{
			// directories which are normalized to terminate with a slash
			case 'f':
				xfree(&g_args.buildfile);
				g_args.buildfile = strdup(optarg);
				break;
			case 'h':
				usage(1);
				break;
		}
	}

	for(x = optind; x < argc; x++)
	{
		g_args.targets = realloc(g_args.targets, sizeof(*g_args.targets) * (g_args.targets_len + 1));
		g_args.targets[g_args.targets_len] = strdup(argv[x]);
		g_args.targets_len++;
	}

	return 1;
}

void args_teardown()
{
	int x;
	for(x = 0; x < g_args.targets_len; x++)
		xfree(&g_args.targets[x]);

	xfree(&g_args.buildfile);
}
