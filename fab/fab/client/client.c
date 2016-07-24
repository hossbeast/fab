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

#include "xapi.h"
#include "xapi/trace.h"

#include "xlinux/xfcntl.h"
#include "xlinux/SYS.errtab.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstat.h"
#include "xlinux/xstring.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"

#include "narrator.h"
#include "narrator/units.h"
#include "logger.h"
#include "logger/arguments.h"

#include "fabcore.h"
#include "fabcore/sigbank.h"
#include "fabcore/request.h"
#include "fabcore/response.h"
#include "fabcore/ipc.h"
#include "fabcore/identity.h"
#include "fabcore/params.h"

#include "global.h"
#include "client.h"
#include "logging.h"

#include "memblk.h"
#include "memblk.def.h"
#include "macros.h"

#define restrict __restrict

struct client
{
  char *    ipcdir;
  char      hash[8];
  pid_t     pgid;       // fabd process group id

  int       shmid;
  void *    shmaddr;    // request shm
};

//
// static
//

static xapi pgid_load(client * const restrict client)
{
  enter;

  char space[512];
  int fd = -1;

  client->pgid = 0;

  snprintf(space, sizeof(space), "%s/fabd/pgid", client->ipcdir);
  fatal(uxopen, space, O_RDONLY, &fd);

  if(fd != -1)
  {
    // read and validate the pid
    fatal(axread, fd, &client->pgid, sizeof(client->pgid));
    if(client->pgid <= 0)
    {
      xapi_fail_intent();
      xapi_info_adds("expected pgid", "> 0");
      xapi_info_addf("actual pgid", "%ld", (long)client->pgid);
      fail(FAB_BADIPC);
    }
  }

finally:
  fatal(ixclose, &fd);
coda;
}

//
// public
//

xapi client_create(client ** const restrict client, char * const restrict ipcdir, const char hash[8])
{
  enter;

  fatal(xmalloc, client, sizeof(**client));
  fatal(ixstrdup, &(*client)->ipcdir, ipcdir); 
  memcpy((*client)->hash, hash, sizeof((*client)->hash));

  // lazy destroy the request shm
  fatal(uxshmget, ftok((*client)->ipcdir, FABIPC_REQUEST), 0, 0, &(*client)->shmid);
  if((*client)->shmid != -1)
    fatal(xshmctl, (*client)->shmid, IPC_RMID, 0);
  (*client)->shmid = -1;

  finally : coda;
}

xapi client_dispose(client ** const restrict client)
{
  enter;

  if(*client)
  {
    free((*client)->ipcdir);
    fatal(ixshmdt, &(*client)->shmaddr);

    if((*client)->shmid != -1)
      fatal(xshmctl, (*client)->shmid, IPC_RMID, 0);
    (*client)->shmid = -1;

    free(*client);
  }

  *client = 0;

  finally : coda;
}

xapi client_verify(client * const restrict client)
{
  enter;

  char space[2048];
  int exit;
  int error;
  int fd = -1;

  // check child exit file
  snprintf(space, sizeof(space), "%s/fabd/exit", client->ipcdir);
  fatal(ixclose, &fd);
  fatal(uxopen, space, O_RDONLY, &fd);

  if(fd != -1)
  {
    fatal(axread, fd, MM(exit));
    if(WIFEXITED(exit) || WIFSIGNALED(exit))
    {
      xapi_fail_intent();

      if(WIFEXITED(exit))
        xapi_infof("status", "%d", WEXITSTATUS(exit));

      if(WIFSIGNALED(exit))
        xapi_infof("signal", "%d", WTERMSIG(exit));
      
      fail(FAB_FABDFAIL);
    }
  }

  // check child error file
  snprintf(space, sizeof(space),"%s/fabd/error", client->ipcdir);
  fatal(ixclose, &fd);
  fatal(uxopen, space, O_RDONLY, &fd);

  if(fd != -1)
  {
    // propagate
    fatal(axread, fd, MM(error));
    if(error)
      fail(error);
  }

  // check that child is still running
  fatal(xkill, -client->pgid, 0);

finally:
  fatal(ixclose, &fd);
coda;
}

