/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

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
#include "macros.h"

struct g_args_t g_args;

static void usage(int valid, int version, int help, int logopts)
{
	printf(
"fab : parallel and incremental builds, integrated dependency discovery\n"
);
if(version)
{
	printf(" "
#if DBUG
	XQUOTE(FABVERSION) "+DBUG"
#else
	XQUOTE(FABVERSION)
#endif
	  "\n"
	);
}
if(help)
{
	printf(
"\n"
"usage : fab [[options] [logopts] [targets]]*\n"
"            --help|-h : this message\n"
"            --version : version information\n"
"            --logopts : logging category listing\n"
"\n"
"----------- [ targets ] --------------------------------------------------------\n"
"\n"
" <node specifier>        fabrication target\n"
"\n"
"----------- [ options ] --------------------------------------------------------\n"
"\n"
" execution modes\n"
" -p                      buildplan only\n"
" -u                      dependency discovery only\n"
" -d <node specifier>     dump only\n"
"\n"
" -U                      perform dependency discovery upfront\n"
"\n"
" incremental builds\n"
" -b <node specifier>     invalidate node(s)\n"
" -e <node specifier>     invalidate node(s) and add as fabrication target(s)\n"
" -B                      invalidate-all\n"
"\n"
" parallel builds\n"
" -j <number>             concurrency limit\n"
"\n"
" bakescript generation\n"
" -k </path/to/output>    create bakescript\n"
"\n"
" logging\n"
" -c                      set node identifier mode to GNID_CANON for logging\n"
"\n"
" fabfile processing\n"
" -f <path/to/fabfile>    path to initial fabfile\n"
" -I <path/to/directory>  directory for locating invocations\n"
" -v $var=[list]          scope-zero variable definition\n"
" -v $var+=[list]         scope-zero variable transform-addition\n"
" -v $var-=[list]         scope-zero variable transform-subtraction\n"
" -v $var=~generator      scope-zero variable transform-listwise\n"
"\n"
" handling cycles\n"
" --cycle-warn            warn when a cycle is detected (once per unique cycle)\n"
" --cycle-fail            fail when a cycle is detected\n"
" --cycle-deal            deal with cycles (halt traversal)\n"
"\n"
" <node specifier> is one of: \n"
"  1.  text   : path match relative to init-fabfile-rel-dir\n"
"  2. /text/  : regex match on can/abs/rel paths\n"
"  3. /text   : canonical path match\n"
"  4. @text   : nofile match\n"
"\n"
	);
}
if(logopts)
{
	printf(
"----------- [ logopts ] --------------------------------------------------------\n"
"\n"
" +<log name> to enable logging category\n"  
" -<log name> to disable logging category\n"  
"\n"
);

	int x;
	for(x = 0; x < g_logs_l; x++)
		printf("  %-10s %s\n", g_logs[x].s, g_logs[x].d);
	printf("\n");
}

if(help || logopts)
{
	printf(
"For more information visit http://fabutil.org\n"
	);
}
	printf("\n");
	exit(!valid);
}

