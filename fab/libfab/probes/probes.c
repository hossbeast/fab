/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <wait.h>

#include "probes.h"

#include "xlinux/xsignal.h"

#include "usdt-sem.h"
#include "macros.h"
#include "zbuffer.h"

PROBE_SEM(sigutil, sigrx) = 1;
PROBE_SEM(sigutil, sigtx) = 1;

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

printf("%s\n", buf);
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

printf("%s\n", buf);
  PROBE4(sigutil, sigrx, buf, signo, pid, tid);
}
