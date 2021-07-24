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

#include <wait.h>

#include "probes.h"

#include "xlinux/xsignal.h"

#include "usdt-sem.h"
#include "macros.h"
#include "zbuffer.h"

PROBE_SEM(sigutil, sigrx);
PROBE_SEM(sigutil, sigtx);

void sigrx_probe(const siginfo_t * info)
{
  char buf[256];
  size_t sz;
  char *s;
  size_t z;

  if(!sigutil_sigrx_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += znloadf(s + z, sz - z, "rx %s { signo %d", signame(info->si_signo), info->si_signo);

  if(info->si_signo == SIGCHLD)
  {
    z += znloadf(s + z, sz - z, " pid %ld", (long)info->si_pid);

    if(WIFEXITED(info->si_status))
      z += znloadf(s + z, sz - z, " status %d", WEXITSTATUS(info->si_status));

    if(WIFSIGNALED(info->si_status))
    {
      z += znloadf(s + z, sz - z, " signal %d", WTERMSIG(info->si_status));
      z += znloadf(s + z, sz - z, " core %s", WCOREDUMP(info->si_status) ? "yes" : "no");
    }
  }
  else if(info->si_signo < SIGRTMIN)
  {
    z += znloadf(s + z, sz - z, " pid %ld", (long)info->si_pid);
  }
  else
  {
    z += znloadf(s + z, sz - z, " pid %ld value %lld", (long)info->si_pid, (long long)info->si_value.sival_ptr);
  }
  z += znloadf(s + z, sz - z, " }");

  PROBE2(sigutil, sigrx, buf, info);
}

void sigtx_probe(int signo, pid_t pid, pid_t tid)
{
  char buf[256];
  size_t sz;
  char *s;
  size_t z;

  if(!sigutil_sigtx_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += znloadf(s + z, sz - z, "tx %s { signo %d pid %ld tid %ld }", signame(signo), signo, (long)pid, (long)tid);

  PROBE4(sigutil, sigrx, buf, signo, pid, tid);
}
