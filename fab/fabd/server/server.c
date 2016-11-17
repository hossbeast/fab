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

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"

#include "fab/command.h"
#include "fab/ipc.h"
#include "fab/request.h"
#include "fab/response.h"
#include "fab/sigbank.h"
#include "listwise/LISTWISE.errtab.h"
#include "listwise/PCRE.errtab.h"
#include "logger.h"
#include "logger/stream.h"
#include "narrator.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"

#include "server.h"
#include "config.h"
#include "errtab/FABD.errtab.h"
#include "logging.h"
#include "params.h"

#include "macros.h"
#include "memblk.def.h"
#include "memblk.h"

//
// static
//

/// load_client_pid
//
// SUMMARY
//  load the client pid
//
static xapi load_client_pid(fab_server * const restrict server, pid_t expected_client_pid)
{
  enter;

  int fd = -1;

  // load fab/pid

  fatal(xopenf, &fd, O_RDONLY, "%s/client/pid", g_params.ipcdir);
  fatal(axread, fd, &server->client_pid, sizeof(server->client_pid));
  if(server->client_pid <= 0)
  {
    xapi_fail_intent();
    xapi_info_adds("expected client pid", "> 0");
    xapi_info_addf("actual client pid", "%ld", (long)server->client_pid);
    fail(FABD_BADIPC);
  }

  if(expected_client_pid && expected_client_pid != server->client_pid)
  {
    xapi_fail_intent();
    xapi_info_addf("expected client pid", "%ld", expected_client_pid);
    xapi_info_addf("actual client pid", "%ld", server->client_pid);
    fail(FABD_BADIPC);
  }

finally:
  fatal(ixclose, &fd);
coda;
}

/// handle
//
// SUMMARY
//  determine whether an error should be trapped
//
static int handle(fab_request * const restrict request, memblk * const restrict mb, fab_response ** const restrict response)
{
  enter;

  int x;
  for(x = 0; x < request->commandsl; x++)
  {
    fab_command * cmd = request->commands[x];
    if((cmd->attrs & FABCORE_COMMAND_OPT) == FABCORE_CONFIGURATION_MERGE)
    {
      fatal(config_apply, cmd->text);
    }
    else if((cmd->attrs & FABCORE_COMMAND_OPT) == FABCORE_CONFIGURATION_APPLY)
    {
      xapi res;
      fatal(config_reconfigure, &res);
    }
  }

#if 0
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
#endif

  finally : coda;
}

static xapi redirect(fab_server * const restrict server)
{
  enter;

  int fd = -1;
  char space[512];

  // redirect stdout/stderr to the client
  snprintf(space, sizeof(space), "%s/client/out", g_params.ipcdir);
  fatal(xopens, &fd, O_RDWR, space);
  fatal(xdup2, fd, 1);
  fatal(ixclose, &fd);

  snprintf(space, sizeof(space), "%s/client/err", g_params.ipcdir);
  fatal(xopens, &fd, O_RDWR, space);
  fatal(xdup2, fd, 2);

finally:
  fatal(ixclose, &fd);
coda;
}


//
// public
//

xapi fab_server_create(fab_server ** const restrict server)
{
  enter;

  fatal(xmalloc, server, sizeof(**server));
  (*server)->client_cwd_dirfd = -1;

  finally : coda;
}

xapi fab_server_xfree(fab_server * const restrict server)
{
  enter;

  if(server)
  {
    fatal(ixshmdt, &server->request_shm);
    fatal(ixshmdt, &server->response_shm);
    fatal(ixclose, &server->client_cwd_dirfd);
  }

  wfree(server);

  finally : coda;
}

xapi fab_server_ixfree(fab_server ** const restrict server)
{
  enter;

  fatal(fab_server_xfree, *server);
  *server = 0;

  finally : coda;
}

xapi fab_server_ready(fab_server * const restrict server)
{
  enter;

  fatal(load_client_pid, server, 0);
  fatal(xkill, server->client_pid, FABIPC_SIGACK);

  finally : coda;
}

