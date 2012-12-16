#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "xstring.h"

struct g_args_t g_args;

static void usage()
{
	printf(
		"usage: fab [[options] [logopts] [product]]*\n"
		"   --help|-h for this message\n"
		"----------- [ product] ---------------------------\n"
		" 1. canonical path\n"
		" 2. path relative to initial fabfile\n"
		" 3. task names as single word or /../taskname\n"
		"----------- [ options ] --------------------------\n"
		" -f fabfile\n"
		" -p buildplan only\n"
		" -c node identifier is canonical path (default)\n"
		" -r node identifier is relative paths\n"
		" -B invalidate-all\n"
		" -b invalidate node\n"
		" -D dumpnode-all\n"
		" -d dump node\n"
		"----------- [ logopts ] --------------------------\n"
		" +<log name> to enable logging\n"  
		" -<log name> to disable logging\n"  
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		"  %-10s %s\n"
		,	"ERROR"   	, "errors leading to shutdown"
		,	"WARN"    	, "nonfatal warnings"
		,	"INFO"    	, "program flow"
		,	"ARGS" 			, "program arguments"
		,	"FFTOKN" 		, "fabfile parsing - token stream"
		,	"FFSTAT" 		, "fabfile parsing - lexer start condition change"
		,	"FFTREE"		, "fabfile parsing - parsed tree"
		,	"FF"				, "fabfile"
		,	"BPEXEC"		, "buildplan - execution"
		,	"BPEVAL"		, "buildplan - pruning/evaluating"
		,	"BPDUMP"		, "buildplan - dump the final buildplan"
		,	"BP"				, "buildplan"
		,	"FABEXEC"		, "fabrication formulas - execution results/details"
		,	"FABTARG"		, "fabrication formulas - target resolution/assignment"
		,	"FAB"				, "fabrication formulas"
		,	"DSCEXEC"		, "discovery formulas - execution results/details"
		,	"DSCTARG"		, "discovery formulas - target resolution/assignment"
		,	"DSC"				, "discovery formulas"
		,	"DGDEPS"		, "dependency graph - dependencies"
		,	"DGRAPH" 		, "dependency graph - dump/details"
		,	"DGHASH"		, "dependency graph - hash loading/saving"
		,	"DG" 				, "dependency graph"
		,	"VAR"				, "variable defintions"
		,	"LWDEBUG"		, "debug liblistwise invocations ** VERBOSE"
	);
	exit(0);
}

int parse_args(int argc, char** argv)
{
	struct option longopts[] = {
// a
/* b */   { "invalidate"				, required_argument	, 0			, 'v' }		// graph node invalidation
/* c */	, { "canon"							, no_argument				, 0			, 'c' }
/* d */	,	{ "dump"							, required_argument	, 0			, 'd' }		// graph node dump
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
/* r */	, { "relative"					, no_argument				, 0			, 'r' }
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
/* D */	, { "dump-all"					, no_argument				, 0			, 'D' }		// graph node dump
		, { }
	};

	char* switches =
		// no-argument switches
		"chprBD"

		// with-argument switches
		"f:b:d:"
	;

	// defaults
	g_args.pid						= getpid();
	g_args.mode						= DEFAULT_MODE;
	g_args.invalidate_all	= DEFAULT_INVALIDATE_ALL;
	g_args.fabfile				= strdup(DEFAULT_FABFILE);
	g_args.execdir_base		= strdup(DEFAULT_EXECDIR_BASE);
	g_args.hashdir				= strdup(DEFAULT_HASHDIR);
	g_args.mode_gnid			= DEFAULT_MODE_GNID;

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

			case 'c':
				g_args.mode_gnid = MODE_GNID_CANON;
				break;
			case 'r':
				g_args.mode_gnid = MODE_GNID_RELATIVE;
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
				g_args.invalidate[g_args.invalidate_len++] = strdup(optarg);
				break;
			case 'D':
				g_args.dumpnode_all = 1;
				break;
			case 'd':
				fatal(xrealloc, &g_args.dumpnode, sizeof(g_args.dumpnode[0]), g_args.dumpnode_len + 1, g_args.dumpnode_len);
				g_args.dumpnode[g_args.dumpnode_len++] = strdup(optarg);
				break;
		}
	}

	// canonicalize
	g_args.fabfile_canon = realpath(g_args.fabfile, 0);

	// unprocessed options - targets for fabrication
	for(x = optind; x < argc; x++)
	{
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targets_len + 1, g_args.targets_len);
			g_args.targets[g_args.targets_len++] = strdup(argv[x]);
		}
	}

	// dumpnode implies +DGRAPH
	if(g_args.dumpnode)
		log_parse("+DGRAPH", 0);

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
	log(L_ARGS		, " %s (%c) mode               =%s", g_args.mode == DEFAULT_MODE ? " " : "*", 'p', MODE_STR(g_args.mode));
	log(L_ARGS		, " %s (%c) pid                =%u", " ", ' ', g_args.pid);
	log(L_ARGS		, " %s (%c) cwd                =%s", " ", ' ', g_args.cwd);
	log(L_ARGS		, " %s (%c) execdir-base       =%s", strcmp(g_args.execdir_base, DEFAULT_EXECDIR_BASE) == 0 ? " " : "*", ' ', g_args.execdir_base);
	log(L_ARGS		, " %s (%c) execdir            =%s", " ", ' ', g_args.execdir);
	log(L_ARGS		, " %s (%c) hashdir            =%s", strcmp(g_args.hashdir, DEFAULT_HASHDIR) == 0 ? " " : "*", ' ', g_args.hashdir);
	log(L_ARGS		, " %s (%c) mode-gnid          =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", 'r', MODE_STR(g_args.mode_gnid));
	log(L_ARGS		, " %s (%c) invalidate-all     =%s", g_args.invalidate_all == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidate_all ? "yes" : "no");

	if(!g_args.invalidate_all)
	{
		if(!g_args.invalidate)
			log(L_ARGS	, " %s (%c) invalidate(s)      =", " ", 'b');
		for(x = 0; x < g_args.invalidate_len; x++)
			log(L_ARGS	, " %s (%c) invalidate(s)      =%s", "*", 'b', g_args.invalidate[x]);
	}

	log(L_ARGS		, " %s (%c) dumpnode-all       =%s", g_args.dumpnode_all == DEFAULT_DUMPNODE_ALL ? " " : "*", 'B', g_args.dumpnode_all ? "yes" : "no");

	if(!g_args.dumpnode_all)
	{
		if(!g_args.dumpnode)
			log(L_ARGS	, " %s (%c) dumpnode(s)        =", " ", 'd');
		for(x = 0; x < g_args.dumpnode_len; x++)         
			log(L_ARGS	, " %s (%c) dumpnode(s)        =%s", "*", 'd', g_args.dumpnode[x]);
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

	for(x = 0; x < g_args.dumpnode_len; x++)
		free(g_args.dumpnode[x]);

	free(g_args.targets);
	free(g_args.invalidate);
	free(g_args.dumpnode);
	free(g_args.fabfile);
	free(g_args.fabfile_canon);
	free(g_args.execdir_base);
	free(g_args.hashdir);
}
