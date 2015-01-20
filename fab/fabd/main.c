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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>

#include "xapi.h"
#include "xapi/callstack.h"

#include "xlinux.h"

#include "listwise.h"
#include "listwise/object.h"
#include "listwise/operators.h"
#include "listwise/lstack.h"
#include "listwise/LW.errtab.h"
#include "listwise/PCRE.errtab.h"

#include "global.h"

#include "parseint.h"
#include "macros.h"
#include "map.h"
#include "memblk.h"
#include "unitstring.h"
#include "say.h"

// errors to report and continue, otherwise exit
static int fab_swallow[] = {
	  [ FAB_SYNTAX ] = 1
	, [ FAB_ILLBYTE ] = 1
	, [ FAB_UNSATISFIED ] = 1
	, [ FAB_FMLFAIL ] = 1
	, [ FAB_NOINVOKE ] = 1
	, [ FAB_BADPLAN ] = 1
	, [ FAB_CYCLE ] = 1
	, [ FAB_NOSELECT ] = 1
};

static ff_parser *				ffp = 0;				// fabfile parser
static bp *								plan = 0;				// buildplan 
static map * 							rmap = 0;				// root-level map
static map *							vmap = 0;				// init-level map
static map * 							bakemap = 0;		// bakedvars map
static gn *								first = 0;			// first dependency mentioned
static lwx **							stax = 0;				// listwise stacks
static int								staxa = 0;
static int 								staxp = 0;
static ts **							tsp = 0;
static int								tsa = 0;
static int								tsl = 0;
static int								tsw = 0;

/* node selector produced lists */
static map *							smap = 0;							// selector map (must have lifetime >= lifetime of the selector lists)
static gn ***							fabrications = 0;
static int								fabricationsl = 0;
static gn ***							fabricationxs = 0;
static int								fabricationxsl = 0;
static gn ***							fabricationns = 0;
static int								fabricationnsl = 0;
static gn ***							invalidations = 0;
static int								invalidationsl = 0;
static gn ***							discoveries = 0;
static int								discoveriesl = 0;
static gn ***							inspections = 0;
static int								inspectionsl = 0;
static gn ***							queries = 0;
static int								queriesl = 0;

static pstring * 					ptmp;

// signal handling
static int o_signum;
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	if(!o_signum)
		o_signum = signum;

#if 0
	char space[2048];
	char * dst = space;
	size_t sz = sizeof(space);
	size_t z = 0;

	if(signum != SIGCHLD)
	{
		z += znprintf(dst + z, sz - z, "fabd[%u] received %d { from pid : %ld", getpid(), signum, (long)info->si_pid);

		if(signum == SIGCHLD)
		{
			z += znprintf(dst + z, sz - z, ", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);
		}
		z += znprintf(dst + z, sz - z, " }\n");
	}

	int __attribute__((unused)) r = write(1, space, z);
#endif
}

