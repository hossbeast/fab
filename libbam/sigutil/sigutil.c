/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/wait.h>
#include <string.h>

#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "xlinux/xwait.h"
#include "narrator.h"

#include "sigutil.h"
#include "ipc.h"
#include "probes.h"

#include "macros.h"
#include "zcat.h"

#define SIGUTIL_HANDLER_DEBUG 0

bool APIDATA g_sigterm;

static void term_handler(int signum, siginfo_t *info, void *ctx)
{
  g_sigterm = true;

  sigrx_probe(info);
}

static void intr_handler(int signum, siginfo_t * info, void * ctx)
{
  sigrx_probe(info);
}

void API sigutil_install_handlers()
{
  struct sigaction intr_act;
  struct sigaction term_act;

  memset(&intr_act, 0, sizeof(intr_act));
  intr_act.sa_flags = SA_SIGINFO;
  intr_act.sa_sigaction = intr_handler;

  memset(&term_act, 0, sizeof(term_act));
  term_act.sa_flags = SA_SIGINFO;
  term_act.sa_sigaction = term_handler;

  xsigaction(SIGINT, &term_act, 0);
  xsigaction(SIGTERM, &term_act, 0);
  xsigaction(SIGQUIT, &term_act, 0);
  xsigaction(SIGCHLD, &intr_act, 0);
  xsigaction(SIGUSR1, &intr_act, 0);

  /* SIGRTMIN - carries int-valued signal from the client */
  xsigaction(SIGRTMIN, &intr_act, 0);
}

void API sigutil_wait(const sigset_t * sigs, siginfo_t * info)
{
  siginfo_t linfo;
  int r;

  if(!info) {
    info = &linfo;
  }

  // retry EINTR
  r = -1;
  while(r == -1) {
    r = uxsigwaitinfo(sigs, info);
  }

/* sigwaitinfo does *not* cause the signal handler to be invoked */
/* TODO : move this waitpid out into callers if any */
  if(info->si_signo == SIGCHLD)
  {
    // sigwaitinfo returns only part of the child status
    xwaitpid(info->si_pid, &info->si_status, 0);
  }

  sigrx_probe(info);
}

int API sigutil_timedwait(const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
{
  int err;

  err = uxsigtimedwait(sigs, info, timeout);

  if(err == 0) {
    sigrx_probe(info);
  } else {
    memset(info, 0, sizeof(*info));
  }

  return err;
}

void API sigutil_exchange(int sig, pid_t pid, const sigset_t * sigs, siginfo_t * info)
{
  // signal target
  xkill(pid, sig);

  // receive a signal in sigs
  sigutil_wait(sigs, info);
}

void API sigutil_kill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);
  xkill(pid, signo);
}

int API sigutil_uxkill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);

  return uxkill(pid, signo);
}

void API sigutil_tgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  xtgkill(pid, tid, signo);
}

int API sigutil_uxtgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  return uxtgkill(pid, tid, signo);
}

int API sigutil_uxrt_sigqueueinfo(pid_t pid, int signo, union sigval value)
{
  sigtx_probe(signo, pid, 0);

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  return uxrt_sigqueueinfo(pid, signo, &info);
}

int API sigutil_uxrt_tgsigqueueinfo(pid_t pid, pid_t tid, int signo, union sigval value)
{
  sigtx_probe(signo, pid, tid);

  siginfo_t info;
  memset(&info, 0, sizeof(info));
  info.si_code = -10;
  info.si_pid = getpid();
  info.si_uid = getuid();
  info.si_value = value;

  return uxrt_tgsigqueueinfo(pid, tid, signo, &info);
}
