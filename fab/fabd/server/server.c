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

#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"

#include "xlinux/mempolicy.h"
#include "listwise/LISTWISE.errtab.h"
#include "listwise/PCRE.errtab.h"
#include "narrator.h"
#include "logger.h"

#include "fabcore/params.h"
#include "fabcore/logging.h"
#include "fabcore/request.h"
#include "fabcore/response.h"
#include "fabcore/ipc.h"
#include "fabcore/sigbank.h"

#include "global.h"
#include "server.h"

#include "memblk.h"
#include "memblk.def.h"
#include "macros.h"

// errors to report and continue
static int fab_trap[] = {
    [ ERRMAX_FAB + 1] = 0

  , [ FAB_SYNTAX ] = 1
  , [ FAB_ILLBYTE ] = 1
  , [ FAB_UNSATISFIED ] = 1
  , [ FAB_CMDFAIL ] = 1
  , [ FAB_NOINVOKE ] = 1
  , [ FAB_BADPLAN ] = 1
  , [ FAB_CYCLE ] = 1
  , [ FAB_NOSELECT ] = 1
  , [ FAB_DSCPARSE ] = 1
};

//
// static
//

/// pid_load
//
// SUMMARY
//  load the client pid
//
static xapi pid_load(server * const restrict server)
{
  enter;

  int fd = -1;
  char space[512];

  // load fab/pid
  snprintf(space, sizeof(space), "%s/client/pid", server->ipcdir);
  fatal(xopen, space, O_RDONLY, &fd);
  fatal(axread, fd, &server->pid, sizeof(server->pid));
  if(server->pid <= 0)
  {
    xapi_fail_intent();
    xapi_info_adds("expected pid", "> 0");
    xapi_info_addf("actual pid", "%ld", (long)server->pid);
    fail(FAB_BADIPC);
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
static int handle(request * const restrict req, memblk * const restrict mb, response ** const restrict resp)
{
  enter;

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

//
// public
//

xapi server_create(server ** const restrict server, char * const restrict ipcdir, const char hash[8])
{
  enter;

  fatal(xmalloc, server, sizeof(**server));
  fatal(ixstrdup, &(*server)->ipcdir, ipcdir); 
  memcpy((*server)->hash, hash, sizeof((*server)->hash));

  // lazy destroy the response shm
  fatal(uxshmget, ftok((*server)->ipcdir, FABIPC_RESPONSE), 0, 0, &(*server)->shmid);
  if((*server)->shmid != -1)
    fatal(xshmctl, (*server)->shmid, IPC_RMID, 0);
  (*server)->shmid = -1;

  finally : coda;
}

xapi server_dispose(server ** const restrict server)
{
  enter;

  if(*server)
  {
    ifree(&(*server)->ipcdir);
    fatal(ixshmdt, &(*server)->shmaddr);

    if((*server)->shmid != -1)
      fatal(xshmctl, (*server)->shmid, IPC_RMID, 0);
    (*server)->shmid = -1;

    xfree(*server);
  }

  *server = 0;

  finally : coda;
}

xapi server_ready(server * const restrict server)
{
  enter;

  fatal(pid_load, server);
  fatal(xkill, server->pid, FABSIG_ACK);

  finally : coda;
}

xapi server_validate(server * const restrict server, pid_t pid)
{
  enter;

  // verify the client
  fatal(pid_load, server);
  fatal(xkill, server->pid, 0);

  if(pid != server->pid)
  {
    xapi_fail_intent();

    fail(FAB_BADIPC);
  }

  finally : coda;
}

xapi server_redirect(server * const restrict server)
{
  enter;

  int fd = -1;
  char space[512];

  // redirect stdout/stderr to the client
  snprintf(space, sizeof(space), "%s/client/out", server->ipcdir);
  fatal(xopen, space, O_RDWR, &fd);
  fatal(xdup2, fd, 1);
  fatal(ixclose, &fd);

  snprintf(space, sizeof(space), "%s/client/err", server->ipcdir);
  fatal(xopen, space, O_RDWR, &fd);
  fatal(xdup2, fd, 2);

finally:
  fatal(ixclose, &fd);
coda;
}

xapi server_receive(server * const restrict server, request ** const restrict req)
{
  enter;

  int token = 0;

  // get the request shm
  int shmid;
  fatal(xshmget, ftok(server->ipcdir, FABIPC_REQUEST), 0, 0, &shmid);
  fatal(xshmat, shmid, 0, 0, &server->shmaddr);

  *req = server->shmaddr;
  request_thaw(*req, server->shmaddr);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fabd");
  off_t off = 0;
  fatal(narrator_seek, log_narrator(), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s request ", MAX(0, 20 - off), "");
  fatal(request_say, *req, log_narrator());
  fatal(log_finish, &token);
#endif

finally:
  fatal(log_finish, &token);
coda;
}

xapi server_respond(server * const restrict server, memblk * const restrict mb, response * const restrict resp)
{
  enter;

  int token = 0;

  // release the request
  fatal(ixshmdt, &server->shmaddr);

  // create the shm for the response
  fatal(xshmget, ftok(server->ipcdir, FABIPC_RESPONSE), memblk_size(mb), IPC_CREAT | IPC_EXCL | FABIPC_DATA, &server->shmid);
  fatal(xshmat, server->shmid, 0, 0, &server->shmaddr);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fabd");
  off_t off = 0;
  fatal(narrator_seek, log_narrator(), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s response ", MAX(0, 20 - off), "");
  fatal(response_say, resp, log_narrator());
  fatal(log_finish, &token);
#endif

  // write the response to shm
  response_freeze(resp, mb);
  memblk_copyto(mb, server->shmaddr, memblk_size(mb));

  // awaken client and await response
  fatal(sigbank_exchange, server->pid, FABSIG_SYN, (int[]) { FABSIG_ACK, 0 }, 0, 0, 0);

  // destroy the response
  fatal(xshmctl, server->shmid, IPC_RMID, 0);
  server->shmid = -1;
  fatal(ixshmdt, &server->shmaddr);

finally:
  fatal(log_finish, &token);
coda;
}

xapi server_release(server * const restrict server)
{
  enter;

  if(server->shmaddr)
    fatal(ixshmdt, &server->shmaddr);

  finally : coda;
}

xapi server_dispatch(server * const restrict server, request * const restrict req, memblk * const restrict mb, response ** const restrict resp)
{
  enter;

  int mpc = 0;

  fatal(xchdirf, "%s/client/cwd", server->ipcdir);

  // handle this request
  xapi exit = 0;
  if((exit = invoke(handle, req, mb, resp)))
  {
    if(xapi_exit_errtab(exit) == perrtab_FAB && fab_trap[xapi_exit_errcode(exit)])
    {
    }
    else if(xapi_exit_errtab(exit) == perrtab_LISTWISE)
    {
    }
    else if(xapi_exit_errtab(exit) == perrtab_PCRE)
    {
    }
    else
    {
      fail(0);  // propagate unhandled errors
    }

    char trace[512];
    size_t tracesz = xapi_trace_full(trace, sizeof(trace));
    xapi_calltree_unwind();

    // build an error response
    fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);
    fatal(response_create, resp);
    fatal(response_error, *resp, exit, trace, tracesz);
  }

finally:
  fatal(xchdir, "/");
  mempolicy_unwind(&mpc);
coda;
}
