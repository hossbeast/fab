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
#include "narrate.h"

#include "listwise.h"
#include "listwise/object.h"
#include "listwise/operators.h"
#include "listwise/lstack.h"
#include "listwise/LW.errtab.h"
#include "listwise/PCRE.errtab.h"

#include "global.h"
#include "params.h"
#include "args.h"
#include "gn.h"
#include "fabd_handler.h"
#include "identity.h"
#include "gnlw.h"
#include "lwutil.h"
#include "traverse.h"
#include "var.h"
#include "path.h"
#include "ffproc.h"
#include "cfg.h"
#include "cfg.protected.h"
#include "fml.h"
#include "tmp.h"
#include "selector.h"

#include "parseint.h"
#include "macros.h"
#include "map.h"
#include "memblk.h"
#include "unitstring.h"
#include "cksum.h"
#include "sigbank.h"

// write log messages about process creation and signals received
#ifndef DEBUG_IPC
# define DEBUG_IPC 0
#endif

// errors to report and continue, otherwise exit
static int fab_swallow[] = {
	  [ FAB_SYNTAX ] = 1
	, [ FAB_ILLBYTE ] = 1
	, [ FAB_UNSATISFIED ] = 1
	, [ FAB_CMDFAIL ] = 1
	, [ FAB_NOINVOKE ] = 1
	, [ FAB_BADPLAN ] = 1
	, [ FAB_CYCLE ] = 1
	, [ FAB_NOSELECT ] = 1
	, [ FAB_DSCPARSE ] = 1
};

static pid_t fab_pids[3];      // most recent fab pids
static int initialized;

