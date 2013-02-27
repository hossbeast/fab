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
#include "unitstring.h"
#include "canon.h"

struct g_args_t g_args;

static void usage()
{
	printf(
"usage: fab [[options] [logopts] [product]]*\n"
"   --help|-h for this message\n"
"\n"
" <node specifier> may be: \n"
"  1.  text   : append to init-fabfile-rel-dir, relative path match\n"
"  2. /text/  : regex match on abs/can/rel paths\n"
"  3. <text>  : canonical path match\n"
"  4. .text   : canonical path match with nofile notation\n"
"\n"
"----------- [ product] ---------------------------------------------------------\n"
"\n"
" <node specifier>        fabrication target\n"
"\n"
"----------- [ options ] --------------------------------------------------------\n"
"\n"
" -b <node specifier>     invalidate node(s)\n"
" -B                      invalidate-all\n"
" -c                      set node identifier mode to GNID_CANON for logging\n"
" -d <node specifier>     dump node(s)\n"
" -f <path/to/fabfile>    path to initial fabfile\n"
" -I <path/to/directory>  root directory for locating invocations\n"
" -j <number>             concurrency limit\n"
" -p                      create buildplan only\n"
" -v <key=value>          provide sticky variable definition as key=value\n"
"\n"
"----------- [ logopts ] --------------------------------------------------------\n"
"\n"
" +<log name> to enable logging\n"  
" -<log name> to disable logging\n"  
	);

	int x;
	for(x = 0; x < g_logs_l; x++)
		printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);

	exit(0);
}

