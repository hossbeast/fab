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

bool g_sigterm;
bool g_sigquit;

static void quit_handler(int signum, siginfo_t *info, void *ctx)
{
  g_sigquit = true;
  sigrx_probe(info);
}

static void term_handler(int signum, siginfo_t *info, void *ctx)
{
  g_sigterm = true;
  sigrx_probe(info);
}

static void intr_handler(int signum, siginfo_t * info, void * ctx)
{
  sigrx_probe(info);
}

void sigutil_install_handlers()
{
  struct sigaction intr_act;
  struct sigaction quit_act;
  struct sigaction term_act;

  memset(&intr_act, 0, sizeof(intr_act));
  intr_act.sa_flags = SA_SIGINFO;
  intr_act.sa_sigaction = intr_handler;

  memset(&term_act, 0, sizeof(term_act));
  term_act.sa_flags = SA_SIGINFO;
  term_act.sa_sigaction = term_handler;

  memset(&quit_act, 0, sizeof(quit_act));
  quit_act.sa_flags = SA_SIGINFO;
  quit_act.sa_sigaction = quit_handler;

  /* QUIT - interrupt system call if any, then exit immediately */
  xsigaction(SIGQUIT, &quit_act, 0);

  /* TERM - interrupt system call if any, finish pending work, then exit */
  xsigaction(SIGINT, &term_act, 0);
  xsigaction(SIGTERM, &term_act, 0);

  /* USR1 - interrupt system call if any */
  xsigaction(SIGCHLD, &intr_act, 0);
  xsigaction(SIGUSR1, &intr_act, 0);

  /* SIGRTMIN - carries int-valued signals */
  xsigaction(SIGRTMIN, &intr_act, 0);
}

void sigutil_wait(const sigset_t * sigs, siginfo_t * info)
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

int sigutil_timedwait(const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
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

void sigutil_exchange(int sig, pid_t pid, const sigset_t * sigs, siginfo_t * info)
{
  // signal target
  xkill(pid, sig);

  // receive a signal in sigs
  sigutil_wait(sigs, info);
}

void sigutil_kill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);
  xkill(pid, signo);
}

int sigutil_uxkill(pid_t pid, int signo)
{
  sigtx_probe(signo, pid, 0);

  return uxkill(pid, signo);
}

void sigutil_tgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  xtgkill(pid, tid, signo);
}

int sigutil_uxtgkill(pid_t pid, pid_t tid, int signo)
{
  sigtx_probe(signo, pid, tid);

  return uxtgkill(pid, tid, signo);
}

int sigutil_uxrt_sigqueueinfo(pid_t pid, int signo, union sigval value)
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

int sigutil_uxrt_tgsigqueueinfo(pid_t pid, pid_t tid, int signo, union sigval value)
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