int parse_args(int argc, char** argv)
{
	path * fabpath = 0;

	int help = 0;
	int logopts = 0;
	int version = 0;

	struct option longopts[] = {
/* informational */
				  { "help"				, no_argument	, &help, 1 }
				, { "logopts"			, no_argument	, &logopts, 1 }
				, { "version"			, no_argument	, &version, 1 }

/* program longopts */
				, { "cycle-warn"	, no_argument	, &g_args.mode_cycl, MODE_CYCL_WARN	}
				, { "cycle-fail"	, no_argument	, &g_args.mode_cycl, MODE_CYCL_FAIL }
				, { "cycle-deal"	, no_argument	, &g_args.mode_cycl, MODE_CYCL_DEAL }
// a
/* b */ , { 0	, required_argument	, 0			, 'b' }		// graph node invalidation
/* c */	, { 0	, no_argument				, 0			, 'c' }		// MODE_GNID_RELATIVE	
/* d */	,	{ 0	, required_argument	, 0			, 'd' }		// graph node dump
/* e */	,	{ 0 , required_argument	, 0			, 'e' }		// graph node invalidation
/* f */ , { 0	, required_argument	, 0			, 'f' }		// init-fabfile-path
// g
/* h */ , { 0	, no_argument				, 0			, 'h' }		// help
// i
/* j */ , { 0	, required_argument	, 0			, 'j' }		// concurrency limit
/* k */	, { 0	, required_argument	, 0			, 'k'	}		// bakescript output path
// l 
// m
// n
// o
/* p */	, { 0	, no_argument				, 0			, 'p' } 	// implies BPDUMP
// q
// r
// s
// t
/* u */	, { 0	, no_argument				, 0			, 'u' }		// dependency discovery mode
/* v */ , { 0	, required_argument	, 0			, 'v' }		// root-level variable definition
// w
// x
// y
// z
// A
/* B */ , { 0	, no_argument				, 0			, 'B' }		// graph node invalidation
// C
// D
// E
// F
// G
// H
/* I */	, { 0	, required_argument	, 0			, 'I' }		// directory to search for invocations
// J
// K
// L
// M
// N
// O
// P
// Q
// R
// S
// T
/* U */	, { 0	, no_argument				, 0			, 'U' }		// dependency discovery upfront
// V
// W
// X
// Y
// Z
				, { }
	};

	char* switches =
		// no-argument switches
		"chpuBIU"

		// with-argument switches
		"b:d:e:f:j:k:v:I:"
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
	g_args.mode_cycl			= DEFAULT_MODE_CYCL;
	g_args.invalidationsz	= DEFAULT_INVALIDATE_ALL;
	fatal(path_create, &fabpath, g_args.cwd, "%s", DEFAULT_INIT_FABFILE);
	fatal(path_copy, &g_args.init_fabfile_path, fabpath);

	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], DEFAULT_INVOKEDIR);

	int x;
	int indexptr;
	opterr = 1;
	while((x = getopt_long(argc, argv, switches, longopts, &indexptr)) != -1)
	{
		switch(x)
		{
			case 'h':
				usage(1, 1, 1, 0);
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
			case 'e':
				fatal(xrealloc, &g_args.invalidations, sizeof(g_args.invalidations[0]), g_args.invalidationsl + 1, g_args.invalidationsl);
				g_args.invalidations[g_args.invalidationsl++] = strdup(optarg);

				fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targetsl + 1, g_args.targetsl);
				g_args.targets[g_args.targetsl++] = strdup(optarg);
				break;
			case 'f':
				path_xfree(&g_args.init_fabfile_path);
				fatal(path_create, &g_args.init_fabfile_path, g_args.cwd, "%s", optarg);
				break;
			case 'p':
				g_args.mode_exec = MODE_EXEC_BUILDPLAN;
				break;
			case 'u':
				g_args.mode_exec = MODE_EXEC_DDSC;
				break;
			case 'k':
				g_args.mode_exec = MODE_EXEC_BAKE;
				xfree(&g_args.bakescript_path);
				g_args.bakescript_path = strdup(optarg);
				break;
			case 'v':
				{
					if(g_args.rootvarsl == g_args.rootvarsa)
					{
						int newa = g_args.rootvarsa ?: 3;
						newa = newa * 2 + newa / 2;
						fatal(xrealloc, &g_args.rootvars, sizeof(g_args.rootvars[0]), newa, g_args.rootvarsa);
						g_args.rootvarsa = newa;
					}

					fatal(xsprintf, &g_args.rootvars[g_args.rootvarsl], "%s%c", optarg, 0);
					g_args.rootvarsl++;
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
				g_args.mode_ddsc = MODE_DDSC_UPFRONT;
				break;
			case 'I':
				fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
				fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], optarg);
				break;
			case '?':
				usage(0, 1, 1, 0);
				break;
		}
	}

	if(help || logopts || version)
	{
		usage(1, 1, help, logopts);
	}

	fatal(xrealloc, &g_args.invokedirs, sizeof(g_args.invokedirs[0]), g_args.invokedirsl + 1, g_args.invokedirsl);
	fatal(xstrdup, &g_args.invokedirs[g_args.invokedirsl++], g_args.init_fabfile_path->abs_dir);

	// unprocessed options - fabrication targets
	for(x = optind; x < argc; x++)
	{
		if(argv[x][0] != '+' && argv[x][0] != '-')
		{
			fatal(xrealloc, &g_args.targets, sizeof(g_args.targets[0]), g_args.targetsl + 1, g_args.targetsl);
			g_args.targets[g_args.targetsl++] = strdup(argv[x]);
		}
	}

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
	log(L_PARAMS	, "%11spid                =%u"						, ""	, g_args.pid);
	log(L_PARAMS	, "%11sTid                =%u"						, ""	, g_args.sid);
	log(L_PARAMS	, "%11seid                =%s/%d:%s/%d"	, ""	, g_args.euid_name, g_args.euid, g_args.egid_name, g_args.egid);
	log(L_PARAMS	, "%11srid                =%s/%d:%s/%d"	, ""	, g_args.ruid_name, g_args.ruid, g_args.rgid_name, g_args.rgid);
	log(L_PARAMS	, "%11scwd                =%s"						, ""	, g_args.cwd);
	log(L_PARAMS	, "%11scache-dir-base     =%s"						, ""	, CACHEDIR_BASE);
	log(L_PARAMS	, "%11stmp-dir-base       =%s"						, ""	,	TMPDIR_BASE);
	log(L_PARAMS	, "%11sexpiration-policy  =%s"						, ""	, durationstring(EXPIRATION_POLICY));

	// log cmdline args under ARGS
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-can   =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->can);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-abs   =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->abs);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel   =%s", path_cmp(g_args.init_fabfile_path, fabpath) ? "*" : " ", 'f', g_args.init_fabfile_path->rel);
	log(L_ARGS | L_PARAMS				, " %s (%5s) mode-exec          =%s", g_args.mode_exec == DEFAULT_MODE_EXEC ? " " : "*", "k/p/u", MODE_STR(g_args.mode_exec));
	if(g_args.mode_exec == MODE_EXEC_BAKE)
	{
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) bakescript-path    =%s", strcmp(g_args.bakescript_path, DEFAULT_BAKE_PATH) == 0 ? " " : "*", 'k', g_args.bakescript_path);
	}
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-gnid          =%s", g_args.mode_gnid == DEFAULT_MODE_GNID ? " " : "*", 'c', MODE_STR(g_args.mode_gnid));
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-ddsc          =%s", g_args.mode_ddsc == DEFAULT_MODE_DDSC ? " " : "*", 'U', MODE_STR(g_args.mode_ddsc));
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-cycl          =%s", g_args.mode_cycl == DEFAULT_MODE_CYCL ? " " : "*", ' ', MODE_STR(g_args.mode_cycl));
	if(g_args.concurrency > 0)
		snprintf(buf, sizeof(buf)	, "%d", g_args.concurrency);
	else
		snprintf(buf, sizeof(buf)	, "%s", "unbounded");
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) concurrency        =%s", g_args.concurrency == DEFAULT_CONCURRENCY_LIMIT ? " " : "*", 'j', buf);
	log(L_ARGS | L_PARAMS				, " %s (  %c  ) invalidations-all  =%s", g_args.invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args.invalidationsz ? "yes" : "no");

	for(x = 0; x < g_args.invokedirsl; x++)
	{
		int star = x && x != (g_args.invokedirsl - 1);
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) invokedirs(s)      =%s", star ? "*" : " ", 'I', g_args.invokedirs[x]);
	}

	if(!g_args.invalidationsz)
	{
		if(!g_args.invalidations)
			log(L_ARGS | L_PARAMS		, " %s ( %s ) invalidations(s)   =", " ", "b/e");
		for(x = 0; x < g_args.invalidationsl; x++)
			log(L_ARGS | L_PARAMS		, " %s ( %s ) invalidations(s)   =%s", "*", "b/e", g_args.invalidations[x]);
	}

	if(!g_args.dumpnodesz)
	{
		if(!g_args.dumpnodes)
			log(L_ARGS | L_PARAMS		, " %s (  %c  ) dumpnodes(s)       =", " ", 'd');
		for(x = 0; x < g_args.dumpnodesl; x++)         
			log(L_ARGS | L_PARAMS		, " %s (  %c  ) dumpnodes(s)       =%s", "*", 'd', g_args.dumpnodes[x]);
	}

	if(!g_args.rootvarsl)
		log(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)     =", " ", ' ');
	for(x = 0; x < g_args.rootvarsl; x++)
		log(L_ARGS | L_PARAMS 		, " %s (  %c  ) scope-0-var(s)     =%s", "*", 'v', g_args.rootvars[x]);

	if(!g_args.targets)
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) target(s)          =", " ", ' ');
	for(x = 0; x < g_args.targetsl; x++)
		log(L_ARGS | L_PARAMS			, " %s (  %c  ) target(s)          =%s", "*", ' ', g_args.targets[x]);

	log(L_ARGS | L_PARAMS, "---------------------------------------------------");

finally:
	path_free(fabpath);
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

	for(x = 0; x < g_args.rootvarsl; x++)
	{
		free(g_args.rootvars[x]);
	}
	free(g_args.rootvars);

	for(x = 0; x < g_args.invokedirsl; x++)
		free(g_args.invokedirs[x]);

	free(g_args.targets);
	free(g_args.invalidations);
	free(g_args.dumpnodes);
	free(g_args.invokedirs);

	path_free(g_args.init_fabfile_path);
	free(g_args.bakescript_path);
}

