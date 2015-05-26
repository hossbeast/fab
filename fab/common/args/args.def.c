/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include <stdio.h>

#include "xapi.h"

#include "args.def.h"

#include "memblk.h"
#include "memblk.def.h"
#include "selector.def.h"
#include "path.h"
#include "params.h"
#include "logs.h"

#include "macros.h"

#define restrict __restrict

struct g_args_t * g_args;

//
// public
//

int args_summarize()
{
	path * init_fabpath = 0;

	char space[512];
	int x;

	fatal(path_create_init, &init_fabpath, g_params.cwd, "%s", DEFAULT_INIT_FABFILE);

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

	// log execution parameters under PARAMS
	logf(L_PARAMS	, "%11spid                    =%u"						, ""	, g_params.pid);
	logf(L_PARAMS	, "%11seid                    =%s/%d:%s/%d"		, ""	, g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid);
	logf(L_PARAMS	, "%11srid                    =%s/%d:%s/%d"		, ""	, g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid);
	logf(L_PARAMS	, "%11scwd                    =%s"						, ""	, g_params.cwd);
	logf(L_PARAMS	, "%11sexedir                 =%s"						, ""	, g_params.exedir);
	logf(L_PARAMS	, "%11sprocessors             =%ld"						, ""	, g_params.procs);
	logf(L_PARAMS	, "%11stmpdir                 =%s"						, ""	, XQUOTE(FABTMPDIR));
	logf(L_PARAMS	, "%11sipcdir                 =%s"						, ""	, XQUOTE(FABIPCDIR));
	logf(L_PARAMS	, "%11slwopdir                =%s"						, ""	, XQUOTE(FABLWOPDIR));
	logf(L_PARAMS	, "%11sinvokedir              =%s"						, ""	, XQUOTE(FABINVOKEDIR));

	// log cmdline args under ARGS
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-can       =%s", path_cmp(g_args->init_fabfile_path, init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->can);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-hash      =%u", path_cmp(g_args->init_fabfile_path, init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->can_hash);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-abs       =%s", path_cmp(g_args->init_fabfile_path, init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->abs);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-cwd   =%s", path_cmp(g_args->init_fabfile_path, init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->rel_cwd);
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) init-fabfile-rel-fab   =%s", path_cmp(g_args->init_fabfile_path, init_fabpath) ? "*" : " ", 'f', g_args->init_fabfile_path->rel_fab);
	logf(L_ARGS | L_PARAMS				, " %s (%5s) mode-bplan             =%s", g_args->mode_bplan == DEFAULT_MODE_BPLAN ? " " : "*", "k/p", MODE_STR(g_args->mode_bplan));

	if(g_args->mode_bplan == MODE_BPLAN_BUILDSCRIPT)
	{
		logf(L_ARGS | L_PARAMS				, " %s (  %c  ) buildscript-path       =%s", "*", 'k', g_args->buildscript_path);

		if(g_args->bs_runtime_varsl == 0)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bs-runtime-var(s)      =", " ", 'K');
		for(x = 0; x < g_args->bs_runtime_varsl; x++)
			logf(L_ARGS | L_PARAMS 		, " %s (  %c  ) bs-runtime-var(s)      =%s", "*", 'K', g_args->bs_runtime_vars[x]);
	}

#if DEBUG || DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-backtrace         =%s", g_args->mode_backtrace == DEFAULT_MODE_BACKTRACE ? " " : "*", ' ', MODE_STR(g_args->mode_backtrace));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-logtrace          =%s", g_args->mode_logtrace == DEFAULT_MODE_LOGTRACE ? " " : "*", ' ', MODE_STR(g_args->mode_logtrace));
#endif
#if DEVEL
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-bslic             =%s", g_args->mode_bslic == DEFAULT_MODE_BSLIC ? " " : "*", ' ', MODE_STR(g_args->mode_bslic));
#endif
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-gnid              =%s", g_args->mode_gnid == DEFAULT_MODE_GNID ? " " : "*", ' ', MODE_STR(g_args->mode_gnid));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-paths             =%s", g_args->mode_paths == DEFAULT_MODE_PATHS ? " " : "*", ' ', MODE_STR(g_args->mode_paths));
	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) mode-cycles            =%s", g_args->mode_cycles == DEFAULT_MODE_CYCLES ? " " : "*", ' ', MODE_STR(g_args->mode_cycles));

	if(g_args->concurrency == 0)
		snprintf(space, sizeof(space)  , "%s", "unbounded");
	else
		snprintf(space, sizeof(space)  , "%d", g_args->concurrency);

	logf(L_ARGS | L_PARAMS       , " %s (  %c  ) concurrency            =%s", g_args->concurrency == (int)((float)g_params.procs * 1.2f) ? " " : "*", 'j', space);

	for(x = 0; x < g_args->invokedirsl; x++)
	{
		int star = x && x != (g_args->invokedirsl - 1);
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) invokedirs(s)          =%s", star ? "*" : " ", 'I', g_args->invokedirs[x]);
	}

	logf(L_ARGS | L_PARAMS				, " %s (  %c  ) invalidate-all         =%s", g_args->invalidationsz == DEFAULT_INVALIDATE_ALL ? " " : "*", 'B', g_args->invalidationsz ? "yes" : "no");

	if(g_args->selectorsl == 0)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =", " ", ' ');
	for(x = 0; x < g_args->selectorsl; x++)
		logf(L_ARGS | L_PARAMS			, " %s (  %c  ) selector(s)            =%s", "*", ' ', selector_string(&g_args->selectors[x], space, sizeof(space)));

	logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

finally:
	path_free(init_fabpath);
coda;
}

void args_freeze(memblk * const restrict mb)
{
	memblk_freeze(mb, &g_args->argvs);
	memblk_freeze(mb, &g_args->buildscript_path);

	int x;

	for(x = 0; x < g_args->bs_runtime_varsl; x++)
		memblk_freeze(mb, &g_args->bs_runtime_vars[x]);
	memblk_freeze(mb, &g_args->bs_runtime_vars);

	for(x = 0; x < g_args->invokedirsl; x++)
		memblk_freeze(mb, &g_args->invokedirs[x]);
	memblk_freeze(mb, &g_args->invokedirs);

	for(x = 0; x < g_args->selectorsl; x++)
		selector_freeze(mb, &g_args->selectors[x]);
	memblk_freeze(mb, &g_args->selectors);

	path_freeze(mb, g_args->init_fabfile_path);
	memblk_freeze(mb, &g_args->init_fabfile_path);
}

void args_thaw(char * const mb)
{
  int x;

	memblk_thaw(mb, &g_args->argvs);
	memblk_thaw(mb, &g_args->buildscript_path);

	memblk_thaw(mb, &g_args->bs_runtime_vars);
	for(x = 0; x < g_args->bs_runtime_varsl; x++)
		memblk_thaw(mb, &g_args->bs_runtime_vars[x]);

	memblk_thaw(mb, &g_args->invokedirs);
	for(x = 0; x < g_args->invokedirsl; x++)
		memblk_thaw(mb, &g_args->invokedirs[x]);

	memblk_thaw(mb, &g_args->selectors);
	for(x = 0; x < g_args->selectorsl; x++)
		selector_thaw(mb, &g_args->selectors[x]);

	memblk_thaw(mb, &g_args->init_fabfile_path);
	path_thaw(mb, g_args->init_fabfile_path);
}