static int loop()
{
	int x;
	int y;

	char space[256];

	// reset some stuff
	tsl = 0;
	fabricationsl = 0;
	fabricationxsl = 0;
	fabricationnsl = 0;
	invalidationsl = 0;
	discoveriesl = 0;
	inspectionsl = 0;
	queriesl = 0;

	// comprehensive dependency discovery
	fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &tsp, &tsa, &tsw);

	// stat non-primary backing files
	for(x = 0; x < gn_nodes.l; x++)
	{
		if((gn_nodes.e[x]->type & GN_TYPE_HASFILE) && (gn_nodes.e[x]->type != GN_TYPE_PRIMARY))
		{
			fatal(hashblock_stat, gn_nodes.e[x]->path->can, gn_nodes.e[x]->hb);

			if(hashblock_cmp(gn_nodes.e[x]->hb))
			{
				gn_nodes.e[x]->invalid |= GN_INVALIDATION_CHANGED;
			}

			// require fabrication
			if(GN_IS_INVALID(gn_nodes.e[x]))
			{
				gn_nodes.e[x]->invalid |= GN_INVALIDATION_FABRICATE;
			}
		}
	}

	// process selectors
	if(g_args->selectorsl)
	{
		fatal(selector_init);

		int pn = staxp;

		// create $!
		fatal(lw_reset, &stax, &staxa, pn);
		for(x = 0; x < gn_nodes.l; x++)
			fatal(lstack_obj_add, stax[pn], gn_nodes.e[x], LISTWISE_TYPE_GNLW);

		// map for processing selectors
		fatal(map_create, &smap, 0);
		fatal(map_set, smap, MMS("!"), MM(stax[pn]), 0);
		pn++;

		// process selectors
		for(x = 0; x < g_args->selectorsl; x++)
			fatal(selector_process, &g_args->selectors[x], x, ffp, smap, &stax, &staxa, pn);

		fatal(selector_finalize
			, &fabrications, &fabricationsl
			, &fabricationxs, &fabricationxsl
			, &fabricationns, &fabricationnsl
			, &invalidations, &invalidationsl
			, &discoveries, &discoveriesl
			, &inspections, &inspectionsl
			, &queries, &queriesl
		);

		if(log_would(L_LISTS))
		{
			if(fabricationsl + fabricationxsl + fabricationnsl + invalidationsl + discoveriesl + inspectionsl == 0)
			{
				logf(L_LISTS, "empty");
			}
			
			for(x = 0; x < fabricationsl; x++)
				logf(L_LISTS, "fabrication(s)     =%s", (*fabrications[x])->idstring);

			for(x = 0; x < fabricationxsl; x++)
				logf(L_LISTS, "fabricationx(s)    =%s", (*fabricationxs[x])->idstring);

			for(x = 0; x < fabricationnsl; x++)
				logf(L_LISTS, "fabricationn(s)    =%s", (*fabricationns[x])->idstring);

			for(x = 0; x < invalidationsl; x++)
				logf(L_LISTS, "invalidation(s)    =%s", (*invalidations[x])->idstring);

			for(x = 0; x < discoveriesl; x++)
				logf(L_LISTS, "discover(y)(ies)   =%s", (*discoveries[x])->idstring);

			for(x = 0; x < inspectionsl; x++)
				logf(L_LISTS, "inspection(s)      =%s", (*inspections[x])->idstring);
		}
	}

	if(invalidationsl)
	{
		int repeat_discovery = 0;
		fatal(gn_process_invalidations, invalidations, invalidationsl, &repeat_discovery);

		if(repeat_discovery)
		{
			// repeat dependency discovery if any primary nodes that have dscvs were invalidated
			fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &tsp, &tsa, &tsw);
		}
	}

	// propagate invalidations and clear changes from this iteration
	int visit(gn * gn, int d)
	{
		if(d)
			gn->invalid |= GN_INVALIDATION_SOURCES;

		finally : coda;
	};
	for(x = 0; x < gn_nodes.l; x++)
	{
		if((gn_nodes.e[x]->type & GN_TYPE_HASFILE) && (gn_nodes.e[x]->invalid & (GN_INVALIDATION_CHANGED | GN_INVALIDATION_NXFILE | GN_INVALIDATION_USER)))
		{
			// do not cross the nofile boundary
			fatal(traverse_depth_bynodes_feedsward_noweak_usebridge_nonofile, gn_nodes.e[x], visit);

			gn_nodes.e[x]->invalid &= ~(GN_INVALIDATION_CHANGED | GN_INVALIDATION_NXFILE | GN_INVALIDATION_USER);
		}
	}

	if(g_args->selectors_arequery)
	{
#define CENTER(w, fmt, ...) ({																		\
	int act = snprintf(0, 0, fmt, ##__VA_ARGS__);										\
	snprintf(space, sizeof(space), "%*s"fmt"%*s"										\
		, (MAX(w - act, 0) / 2) + ((MAX(w - act, 0) % 2) ? 1 : 0), ""	\
		, ##__VA_ARGS__																								\
		, (MAX(w - act, 0) / 2), ""																		\
	);																															\
	space;																													\
})

		SAYF(" %-40s | %-13s | %-11s | %-11s | %s\n", "id", "type", "degree", "invalidated", "reason");
		for(x = 0; x < queriesl; x++)
		{
			// id
			if((*queries[x])->idstringl > 40)
			{
				SAYF(" .. %-37s", (*queries[x])->idstring + ((*queries[x])->idstringl - 37));
			}
			else
			{
				SAYF(" %-40s", (*queries[x])->idstring);
			}

			// type
			SAYF(" | %-13s", GN_TYPE_STR((*queries[x])->type));

			// degree
			SAYF(" | %11s", CENTER(11, "%-d/%d", (*queries[x])->needs.l, (*queries[x])->feeds.l));

			// invalid
			SAYF(" | %-11s", CENTER(11, "%s", (*queries[x])->invalid ? "x" : ""));

			// reason
			SAYS(" | ");
			if((*queries[x])->type & GN_TYPE_HASFILE)
			{
				gn_invalid_reasons_write((*queries[x]), space, sizeof(space));
				SAYS(space);
			}
			SAYF("\n");
		}
		SAYF(" %d nodes\n", queriesl);
	}

	if(g_args->selectors_arediscovery)
	{
		fatal(log_parse_and_describe, "+DSC", 0, 0, L_INFO);
		fatal(dsc_exec_specific, discoveries, discoveriesl, vmap, ffp->gp, &stax, &staxa, staxp, &tsp, &tsa, &tsw);
		fatal(log_parse_pop);
	}

	if(g_args->selectors_areinspections)
	{
		fatal(log_parse_and_describe, "+DGRAPH", 0, 0, L_INFO);

		for(x = 0; x < inspectionsl; x++)
			gn_dump((*inspections[x]));

		fatal(log_parse_pop);
	}

	if(g_args->selectors_arequery == 0 && g_args->selectors_areinspections == 0 && g_args->selectors_arediscovery == 0)
	{
		//
		// construct a buildplan - use the first node if none was specified
		//
		if(fabricationsl + fabricationxsl + fabricationnsl == 0 && first)
		{
			fatal(xrealloc, &fabrications, sizeof(*fabrications), 1, 0);
			fabrications[fabricationsl++] = &first;
		}

		// mixing task and non-task as buildplan targets does not make sense
		for(x = 1; x < fabricationsl + fabricationxsl + fabricationnsl; x++)
		{
			int a = x - 1;
			int b = x;
			gn * A = 0;
			gn * B = 0;

			if(a < fabricationsl)
				A = *fabrications[a];
			else if(a < (fabricationsl + fabricationxsl))
				A = *fabricationxs[a - fabricationsl];
			else
				A = *fabricationns[a - fabricationsl - fabricationxsl];

			if(b < fabricationsl)
				B = *fabrications[b];
			else if(b < (fabricationsl + fabricationxsl))
				B = *fabricationxs[b - fabricationsl];
			else
				B = *fabricationns[b - fabricationsl - fabricationxsl];

			if((A->type == GN_TYPE_TASK) ^ (B->type == GN_TYPE_TASK))
				fails(FAB_BADPLAN, "cannot mix task and non-task targets");
		}

		fatal(bp_create, fabrications, fabricationsl, fabricationxs, fabricationxsl, fabricationns, fabricationnsl, &plan);

		// selector lists not referenced again past this point
		map_xfree(&smap);

		if(plan)
		{
			if(g_args->mode_bplan == MODE_BPLAN_BUILDSCRIPT)
			{
				// prepare bs_runtime_vars map
				fatal(map_create, &bakemap, 0);

				for(x = 0; x < g_args->bs_runtime_varsl; x++)
					fatal(map_set, bakemap, MMS(g_args->bs_runtime_vars[x]), 0, 0, 0);

				// dump buildplan, pending logging
				if(plan)
					bp_dump(plan);

				// create buildscript
				fatal(buildscript_mk, plan, g_args->argvs, vmap, ffp->gp, &stax, &staxa, staxp, bakemap, &tsp, &tsa, &tsw, g_args->buildscript_path);
			}
			else
			{
				// incremental build - prune the buildplan
				fatal(bp_eval, plan);

				if(g_args->mode_bplan == MODE_BPLAN_GENERATE)
					fatal(log_parse_and_describe, "+BPDUMP", 0, 0, L_INFO);

				// dump buildplan, pending logging
				if(plan)
					bp_dump(plan);

				if(g_args->mode_bplan == MODE_BPLAN_EXEC)
				{
					if(plan && plan->stages_l)
					{
						// execute the build plan, one stage at a time
						fatal(bp_prepare, plan, ffp->gp, &stax, &staxa, staxp, &tsp, &tsl, &tsa);

						// work required ; notify fab
						fatal(uxkill, g_params.fab_pid, FABSIG_BPSTART, 0);

						// await response
						o_signum = 0;
						pause();

						if(o_signum == FABSIG_BPGOOD)
						{
							// plan was executed successfully ; update nodes for all products
							for(x = 0; x < tsl; x++)
							{
								for(y = 0; y < tsp[x]->fmlv->productsl; y++)
								{
									// mark as up-to-date
									tsp[x]->fmlv->products[y]->invalid = 0;

									// reload hashblock
									fatal(hashblock_stat, tsp[x]->fmlv->products[y]->path->can, tsp[x]->fmlv->products[y]->hb);
								}
							}

#if 0
							logs(L_BPINFO, "updated");
							for(x = 0; x < fabricationsl; x++)
								logf(L_BPINFO, " %s", (*fabrications[x])->idstring);
							for(x = 0; x < fabricationxsl; x++)
								logf(L_BPINFO, " %s", (*fabricationxs[x])->idstring);
							for(x = 0; x < fabricationnsl; x++)
								logf(L_BPINFO, " %s", (*fabricationns[x])->idstring);
#endif
						}
						else if(o_signum == FABSIG_BPBAD)
						{
							// plan failed ; harvest the results
							fatal(bp_harvest, plan);
						}
						else
						{
							fail(FAB_BADIPC);
						}
					}
				}
			}
		}
	}