xapi fab_server_receive(fab_server * const restrict server, int daemon, fab_request ** const restrict request)
{
  enter;

  int token = 0;
  int sig;
  pid_t client_pid;
  int shmid = -1;
  int fd = -1;

//printf("server waiting for SYN(%d)\n", FABIPC_SIGSYN);
  fatal(sigbank_receive, &sig, &client_pid);
//printf("server received %d\n", sig);

  if(sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
  {
    *request = 0;
    goto XAPI_FINALIZE;
  }

  fatal(logger_streams_report);

  // validate the sender
  fatal(load_client_pid, server, client_pid);
  fatal(xkill, server->client_pid, 0);
  fatal(sigbank_assert, FABIPC_SIGSYN, sig, 0, client_pid);

  // redirect output to the client
  if(daemon)
    fatal(redirect, server);

  // get a fd to the clients cwd
  fatal(ixclose, &server->client_cwd_dirfd);
  fatal(xopenf, &server->client_cwd_dirfd, O_RDONLY, "%s/client/cwd", g_params.ipcdir);

  // get the request shm
  fatal(xopenf, &fd, O_RDONLY, "%s/client/request_shmid", g_params.ipcdir);
  fatal(axread, fd, &shmid, sizeof(shmid));
  fatal(xshmat, shmid, 0, 0, &server->request_shm);

  fab_request_thaw(server->request_shm, server->request_shm);
  *request = server->request_shm;

#if DEBUG || DEVEL || XAPI
  fatal(log_start, L_IPC, &token);
  logf(0, "request ");
  fatal(fab_request_say, *request, log_narrator(&token));
  fatal(log_finish, &token);
#endif

finally:
  fatal(log_finish, &token);
  fatal(ixclose, &fd);
coda;
}

xapi fab_server_respond(fab_server * const restrict server, memblk * const restrict mb, fab_response * const restrict response)
{
  enter;

  int token = 0;
  int shmid = -1;
  int fd = -1;
  int sig;
  pid_t pid;

  // release the request
  fatal(ixshmdt, &server->request_shm);

  // create the shm for the response
  fatal(xshmget, ftok(g_params.ipcdir, FABIPC_TOKRES), memblk_size(mb), IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &shmid);
  fatal(xshmat, shmid, 0, 0, &server->response_shm);
  fatal(xshmctl, shmid, IPC_RMID, 0);
  int shmid_tmp = shmid;
  shmid = -1;

  // save the shmid for the client
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/fabd/response_shmid", g_params.ipcdir);
  fatal(axwrite, fd, &shmid_tmp, sizeof(shmid_tmp));
  fatal(ixclose, &fd);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "response ");
  fatal(fab_response_say, response, log_narrator(&token));
  fatal(log_finish, &token);
#endif

  // write the response to shm
  fab_response_freeze(response, mb);
  memblk_copyto(mb, server->response_shm, memblk_size(mb));

  // awaken client and await response
  fatal(sigbank_exchange, FABIPC_SIGACK, server->client_pid, &sig, &pid);
  fatal(sigbank_assert, FABIPC_SIGACK, sig, server->client_pid, pid);

  // detach the response
  fatal(ixshmdt, &server->response_shm);

finally:
  fatal(log_finish, &token);
  if(shmid != -1)
    fatal(xshmctl, shmid, IPC_RMID, 0);
coda;
}

xapi fab_server_dispatch(fab_server * const restrict server, fab_request * const restrict request, memblk * const restrict mb, fab_response ** const restrict response)
{
  enter;

  char space[1024];
  int mpc = 0;

  // handle this request
  xapi exit = 0;
  if((exit = invoke(handle, request, mb, response)))
  {
    // propagate unrecoverable errors
    if(xapi_exit_errtab(exit) == perrtab_KERNEL)
    {
      fail(0);  // propagate unhandled errors
    }

    size_t tracesz = xapi_trace_full(space, sizeof(space));
    xapi_calltree_unwind();
    xlogw(L_ERROR, L_RED, space, tracesz);
  }

  if(*response == 0)
  {
    fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);
    fatal(fab_response_create, response, exit);
  }

finally:
  mempolicy_unwind(&mpc);
coda;
}