xapi client_terminatep(client * const restrict client)
{
  enter;

  fatal(pgid_load, client);
  if(client->pgid)
  {
    // existence assertion
    fatal(xkill, -client->pgid, 0);

 //   fatal(sigbank_exchange, client->pgid, 15, (int[]) { FABSIG_DONE, 0 }, 0, 0, 0);
  }

  finally : coda;
}

xapi client_launchp(client * const restrict client)
{
  enter;

  int token = 0;

  // attempt to get the fabd lock
  while(1)
  {
    fatal(ipc_lock_obtain, &client->pgid, "%s/fabd/lock", client->ipcdir);

    // lock obtained
    if(client->pgid == 0)
      break;

    int r;
    fatal(uxkill, -client->pgid, 0, &r);

    // already running
    if(r == 0)
      break;

    // not running ; forcibly release the lock, try again
    fatal(ipc_lock_release, "%s/fabd/lock", client->ipcdir);
  }

  if(client->pgid == 0)
  {
    fatal(identity_assume_fabsys);
    fatal(xfork, &client->pgid);
    if(client->pgid == 0)
    {
      // create new session/process group and update the lock
      fatal(xsetsid);
      fatal(ipc_lock_update, "%s/fabd/lock", client->ipcdir);
      fatal(xchdir, "/");

#if DEVEL
      char space[2048];
      snprintf(space, sizeof(space), "%s/../fabw/fabw.devel", g_params.exedir);

      char * argv[] = { space, client->hash, (void*)0 };

      fatal(log_start, L_IPC, &token);
      logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fab");
      off_t off = 0;
      fatal(narrator_seek, log_narrator(&token), 0, NARRATOR_SEEK_CUR, &off);
      logf(0, "%*s execv(", MAX(0, 20 - off), "");
      int x;
      for(x = 0; x < sentinel(argv); x++)
      {
        if(x)
          logs(0, ",");
        logs(0, argv[x]);
      }
      logs(0, ")");
      fatal(log_finish, &token);

      execv(space, argv);
#else
      execlp("fabw", "fabw", client->hash, (void*)0);
#endif

      // exec failed
      tfail(perrtab_SYS, errno);
    }

    // await ready signal from the child
    fatal(sigbank_receive, (int[]) { FABSIG_ACK, 0 }, client->pgid, 0, 0);
    fatal(identity_assume_user);
  }

finally:
  fatal(log_finish, &token);
coda;
}

xapi client_make_request(client * const restrict client, memblk * const restrict mb, request * const restrict req)
{
  enter;

  response * res = 0;
  int token = 0;

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fab");
  off_t off = 0;
  fatal(narrator_seek, log_narrator(&token), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s request ", MAX(0, 20 - off), "");
  fatal(request_say, req, log_narrator(&token));
  fatal(log_finish, &token);
#endif

  // create shm for the request
  request_freeze(req, mb);
  fatal(xshmget, ftok(client->ipcdir, FABIPC_REQUEST), memblk_size(mb), IPC_CREAT | IPC_EXCL | FABIPC_DATA, &client->shmid);
  fatal(xshmat, client->shmid, 0, 0, &client->shmaddr);

  // write the request to shm
  memblk_copyto(mb, client->shmaddr, memblk_size(mb));

  // awaken fabd and await response
  fatal(sigbank_exchange, -client->pgid, FABSIG_SYN, (int[]) { FABSIG_ACK, 0 }, 0, 0, 0);

  // destroy the request
  fatal(xshmctl, client->shmid, IPC_RMID, 0);
  client->shmid = -1;
  fatal(ixshmdt, &client->shmaddr);

  // attach to shm for the response
  int shmid;
  fatal(xshmget, ftok(client->ipcdir, FABIPC_RESPONSE), 0, 0, &shmid);
  fatal(xshmat, shmid, 0, 0, &client->shmaddr);

  // consume the response
  res = client->shmaddr;
  response_thaw(res, client->shmaddr);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fab");
  fatal(narrator_seek, log_narrator(&token), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s response ", MAX(0, 20 - off), "");
  fatal(response_say, res, log_narrator(&token));
  fatal(log_finish, &token);
#endif

  // notify fabd
  fatal(xkill, -client->pgid, FABSIG_ACK);

finally:
  fatal(log_finish, &token);
coda;
}