finally:
	bp_xfree(&plan);
coda;
}

int main(int argc, char** argv)
{
	char stem[2048];
	char space[2048];

	int fd = -1;

	int argsfd = -1;
	struct stat argsb = {};

	int logsfd = -1;
	struct stat logsb = {};
	char * logvs = 0;
	size_t logvsl = 0;

	int r;
	int x;
	int y;
	size_t tracesz = 0;

	struct timespec time_start = {};
	struct timespec time_end = {};

#if 0
SAYF("fabd[%ld] started\n", (long)getpid());
#endif

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// handle all signals
	struct sigaction action = {
		  .sa_sigaction = signal_handler
		, .sa_flags = SA_SIGINFO
	};
	for(x = 1; x < SIGUNUSED; x++)
	{
		if(x != SIGKILL && x != SIGSTOP && x != SIGSEGV)
		{
			fatal(xsigaction, x, &action, 0);
		}
	}
	for(x = SIGRTMIN; x <= SIGRTMAX; x++)
	{
#if DEBUG || DEVEL
    if((x - SIGRTMIN) == 30)
    {
      // internal valgrind signal
      continue;
    }
#endif

    fatal(xsigaction, x, &action, 0);
	}

	// standard file descriptors
	close(0);
	fatal(xopen, "/dev/null", O_RDONLY, 0);

#if __linux__
	// arrange to terminate if the parent fabw process dies
	fatal(xprctl, PR_SET_PDEATHSIG, SIGTERM, 0, 0, 0);
#endif

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);

	// fabd maintains the fabsys identity
	fatal(identity_assume_fabsys);

	// initialize logger
	fatal(log_init);

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	fatal(listwise_register_opdir, XQUOTE(FABLWOPDIR));

