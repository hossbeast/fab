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
		" -c node identifier is canonical path\n"
		" -B invalidate-all\n"
		" -b invalidate node\n"
		" -d dump node\n"
		"----------- [ logopts ] --------------------------\n"
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
	char * fabfile = 0;

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
/* u */	, { "upfront"						, no_argument				, 0			, 'u' }		// dependency discovery upfront
// v
// w
// x
// y
// z
// A
/* B */ , { "invalidate-all"		, no_argument				, 0			, 'B' }		// graph node invalidation
// C
/* U */	, { "discovery"					, no_argument				, 0			, 'U' }		// dependency discovery mode
		, { }
	};

	char* switches =
		// no-argument switches
		"chpuBU"

		// with-argument switches
		"b:d:f:"
	;

	//
	// parameters
	//
	g_args.pid						= getpid();
	g_args.sid						= getsid(0);

	// current working directory, canonicalized
	char space[512];
	getcwd(space, sizeof(space));
	g_args.cwd = realpath(space, 0);

	//
	// args:defaults
	//
	g_args.mode_exec			= DEFAULT_MODE_EXEC;
	g_args.mode_gnid			= DEFAULT_MODE_GNID;
	g_args.mode_ddsc			= DEFAULT_MODE_DDSC;
	g_args.invalidate_all	= DEFAULT_INVALIDATE_ALL;
	fabfile								= strdup(DEFAULT_FABFILE);

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
				fatal(xrealloc, &g_args.invalidate, sizeof(g_args.invalidate[0]), g_args.invalidate_len + 1, g_args.invalidate_len);
				g_args.invalidate[g_args.invalidate_len++] = strdup(optarg);
				break;
			case 'c':
				g_args.mode_gnid = MODE_GNID_CANON;
				break;
			case 'd':
				fatal(xrealloc, &g_args.dumpnode, sizeof(g_args.dumpnode[0]), g_args.dumpnode_len + 1, g_args.dumpnode_len);
				g_args.dumpnode[g_args.dumpnode_len++] = strdup(optarg);
				break;
			case 'f':
				xfree(&fabfile);
				fabfile = strdup(optarg);
				break;
			case 'p':
				g_args.mode_exec = MODE_EXEC_BUILDPLAN;
				break;
			case 'u':
				g_args.mode_ddsc = MODE_DDSC_UPFRONT;
				break;
			case 'B':
				g_args.invalidate_all = 1;
				break;
			case 'U':
				g_args.mode_exec = MODE_EXEC_DDSC;
				break;
		}
	}

	// canonicalize
	g_args.fabfile_canon = realpath(fabfile, 0);

	// terminate at the final slash
	g_args.fabfile_canon_dir = strdup(g_args.fabfile_canon);
	
	char* s = g_args.fabfile_canon_dir + strlen(g_args.fabfile_canon_dir);
	while(s[0] != '/')
		s--;
	s[0] = 0;

	// unprocessed options - fabrication targets
	for(x = optind; x < argc; x++)
	{
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targets_len + 1, g_args.targets_len);
			g_args.targets[g_args.targets_len++] = strdup(argv[x]);
		}
	}

	// dumpnode implies +DGRAPH and MODE_EXEC_DUMP
	if(g_args.dumpnode)
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
	log(L_PARAMS	, "%7ssid-dir-pat        =%s"						, ""	, SID_DIR_BASE);
	log(L_PARAMS	, "%7sgn-dir-pat         =%s"						, ""	, GN_DIR_BASE);
	log(L_PARAMS	, "%7spid-dir-pat        =%s"						, ""	, PID_DIR_BASE);
	log(L_PARAMS	, "%7sexpiration-policy  =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	snprintf(space, sizeof(space), "%s/%s", g_args.cwd, DEFAULT_FABFILE);

	log(L_ARGS | L_PARAMS		, " %s (%c) fabfile-canon      =%s", strcmp(g_args.fabfile_canon, space) == 0 ? " " : "*", 'f', g_args.fabfile_canon);
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-exec          =%s", g_args.mode_exec == DEFAULT_MODE_EXEC ? " " : "*", 'p', MODE_STR(g_args.mode_exec));
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-gnid          =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", 'r', MODE_STR(g_args.mode_gnid));
	log(L_ARGS | L_PARAMS		, " %s (%c) mode-ddsc          =%s", g_args.mode_ddsc == DEFAULT_MODE_DDSC ? " " : "*", 'u', MODE_STR(g_args.mode_ddsc));
	log(L_ARGS | L_PARAMS		, " %s (%c) invalidate-all     =%s", g_args.invalidate_all == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidate_all ? "yes" : "no");

	if(!g_args.invalidate_all)
	{
		if(!g_args.invalidate)
			log(L_ARGS | L_PARAMS	, " %s (%c) invalidate(s)      =", " ", 'b');
		for(x = 0; x < g_args.invalidate_len; x++)
			log(L_ARGS | L_PARAMS	, " %s (%c) invalidate(s)      =%s", "*", 'b', g_args.invalidate[x]);
	}

	if(!g_args.dumpnode_all)
	{
		if(!g_args.dumpnode)
			log(L_ARGS | L_PARAMS	, " %s (%c) dumpnode(s)        =", " ", 'd');
		for(x = 0; x < g_args.dumpnode_len; x++)         
			log(L_ARGS | L_PARAMS	, " %s (%c) dumpnode(s)        =%s", "*", 'd', g_args.dumpnode[x]);
	}

	if(!g_args.targets)
		log(L_ARGS | L_PARAMS	, " %s (%c) target(s)          =", " ", ' ');
	for(x = 0; x < g_args.targets_len; x++)
		log(L_ARGS | L_PARAMS	, " %s (%c) target(s)          =%s", "*", ' ', g_args.targets[x]);
	log(L_ARGS | L_PARAMS, "---------------------------------------------------");

finally:
	free(fabfile);
coda;
}

void args_teardown()
{
	free(g_args.cwd);
	free(g_args.ruid_name);
	free(g_args.euid_name);
	free(g_args.rgid_name);
	free(g_args.egid_name);

	int x;
	for(x = 0; x < g_args.targets_len; x++)
		free(g_args.targets[x]);

	for(x = 0; x < g_args.invalidate_len; x++)
		free(g_args.invalidate[x]);

	for(x = 0; x < g_args.dumpnode_len; x++)
		free(g_args.dumpnode[x]);

	free(g_args.targets);
	free(g_args.fabfile_canon);
	free(g_args.fabfile_canon_dir);
	free(g_args.invalidate);
	free(g_args.dumpnode);
}
