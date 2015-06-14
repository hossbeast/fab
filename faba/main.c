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
#include <sys/wait.h>
#include <sys/prctl.h>

#include "xapi.h"
#include "xapi/callstack.h"
#include "xlinux.h"
#include "pstring.h"
#include "narrate.h"

#include "global.h"
#include "args.def.h"
#include "params.h"
#include "identity.h"
#include "executor.h"
#include "faba_handler.h"

#include "parseint.h"
#include "macros.h"
#include "sigbank.h"

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

int main(int argc, char ** argv, char ** envp)
{
	char space[2048];

  size_t tracesz;
	int argsfd = -1;
	struct stat argsb = {};

	int logsfd = -1;
	struct stat logsb = {};
	char * logvs = 0;
	size_t logvsl = 0;

	int fd = -1;

  executor_context * ctx = 0;

  // faba is normally invoked with a single argument
  int nodaemon = 0;
  if(argc > 2)
    if(strcmp(argv[2], "--nodaemon") == 0)
      nodaemon = 1;

  // initialize signal handling
  fatal(sigbank_init
#if DEBUG_IPC
    , "faba", getpid()
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

	// default logger configuration (should match the other invocation of log_parse)
	fatal(log_parse_and_describe, "+ERROR|WARN|INFO|BPINFO|DSCINFO", 0, 0, L_INFO);
  
  // other initializations
  fatal(executor_context_mk, &ctx);

	// get ipc dir
	uint32_t canhash;
	if(argc < 2 || parseuint(argv[1], SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) != 0)
		fail(FAB_BADARGS);

	// ipc-dir stem
	snprintf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/%u", XQUOTE(FABIPCDIR), g_params.canhash);

sigset_t pend;
sigpending(&pend);
printf("%d entering while loop, pending : ", g_params.pid);
int x;
for(x = FABSIG_BASE; x <= (FABSIG_BASE + FABSIG_SIZE); x++)
{
  if(sigismember(&pend, x))
    printf("FABSIG_BASE+%d, ", x);
}
printf("\n");

	// load fab/pid
	snprintf(space, sizeof(space), "%s/fab/pid", g_params.ipcdir);
	fatal(xopen, space, O_RDONLY, &fd);
	fatal(axread, fd, &g_params.fab_pid, sizeof(g_params.fab_pid));
  fatal(ixclose, &fd);

  // signal to fab that I am ready to receive a command
  fatal(xkill, g_params.fab_pid, FABSIG_READY);

  // process requests
  while(g_signum != SIGINT && g_signum != SIGQUIT && g_signum != SIGTERM)
  {
    // receive a signal from fab
    int sig;
    fatal(sigreceive, (int[]) { FABSIG_START, 0 }, &sig);

    if(sig == FABSIG_START)
    {
      // read fabd/pgid
      snprintf(space, sizeof(space), "%s/fabd/pgid", g_params.ipcdir);
      fatal(ixclose, &fd);
      fatal(xopen, space, O_RDONLY, &fd);
      fatal(axread, fd, &g_params.fabd_pgid, sizeof(g_params.fabd_pgid));

      // existence check - is anyone listening?
      int r = -1;
      fatal(uxkill, -g_params.fabd_pgid, 0, &r);

      if(!nodaemon && r)
      {
        failf(FAB_BADIPC, "fabd[%lu] not listening", (long)g_params.fabd_pgid);
      }
      else
      {
        if(!nodaemon)
        {
          fatal(ixclose, &fd);

          // reopen standard file descriptors
          snprintf(space, sizeof(space), "%s/fab/out", g_params.ipcdir);
          fatal(xopen, space, O_RDWR, &fd);
          fatal(xdup2, fd, 1);
          fd = -1;

          snprintf(space, sizeof(space), "%s/fab/err", g_params.ipcdir);
          fatal(xopen, space, O_RDWR, &fd);
          fatal(xdup2, fd, 2);
          fd = -1;
        }

        // open args file
        snprintf(space, sizeof(space), "%s/args", g_params.ipcdir);
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
        snprintf(space, sizeof(space), "%s/logs", g_params.ipcdir);
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

        // chdir
        if(!nodaemon)
        {
          snprintf(space, sizeof(space), "%s/fab/cwd", g_params.ipcdir);
          fatal(xchdir, space);
        }

        // handle this request
        int frame;
        if(invoke(&frame, faba_handler_handle_request, ctx))
        {
          // capture the error code
          int code = XAPI_ERRCODE;

          // propagate errors other than those specifically being trapped
          if(XAPI_ERRTAB == perrtab_FAB && fab_swallow[code])
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
          snprintf(space, sizeof(space), "%s/faba/error", g_params.ipcdir);
          fatal(ixclose, &fd);
          fatal(xopen_mode, space, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
          fatal(axwrite, fd, &code, sizeof(code));
        }

        // task complete : notify fabd
        fatal(uxkill, g_params.fab_pid, FABSIG_DONE, 0);
      }
    }

		if(nodaemon)
			break;
  }
	
finally:
  // cleanup
	fatal(ixmunmap, &g_args, argsb.st_size);
	fatal(ixclose, &argsfd);
	fatal(ixclose, &logsfd);
	fatal(ixclose, &fd);
  fatal(executor_context_dispose, ctx);

  // module teardown

  // error trace
#if DEBUG || DEVEL
  XAPI_INFOS("name", "faba");
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