#if DEBUG || DEVEL
	// configure liblistwise logging
	lw_configure_logging();
#endif

	// default logger configuration (should match the other invocation of log_parse
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0, L_INFO);

	// other initializations
	fatal(tmp_setup);
	fatal(gn_init);
	fatal(traverse_init);
	fatal(ff_mkparser, &ffp);

	// create the rootmap
	fatal(var_root, &rmap);

	// get ipc dir
	uint32_t canhash;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	size_t z = snprintf(stem, sizeof(stem), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	while(o_signum != SIGINT && o_signum != SIGQUIT && o_signum != SIGTERM)
	{
		// start measuring
		fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);

		// read fab/pid
		snprintf(space, sizeof(space), "%s/fab/pid", stem);
		fatal(ixclose, &fd);
		fatal(uxopen, space, O_RDONLY, &fd);

		r = -1;
		if(fd == -1)
		{
			logf(L_RED, "failed to open %s", space);
		}
		else
		{
			fatal(axread, fd, &g_params.fab_pid, sizeof(g_params.fab_pid));

			// existence check - is anyone listening?
			fatal(uxkill, g_params.fab_pid, 0, &r);
		}

#define REQUIRE_LISTENER 1

#if REQUIRE_LISTENER
		if(r)
#else
		if(0)
#endif
		{
			SAYF("fab[%lu] not listening\n", (long)g_params.fab_pid);
			exit(0);
		}
		else
		{
#if REQUIRE_LISTENER
			// reopen file descriptors
			fatal(xclose, 1);
			snprintf(space, sizeof(space), "/proc/%ld/fd/1", (long)g_params.fab_pid);
			fatal(xopen, space, O_RDWR, 0);

			fatal(xclose, 2);
			snprintf(space, sizeof(space), "/proc/%ld/fd/2", (long)g_params.fab_pid);
			fatal(xopen, space, O_RDWR, 0);
#endif

			// open args file
			snprintf(space, sizeof(space), "%s/args", stem);
			fatal(ixclose, &argsfd);
			fatal(xopen, space, O_RDWR, &argsfd);
			
			// release previous args mapping
			fatal(ixmunmap, &g_args, argsb.st_size);

			// map the file
			fatal(xfstat, argsfd, &argsb);
			fatal(xmmap, 0, argsb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, argsfd, 0, (void*)&g_args);

			// unpack the args
			args_thaw((void*)g_args);

			// open logs file
			snprintf(space, sizeof(space), "%s/logs", stem);
			fatal(ixclose, &logsfd);
			fatal(xopen, space, O_RDWR, &logsfd);

			// release previous logs mapping
			fatal(ixmunmap, &logvs, logsb.st_size);

			// map the file
			fatal(xfstat, logsfd, &logsb);
			if((logvsl = logsb.st_size))
			{
				fatal(xmmap, 0, logsb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, logsfd, 0, (void*)&logvs);
			}

			// re-configure logger
			fatal(log_parse_clear);

#if DEBUG || DEVEL
			if(g_args->mode_logtrace == MODE_LOGTRACE_FULL)
				fatal(log_config, ~0, ~0);
			else
				fatal(log_config, ~0, 0);
#else
			fatal(log_config, ~0);
#endif

			// default tags - do not describe, that as done previously
			fatal(log_parse, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0);

			// user-specified
			if(logvsl)
				fatal(log_parse_and_describe, logvs, logvsl, 0, L_INFO);

			// log args summary
			fatal(args_summarize);

#if REQUIRE_LISTENER
			// chdir
			snprintf(space, sizeof(space), "/proc/%ld/cwd", (long)g_params.fab_pid);
			fatal(xchdir, space);
#endif

			// parse the fabfiles only on the first run
			static int initialized;
			if(initialized == 0)
			{
				// parse variable expression text from cmdline (the -v option)
				for(x = 0; x < g_args->rootvarsl; x++)
				{
					ff_file * vff = 0;
					fatal(ff_var_parse, ffp, g_args->rootvars[x], strlen(g_args->rootvars[x]), x, &vff);

					// process variable expressions
					int k;
					for(k = 0; k < vff->ffn->statementsl; k++)
					{
						ff_node * stmt = vff->ffn->statements[k];

						if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARXFM_ADD || stmt->type == FFN_VARXFM_SUB)
						{
							int pn = staxp;
							fatal(list_resolve, stmt->definition, rmap, ffp->gp, &stax, &staxa, &staxp, 0, 0);
							staxp++;

							for(y = 0; y < stmt->varsl; y++)
							{
								if(stmt->type == FFN_VARASSIGN)
								{
									fatal(var_set, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, 0, stmt);
								}
								else if(stmt->type == FFN_VARXFM_ADD)
								{
									fatal(var_xfm_add, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, stmt);
								}
								else if(stmt->type == FFN_VARXFM_SUB)
								{
									fatal(var_xfm_sub, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, stmt);
								}
							}
						}
						else if(stmt->type == FFN_VARXFM_LW)
						{
							for(y = 0; y < stmt->varsl; y++)
							{
								if(stmt->transform_node)
								{
									fatal(var_xfm_lw, rmap, stmt->vars[y]->name->text->s, stmt->transform_node->transform, stmt->transform_node->text->s, 1, stmt);
								}
								else if(stmt->transform_list_node)
								{

								}
							}
						}
					}
				}

				// use up one list and populate the # variable (relative directory path to the initial fabfile)
				fatal(lw_reset, &stax, &staxa, staxp);
				fatal(lstack_addw, stax[staxp], g_args->init_fabfile_path->abs_dir, g_args->init_fabfile_path->abs_dirl);
				fatal(var_set, rmap, "#", stax[staxp++], 1, 0, 0);

				// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
				fatal(var_clone, rmap, &vmap);

				// parse, starting with the initial fabfile, construct the graph
				fatal(ffproc, ffp, g_args->init_fabfile_path, vmap, &stax, &staxa, &staxp, &first);

				// compute node types
				initialized = 1;

				// load file hashes
				for(x = 0; x < ff_files.l; x++)
					fatal(hashblock_stat, ff_files.e[x]->path->abs, ff_files.e[x]->hb);
			}
			else
			{
				x = 0;
				if(g_args->rootvarsl == 0)
				{
					// check for fabfile changes
					for(x = 0; x < ff_files.l; x++)
					{
						fatal(hashblock_stat, ff_files.e[x]->path->abs, ff_files.e[x]->hb);
						if(hashblock_cmp(ff_files.e[x]->hb))
							break;
					}
				}

				if(x < ff_files.l)
				{
					// fabfile changes : exec new fabd instance
					execvp(argv[0], argv);
				}
			}

			// handle this request
			int frame;
			if(invoke(&frame, loop))
			{
				// propagate errors other than those specifically being trapped
				if(XAPI_ERRTAB == perrtab_FAB && fab_swallow[XAPI_ERRCODE])
				{

				}
				else if(XAPI_ERRTAB == perrtab_LW)
				{

				}
				else if(XAPI_ERRTAB == perrtab_PCRE)
				{
					
				}
				else
				{
					fail(0);
				}

				// log the backtrace
#if DEBUG || DEVEL
				int mode = DEFAULT_MODE_BACKTRACE;

				if(g_args)
					mode = g_args->mode_backtrace;
#endif

				/*
				** populate the base frame
				*/
				XAPI_FRAME_SET(perrtab, 0);

#if DEBUG || DEVEL
				if(mode == MODE_BACKTRACE_FULL)
					tracesz = xapi_trace_full(space, sizeof(space));
				else
#endif
					tracesz = xapi_trace_pithy(space, sizeof(space));

				logw(L_RED, space, tracesz);

				// pop the error frames
				xapi_callstack_unwindto(frame);
			}

			// stop measuring
			fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_end);

			z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
			logf(L_INFO, "elapsed : %.*s", (int)z, space);