int main(int argc, char** argv, char ** envp)
{
	char space[2048];
	char space2[2048];

	map * rmap = 0;					// root-level map
	map *	vmap = 0;					// init-level map
	gn *	first = 0;				// first dependency mentioned

	int fd = -1;

	int argsfd = -1;
	struct stat argsb = {};

	int logsfd = -1;
	struct stat logsb = {};
	char * logvs = 0;
	size_t logvsl = 0;

	struct timespec time_start;
	struct timespec time_end;

	int r;
	int x;
	int y;
	size_t tracesz = 0;

  fabd_handler_context * ctx = 0;

	// fabd is normally invoked with a single argument
	int nodaemon = 0;
	if(argc > 2)
		if(strcmp(argv[2], "--nodaemon") == 0)
			nodaemon = 1;

  // initialize signal handling
  fatal(sigbank_init
#if DEBUG_IPC
    , "fabd", getpid()
#endif
  );

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
	
	// get fabsys identity
	fatal(identity_assume_fabsys);

	// allow creation of world-rw files
	umask(0);

	unsigned long * auxv = 0;
#if __linux__
	// locate auxiliary vector
	while(*envp)
		envp++;
	envp++;
	auxv = (void*)envp;
#endif

	// initialize logger
	fatal(log_init, auxv);

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	fatal(listwise_register_opdir, XQUOTE(FABLWOPDIR));

#if DEBUG || DEVEL
	// configure liblistwise logging
	lw_configure_logging();
#endif

	// default logger configuration (should match the other invocation of log_parse)
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0, L_INFO);

	// other initializations
  fatal(fabd_handler_context_mk, &ctx);
	fatal(gn_init);
	fatal(traverse_init);

	// track start time
	g_params.starttime = time(0);

	// create the rootmap
	fatal(var_root, &rmap);

	// get ipc dir
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &g_params.canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	snprintf(g_params.ipcstem, sizeof(g_params.ipcstem), "%s/%u", XQUOTE(FABIPCDIR), g_params.canhash);

	// create symlink for dsc in hashdir
	snprintf(space, sizeof(space), "%s/dsc", g_params.ipcstem);
	snprintf(space2, sizeof(space2), XQUOTE(FABTMPDIR) "/pid/%d/dsc", g_params.pid);
	fatal(uxunlink, space, 0);
	fatal(xsymlink, space2, space);

  int sig = g_signum;
	while(sig != SIGINT && sig != SIGQUIT && sig != SIGTERM)
	{
		// read fab/pid
		snprintf(space, sizeof(space), "%s/fab/pid", g_params.ipcstem);
		fatal(ixclose, &fd);
		fatal(xopen, space, O_RDONLY, &fd);

		fatal(axread, fd, &g_params.fab_pid, sizeof(g_params.fab_pid));

		// existence check - is anyone listening?
		r = -1;
		fatal(uxkill, g_params.fab_pid, 0, &r);

		if(!nodaemon && r)
		{
			failf(FAB_BADIPC, "fab[%lu] not listening", (long)g_params.fab_pid);
		}
		else
		{
			if(!nodaemon)
			{
				fatal(ixclose, &fd);

				// reopen standard file descriptors
				snprintf(space, sizeof(space), "%s/fab/out", g_params.ipcstem);
				fatal(xopen, space, O_RDWR, &fd);
				fatal(xdup2, fd, 1);
				fd = -1;

				snprintf(space, sizeof(space), "%s/fab/err", g_params.ipcstem);
				fatal(xopen, space, O_RDWR, &fd);
				fatal(xdup2, fd, 2);
				fd = -1;
			}

			// open args file
			snprintf(space, sizeof(space), "%s/args", g_params.ipcstem);
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
			snprintf(space, sizeof(space), "%s/logs", g_params.ipcstem);
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

			// default tags - do not describe, that was done previously
			fatal(log_parse, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0);

			// user-specified
			if(logvsl)
				fatal(log_parse_and_describe, logvs, logvsl, 0, L_INFO);

			// log args summary
			fatal(args_summarize);

			// chdir
			if(!nodaemon)
			{
				snprintf(space, sizeof(space), "%s/fab/cwd", g_params.ipcstem);
				fatal(xchdir, space);
			}

			// parse the fabfiles only on the first run
			if(!initialized)
			{
				logf(L_INFO, "loading build description");

				// start measuring
				fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);

				// use up one list and populate the # variable (relative directory path to the initial fabfile)
				fatal(lw_reset, &ctx->stax, &ctx->staxa, ctx->staxp);
				fatal(lstack_addw, ctx->stax[ctx->staxp], g_args->init_fabfile_path->abs_dir, g_args->init_fabfile_path->abs_dirl);
				fatal(var_set, rmap, "#", ctx->stax[ctx->staxp++], 1, 0, 0);

				// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
				fatal(var_clone, rmap, &vmap);

				// parse, starting with the initial fabfile, construct the graph
				fatal(ffproc, ctx->ffp, g_args->init_fabfile_path, vmap, &ctx->stax, &ctx->staxa, &ctx->staxp, &first);

				// compute node types
				initialized = 1;

				// load file hashes
				for(x = 0; x < ff_files.l; x++)
					fatal(hashblock_stat, ff_files.e[x]->path->abs, ff_files.e[x]->hb);

				if(log_would(L_TIME))
				{
					// stop the clock
					fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_end);
					
					size_t z = elapsed_string_timespec(&time_start, &time_end, space, sizeof(space));
					logf(L_TIME, "elapsed : %.*s", (int)z, space);
				}
			}
			else
			{
				x = 0;
				y = 0;

        // check for fabfile changes
        for(x = 0; x < ff_files.l; x++)
        {
          fatal(hashblock_stat, ff_files.e[x]->path->abs, ff_files.e[x]->hb);
          if(hashblock_cmp(ff_files.e[x]->hb))
            break;
        }

        if(x == ff_files.l)
        {
          // check for cfg file changes
          for(y = 0; y < cfg_files.l; y++)
          {
            fatal(hashblock_stat, cfg_files.e[y]->path->abs, cfg_files.e[y]->hb);
            if(hashblock_cmp(cfg_files.e[y]->hb))
              break;
          }
        }

				if(x < ff_files.l || y < cfg_files.l)
				{
					// exec fabd, because of changes to:
					//  fabfile, or
					//  cfg file
					execvp(argv[0], argv);
				}
			}

			// handle this request
			int frame;
			if(invoke(&frame, fabd_handler_handle_request, ctx, vmap, first))
			{
				// capture the error code
				int code = XAPI_ERRCODE;

				// propagate errors other than those specifically being trapped
				if(XAPI_ERRTAB == perrtab_FAB && fab_swallow[code])
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
				{
					tracesz = xapi_trace_full(space, sizeof(space));
				}
				else
#endif
				{
					tracesz = xapi_trace_pithy(space, sizeof(space));
				}

				// log the error
				logw(L_RED, space, tracesz);

				// pop the error frames
				xapi_callstack_unwindto(frame);

				// write the error to ipcdir
				snprintf(space, sizeof(space), "%s/fabd/error", g_params.ipcstem);
				fatal(ixclose, &fd);
				fatal(xopen_mode, space, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
				fatal(axwrite, fd, &code, sizeof(code));
			}

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
				logf(L_USAGE, "usage : mem(%s), lwx(%d), gn(%d), ts(%d)", bytestring(pgz * g_params.pagesize), ctx->staxa, gn_nodes.a, ctx->tsa);
			}

			// task complete : notify fab
			fatal(uxkill, g_params.fab_pid, FABSIG_DONE, 0);
		}

		// cleanup tmp dir, including specifically the last fab pid we are tracking
		fatal(tmp_cleanup, &fab_pids[sizeof(fab_pids) / sizeof(fab_pids[0]) - 1], 1);

		// cycle fab pids
		memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
		fab_pids[0] = g_params.fab_pid;

		if(nodaemon)
			break;

    // receive a signal
    fatal(sigreceive, &sig);
	}

	// touch stamp file
	snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);
	fatal(ixclose, &fd);
	fatal(uxopen_mode, space, O_CREAT | O_RDWR, FABIPC_DATA, &fd);
	if(fd != -1)
	  fatal(xfutimens, fd, 0);

	// cycle in my own pid
	memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
	fab_pids[0] = g_params.pid;

	// cleanup tmp dir, including specifically all of the fab pids we are tracking and my own pid
	fatal(tmp_cleanup, fab_pids, sizeof(fab_pids) / sizeof(fab_pids[0]));

finally:
  // cleanup
	map_free(rmap);
	map_free(vmap);
  fabd_handler_context_free(ctx);
	fatal(ixmunmap, &g_args, argsb.st_size);
	fatal(ixclose, &argsfd);
	fatal(ixclose, &logsfd);
	fatal(ixclose, &fd);

  // module teardown
	gn_teardown();
	fml_teardown();
	ff_teardown();
	params_teardown();
	traverse_teardown();
	selector_teardown();

  // error trace
#if DEBUG || DEVEL
  XAPI_INFOF("pid", "%ld", (long)getpid());
#endif

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

	log_teardown();
coda;
}
