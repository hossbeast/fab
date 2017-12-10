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

#include <sys/syscall.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"

#include "fab/ipc.h"
#include "fab/request.def.h"
#include "fab/command.def.h"
#include "fab/response.h"
#include "fab/sigutil.h"
#include "logger/stream.h"
#include "logger/config.h"
#include "narrator.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "moria/graph.h"

#include "server_thread.h"
#include "config.h"
#include "errtab/FABD.errtab.h"
#include "extern.h"
#include "handler.h"
#include "logging.h"
#include "module.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "reconfigure.h"
#include "usage.h"
#include "walker.internal.h"

#include "macros.h"
#include "memblk.def.h"
#include "memblk.h"
#include "atomic.h"

int server_thread_rebuild;

/// load_client_pid
//
// SUMMARY
//  load the client pid
//
static xapi load_client_pid(pid_t * client_pid)
{
  enter;

  int fd = -1;

  // load fab/pid
  fatal(xopenf, &fd, O_RDONLY, "%s/client/pid", g_params.ipcdir);
  fatal(axread, fd, client_pid, sizeof(*client_pid));
  if(*client_pid <= 0)
  {
    xapi_info_pushs("expected client pid", "> 0");
    xapi_info_pushf("actual client pid", "%ld", (long)*client_pid);
    fail(FABD_BADIPC);
  }

finally:
  fatal(ixclose, &fd);
coda;
}

#if DEBUG || DEVEL
static xapi redirect()
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
#endif

static xapi receive(void ** request_shm, fab_request ** const restrict request)
{
  enter;

  int shmid = -1;
  int fd = -1;

#if DEBUG || DEVEL
  if(!g_server_no_acknowledge) 
    fatal(redirect);
#endif

  // get the request shm
  fatal(xopenf, &fd, O_RDONLY, "%s/client/request_shmid", g_params.ipcdir);
  fatal(axread, fd, &shmid, sizeof(shmid));
  fatal(xshmat, shmid, 0, 0, request_shm);

  fab_request_thaw(*request_shm, *request_shm);
  *request = *request_shm;

#if DEBUG || DEVEL || XAPI
  narrator * N;
  fatal(log_start, L_PROTOCOL, &N);
  xsayf("request ");
  fatal(fab_request_say, *request, N);
  fatal(log_finish);
#endif

finally:
  fatal(ixclose, &fd);
coda;
}

static xapi respond(pid_t client_pid, const sigset_t * sigs, void ** response_shm, memblk * const restrict mb, fab_response * const restrict response)
{
  enter;

  int shmid = -1;
  int fd = -1;
  siginfo_t siginfo;

  // create the shm for the response
  fatal(xshmget, ftok(g_params.ipcdir, FABIPC_TOKRES), memblk_size(mb), IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &shmid);
  fatal(xshmat, shmid, 0, 0, response_shm);
  fatal(xshmctl, shmid, IPC_RMID, 0);
  int shmid_tmp = shmid;
  shmid = -1;

  // save the shmid for the client
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/fabd/response_shmid", g_params.ipcdir);
  fatal(axwrite, fd, &shmid_tmp, sizeof(shmid_tmp));
  fatal(ixclose, &fd);

#if DEBUG || DEVEL
  narrator * N;
  fatal(log_start, L_PROTOCOL, &N);
  xsayf("response ");
  fatal(fab_response_say, response, N);
  fatal(log_finish);
#endif

  // write the response to shm
  fab_response_freeze(response, mb);
  memblk_copyto(mb, *response_shm, memblk_size(mb));

  // awaken client and await response
  fatal(sigutil_exchange, FABIPC_SIGACK, client_pid, sigs, &siginfo);
  fatal(sigutil_assert, FABIPC_SIGACK, client_pid, &siginfo);

finally:
  if(shmid != -1)
    fatal(xshmctl, shmid, IPC_RMID, 0);
coda;
}

static xapi server_thread()
{
  enter;

  node * project_root = 0;
  pid_t client_pid = 0; // client process id
  memblk * mb = 0;
  sigset_t sigs;
  siginfo_t siginfo;
  void * request_shm = 0;
  void * response_shm = 0;

  g_params.thread_server = gettid();
  fatal(memblk_mk, &mb);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, FABIPC_SIGSYN);
  sigaddset(&sigs, FABIPC_SIGACK);
  sigaddset(&sigs, FABIPC_SIGSCH);

  // load the config
  fatal(reconfigure_stage_files);
  fatal(reconfigure);

  // load the filesystem rooted at the module dir
  fatal(walker_walk, &project_root, 0, g_params.proj_dir, 0);

  // load the module in this directory (and nested modules, recursively)
  fatal(module_load_project, project_root, g_params.proj_dir);

  // signal to the client readiness to receive requests
#if DEBUG || DEVEL
  if(!g_server_no_acknowledge)
  {
#endif
    fatal(load_client_pid, &client_pid);
    fatal(xkill, client_pid, FABIPC_SIGACK);
#if DEBUG || DEVEL
  }
#endif

  while(!g_params.shutdown)
  {
    if(server_thread_rebuild)
    {
      fatal(handler_build);
      server_thread_rebuild = 0;
    }

    // receive a signal
    fatal(sigutil_wait, &sigs, &siginfo);
    if(siginfo.si_signo == FABIPC_SIGSCH)
      continue;

    fatal(logger_streams_report);

    // validate the sender
    fatal(load_client_pid, &client_pid);
    fatal(xkill, client_pid, 0);
    fatal(sigutil_assert, FABIPC_SIGSYN, client_pid, &siginfo);

    // receive the request
    fab_request * request = 0;
    fatal(receive, &request_shm, &request);

    // process and release the request
    fab_response * response = 0;
    fatal(handler_dispatch, request, mb, &response);
    fatal(ixshmdt, &request_shm);

    // send and release the response
    fatal(respond, client_pid, &sigs, &response_shm, mb, response);
    fatal(ixshmdt, &response_shm);

    //fatal(node_dump);
    fatal(usage_report);
  }

finally:
  // locals
  memblk_free(mb);
  fatal(ixshmdt, &request_shm);
  fatal(ixshmdt, &response_shm);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * server_thread_main(void * arg)
{
  enter;

  xapi R;

  logger_set_thread_name("server");
  logger_set_thread_categories(L_SERVER);
  fatal(server_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd/server");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);
  return 0;
}

//
// public
//

xapi server_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, server_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