#if DEBUG || DEVEL
			if(log_would(L_USAGE))
			{
				// check memory usage
				snprintf(space, sizeof(space), "/proc/self/statm");
				fatal(ixclose, &fd);
				fatal(uxopen, space, O_RDONLY, &fd);

				long pgz = 0;
				if(fd != -1)
				{
					fatal(xread, fd, space, sizeof(space), 0);
					sscanf(space, "%*d %ld", &pgz);
				}
				logf(L_USAGE, "usage : mem(%s), lwx(%d), gn(%d), ts(%d)", bytestring(pgz * g_params.pagesize), staxa, gn_nodes.a, tsa);
			}
#endif

			// task complete : notify fab
			fatal(uxkill, g_params.fab_pid, FABSIG_DONE, 0);

			// timer reset
			memset(&time_start, 0, sizeof(time_start));
			memset(&time_end, 0, sizeof(time_end));
		}

#if REQUIRE_LISTENER
		// wait for next command
		o_signum = 0;
		pause();
#else
		break;
#endif
	}

finally:

	ff_freeparser(ffp);
	bp_free(plan);
	map_free(rmap);
	map_free(vmap);
	map_free(smap);
	map_free(bakemap);

	for(x = 0; x < staxa; x++)
	{
		if(stax[x])
			free(lwx_getptr(stax[x]));
		lwx_free(stax[x]);
	}
	free(stax);

	for(x = 0; x < tsa; x++)
		ts_free(tsp[x]);
	free(tsp);

	free(fabrications);
	free(fabricationxs);
	free(fabricationns);
	free(invalidations);
	free(discoveries);
	free(inspections);
	free(queries);

	gn_teardown();
	fml_teardown();
	ff_teardown();
	params_teardown();
	traverse_teardown();
	selector_teardown();

	fatal(ixmunmap, &g_args, argsb.st_size);
	fatal(ixclose, &argsfd);
	fatal(ixclose, &logsfd);
	fatal(ixclose, &fd);

	psfree(ptmp);

	if(XAPI_UNWINDING)
	{
#if DEBUG || DEVEL
		int mode = DEFAULT_MODE_BACKTRACE;

		if(g_args)
			mode = g_args->mode_backtrace;

		if(mode == MODE_BACKTRACE_FULL)
			tracesz = xapi_trace_full(space, sizeof(space));
		else
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));

		logw(L_RED, space, tracesz);
	}

	if((time_start.tv_sec || time_start.tv_nsec) && (time_end.tv_sec == 0 && time_end.tv_nsec == 0))
	{
		// stop measuring
		if(clock_gettime(CLOCK_MONOTONIC_RAW, &time_end) == 0)
		{
			z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
			logf(0, "elapsed : %.*s", (int)z, space);
		}
		else
		{
			logf(0, "elapsed : UNKNOWN");
		}
	}
coda;
}