int parse_args(int argc, char** argv)
{
	char * fabpath = 0;

	struct option longopts[] = {
// a
/* b */   { "invalidate"				, required_argument	, 0			, 'v' }		// graph node invalidation
/* c */	, { "canon"							, no_argument				, 0			, 'c' }
/* d */	,	{ "dump"							, required_argument	, 0			, 'd' }		// graph node dump
// e
/* f */ , { "fabfile"						, required_argument	, 0			, 'f' }
// g
/* h */ , { "help"							, no_argument				, 0			, 'h' }
// i
/* j */ , { "concurrency"				, required_argument	, 0			, 'j' }
/* k */	, { "bakescript"				, required_argument	, 0			, 'k'	}		// bakescript output path
// l 
// m
// n
// o
/* p */	, { "buildplan"					, no_argument				, 0			, 'p' } 	// implies BPDUMP
// q
// r
// s
// t
/* u */	, { "upfront"						, no_argument				, 0			, 'u' }		// dependency discovery upfront
/* v */
// w
// x
// y
// z
// A
/* B */ , { "invalidate-all"		, no_argument				, 0			, 'B' }		// graph node invalidation
// C
/* U */	, { "discovery"					, no_argument				, 0			, 'U' }		// dependency discovery mode
/* I */	, { "invokedir"					, no_argument				, 0			, 'I' }		// directory to search for invocations
		, { }
	};

	char* switches =
		// no-argument switches
		"chpuBU"

		// with-argument switches
		"b:d:f:j:k:v:I:"
	;

	//
	// parameters
	//
	g_args.pid						= getpid();
	g_args.sid						= getsid(0);
	g_args.cwd						= getcwd(0, 0);

	//
	// args:defaults
	//
	g_args.concurrency		= DEFAULT_CONCURRENCY_LIMIT;
	g_args.mode_exec			= DEFAULT_MODE_EXEC;
	g_args.mode_gnid			= DEFAULT_MODE_GNID;
	g_args.mode_ddsc			= DEFAULT_MODE_DDSC;
	g_args.invalidationsz	= DEFAULT_INVALIDATE_ALL;
	fatal(xstrdup, &fabpath, DEFAULT_INIT_FABFILE);
	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], DEFAULT_INVOKEDIR);

	int x, indexptr;
	while((x = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		switch(x)
		{
			// directories which are normalized to terminate with a slash
			case 'h':
				usage(1);
				break;
			case 'b':
				fatal(xrealloc, &g_args.invalidations, sizeof(g_args.invalidations[0]), g_args.invalidationsl + 1, g_args.invalidationsl);
				g_args.invalidations[g_args.invalidationsl++] = strdup(optarg);
				break;
			case 'c':
				g_args.mode_gnid = MODE_GNID_CANON;
				break;
			case 'd':
				fatal(xrealloc, &g_args.dumpnodes, sizeof(g_args.dumpnodes[0]), g_args.dumpnodesl + 1, g_args.dumpnodesl);
				g_args.dumpnodes[g_args.dumpnodesl++] = strdup(optarg);
				break;
			case 'f':
				xfree(&fabpath);
				fabpath = strdup(optarg);
				break;
			case 'p':
				g_args.mode_exec = MODE_EXEC_BUILDPLAN;
				break;
			case 'u':
				g_args.mode_ddsc = MODE_DDSC_UPFRONT;
				break;
			case 'k':
				g_args.mode_exec = MODE_EXEC_BAKE;
				xfree(&g_args.bakescript_path);
				g_args.bakescript_path = strdup(optarg);
				break;
			case 'v':
				{
					char * eq;
					if((eq = strstr(optarg, "=")))
					{
						fatal(xrealloc, &g_args.varkeys, sizeof(g_args.varkeys[0]), g_args.varkeysl + 1, g_args.varkeysl);
						fatal(xrealloc, &g_args.varvals, sizeof(g_args.varvals[0]), g_args.varvalsl + 1, g_args.varvalsl);

						*eq = 0;
						g_args.varkeys[g_args.varkeysl++] = strdup(optarg);
						*eq = '=';
						g_args.varvals[g_args.varvalsl++] = strdup(eq+1);
					}
					else
					{
						fail("badly formed option for -v : '%s'", optarg);
					}
				}
				break;
			case 'j':
				{
					int n = 0;
					if(sscanf(optarg, "%d%n", &g_args.concurrency, &n) != 1 || n <= 0)
					{
						fail("badly formed option for -j : '%s'", optarg);
					}
				}
				break;
			case 'B':
				g_args.invalidationsz = 1;
				break;
			case 'U':
				g_args.mode_exec = MODE_EXEC_DDSC;
				break;
			case 'I':
				fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
				g_args.invokedirs[g_args.invokedirsl++] = strdup(optarg);
				break;
		}
	}

	// unprocessed options - fabrication targets
	for(x = optind; x < argc; x++)
	{
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targetsl + 1, g_args.targetsl);
			g_args.targets[g_args.targetsl++] = strdup(argv[x]);
		}
	}

	// init fabfile path
	fatal(path_create, &g_args.init_fabfile_path, g_args.cwd, "%s", fabpath);

	// lookup invalidations
	for(x = 0; x < g_args.invalidationsl; x++)
	{
		char * N = malloc(512);
		fatal(canon, g_args.invalidations[x], 0, N, 512, g_args.cwd, CAN_REALPATH);
		free(g_args.invalidations[x]);
		g_args.invalidations[x] = N;
	}

	// dumpnodes implies +DGRAPH and MODE_EXEC_DUMP
	if(g_args.dumpnodes)
	{
		g_args.mode_exec = MODE_EXEC_DUMP;
		log_parse("+DGRAPH", 0);
	}

	// MODE_BUILDPLAN implies +BPDUMP
	if(g_args.mode_exec == MODE_EXEC_BUILDPLAN)
		log_parse("+BPDUMP", 0);

	// initialize logger
	fatal(log_init, "+ERROR|WARN|INFO|BPEXEC|DSCINFO");

	// active logs
	char buf[256];
	log_active(buf, sizeof(buf));
	log(L_INFO, "%s", buf);

	log(L_ARGS | L_PARAMS, "---------------------------------------------------");

	// log execution parameters under PARAMS
	log(L_PARAMS	, "%7spid                =%u"						, ""	, g_args.pid);
	log(L_PARAMS	, "%7ssid                =%u"						, ""	, g_args.sid);
	log(L_PARAMS	, "%7seid                =%s/%d:%s/%d"	, ""	, g_args.euid_name, g_args.euid, g_args.egid_name, g_args.egid);
	log(L_PARAMS	, "%7srid                =%s/%d:%s/%d"	, ""	, g_args.ruid_name, g_args.ruid, g_args.rgid_name, g_args.rgid);
	log(L_PARAMS	, "%7scwd                =%s"						, ""	, g_args.cwd);
	log(L_PARAMS	, "%7ssid-dir-base       =%s"						, ""	, SID_DIR_BASE);
	log(L_PARAMS	, "%7sgn-dir-base        =%s"						, ""	, GN_DIR_BASE);
	log(L_PARAMS	, "%7spid-dir-base       =%s"						, ""	, PID_DIR_BASE);
	log(L_PARAMS	, "%7sexpiration-policy  =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	log(L_ARGS | L_PARAMS		, " %s (%c) init-fabfile-can   =%s", "*", 'f', g_args.init_fabfile_path->can);
	log(L_ARGS | L_PARAMS		, " %s (%c) init-fabfile-abs   =%s", "*", 'f', g_args.init_fabfile_path->abs);
	log(L_ARGS | L_PARAMS		, " %s (%c) init-fabfile-rel   =%s", "*", 'f', g_args.init_fabfile_path->rel);
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-exec          =%s", g_args.mode_exec == DEFAULT_MODE_EXEC ? " " : "*", 'p', MODE_STR(g_args.mode_exec));
	if(g_args.mode_exec == MODE_EXEC_BAKE)
	{
		log(L_ARGS | L_PARAMS		, " %s (%c) bakescript-path    =%s", strcmp(g_args.bakescript_path, DEFAULT_BAKE_PATH) == 0 ? " " : "*", 'k', g_args.bakescript_path);
	}
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-gnid          =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", 'r', MODE_STR(g_args.mode_gnid));
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-ddsc          =%s", g_args.mode_ddsc == DEFAULT_MODE_DDSC ? " " : "*", 'u', MODE_STR(g_args.mode_ddsc));
	if(g_args.concurrency > 0)
		snprintf(buf, sizeof(buf), "%d", g_args.concurrency);
	else
		snprintf(buf, sizeof(buf), "%s", "unbounded");
	log(L_ARGS | L_PARAMS		, " %s (%c) concurrency        =%s", g_args.concurrency == DEFAULT_CONCURRENCY_LIMIT ? " " : "*", 'j', buf);
	log(L_ARGS | L_PARAMS		, " %s (%c) invalidations-all  =%s", g_args.invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidationsz ? "yes" : "no");

	for(x = 0; x < g_args.invokedirsl; x++)
		log(L_ARGS | L_PARAMS	, " %s (%c) invokedirs(s)      =%s", "*", 'I', g_args.invokedirs[x]);

	if(!g_args.invalidationsz)
	{
		if(!g_args.invalidations)
			log(L_ARGS | L_PARAMS	, " %s (%c) invalidations(s) =", " ", 'b');
		for(x = 0; x < g_args.invalidationsl; x++)
			log(L_ARGS | L_PARAMS	, " %s (%c) invalidations(s) =%s", "*", 'b', g_args.invalidations[x]);
	}

	if(!g_args.dumpnodesz)
	{
		if(!g_args.dumpnodes)
			log(L_ARGS | L_PARAMS	, " %s (%c) dumpnodes(s)     =", " ", 'd');
		for(x = 0; x < g_args.dumpnodesl; x++)         
			log(L_ARGS | L_PARAMS	, " %s (%c) dumpnodes(s)     =%s", "*", 'd', g_args.dumpnodes[x]);
	}

	if(!g_args.targets)
		log(L_ARGS | L_PARAMS	, " %s (%c) target(s)          =", " ", ' ');
	for(x = 0; x < g_args.targetsl; x++)
		log(L_ARGS | L_PARAMS	, " %s (%c) target(s)          =%s", "*", ' ', g_args.targets[x]);

	if(!g_args.varkeys)
		log(L_ARGS | L_PARAMS , " %s (%c) var(s)             =", " ", ' ');
	for(x = 0; x < g_args.varkeysl; x++)
		log(L_ARGS | L_PARAMS , " %s (%c) var(s)             =%s=%s", "*", 'v', g_args.varkeys[x], g_args.varvals[x]);

	log(L_ARGS | L_PARAMS, "---------------------------------------------------");

finally:
	free(fabpath);
coda;
}

void args_teardown()
{
	free(g_args.ruid_name);
	free(g_args.euid_name);
	free(g_args.rgid_name);
	free(g_args.egid_name);
	free(g_args.cwd);

	int x;
	for(x = 0; x < g_args.targetsl; x++)
		free(g_args.targets[x]);

	for(x = 0; x < g_args.invalidationsl; x++)
		free(g_args.invalidations[x]);

	for(x = 0; x < g_args.dumpnodesl; x++)
		free(g_args.dumpnodes[x]);

	for(x = 0; x < g_args.varkeysl; x++)
		free(g_args.varkeys[x]);

	for(x = 0; x < g_args.varvalsl; x++)
		free(g_args.varvals[x]);

	for(x = 0; x < g_args.invokedirsl; x++)
		free(g_args.invokedirs[x]);

	free(g_args.targets);
	free(g_args.invalidations);
	free(g_args.dumpnodes);
	free(g_args.varkeys);
	free(g_args.varvals);
	free(g_args.invokedirs);

	path_free(g_args.init_fabfile_path);
	free(g_args.bakescript_path);
}
