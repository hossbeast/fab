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

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "xsignal/xsignal.h"
#include "errtab/KERNEL.errtab.h"
#include "sigtable.h"

xapi API xkill(pid_t pid, int sig)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, kill, pid, sig);

finally:
  xapi_infof("pid", "%ld", (long)pid);
  xapi_infof("sig", "%d", sig);
coda;
}

xapi API uxkill(pid_t pid, int sig, int * r)
{
  enter;

  if(r && ((*r) = kill(pid, sig)) == -1 && errno != ESRCH)
  {
    tfail(perrtab_KERNEL, errno);
  }
  else if(!r && kill(pid, sig) == -1 && errno != ESRCH)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}

xapi API xtgkill(pid_t pid, pid_t tid, int sig)
{
  enter;

  if(syscall(SYS_tgkill, pid, tid, sig) != 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("pid", "%ld", (long)pid);
  xapi_infof("tid", "%ld", (long)tid);
  xapi_infof("sig", "%d", sig);
coda;
}

xapi API uxtgkill(int * r, pid_t pid, pid_t tid, int sig)
{
  enter;

  if(tid == 0) {
    goto XAPI_FINALIZE;
  }

  if(r && (*r = syscall(SYS_tgkill, pid, tid, sig)) != 0 && errno != ESRCH && errno != EINVAL)
  {
    tfail(perrtab_KERNEL, errno);
  }
  else if(!r && syscall(SYS_tgkill, pid, tid, sig) != 0 && errno != ESRCH && errno != EINVAL)
  {
    tfail(perrtab_KERNEL, errno);
  }

finally:
  xapi_infof("pid", "%ld", (long)pid);
  xapi_infof("tid", "%ld", (long)tid);
  xapi_infof("sig", "%d", sig);
coda;
}

xapi API xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, sigaction, signum, act, oldact);

finally:
  xapi_infof("sig", "%d", signum);
coda;
}

xapi API xsigprocmask(int how, const sigset_t * set, sigset_t * oldset)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, sigprocmask, how, set, oldset);

  finally : coda;
}

xapi API uxsigsuspend(const sigset_t * mask)
{
  enter;

  if(sigsuspend(mask))
  {
    if(errno == EINTR)
    {
      /* the whole reason to use this function ... */
    }
    else
    {
      tfail(perrtab_KERNEL, errno);
    }
  }

  finally : coda;
}

xapi API xsigwaitinfo(const sigset_t * mask, siginfo_t * info)
{
  enter;

  if(sigwaitinfo(mask, info) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API uxsigwaitinfo(int * r, const sigset_t * mask, siginfo_t * info)
{
  enter;

  siginfo_t linfo;
  if(!info)
    info = &linfo;

  if(r && (((*r) = sigwaitinfo(mask, info)) == -1) && errno != EINTR)
    tfail(perrtab_KERNEL, errno);
  else if(!r && sigwaitinfo(mask, info) == -1 && errno != EINTR)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API xsignal(int signum, sighandler_t handler)
{
  enter;

  if(signal(signum, handler) == SIG_ERR)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("sig", "%d", signum);
coda;
}

const char * API signame(int signo)
{
  if(signo < 0 || signo > sigtable_max) {
    return 0;
  }

  return sigtable[signo];
}
