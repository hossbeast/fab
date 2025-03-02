/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "xunit.h"
#include "xunit/assert.h"
#include "xlinux/xpthread.h"
#include "bacon/load.h"
#include "moria/load.h"
#include "xlinux/xsignal.h"

#include "fsent.h"
#include "shadow.h"
#include "fstree.h"

#include "rcu.h"
#include "macros.h"
#include "times.h"
#include "vertex.h"
#include "subsys.h"

typedef struct rcu_test {
  XUNITTEST;

  uint32_t threads;
} rcu_test;

static pid_t base_pid;
static pid_t base_tid;
static uint32_t threads;

static void sighandler(int signo, siginfo_t *info, void *ctx)
{
}

static void *worker(void *ctx)
{
  rcu_thread rcu;
  int x;
  int y;
  struct timespec duration;

  tid = gettid();

  for(y = 0; y < 3; y++)
  {
    memset(&duration, 0, sizeof(duration));
    timespec_add(&duration, random() % MSEC_AS_NSEC(10));
    clock_nanosleep(CLOCK_MONOTONIC, 0, &duration, 0);

    rcu_enter(&rcu);

    for(x = 0; x < 10; x++) {
      rcu_quiesce(&rcu);

      memset(&duration, 0, sizeof(duration));
      timespec_add(&duration, random() % MSEC_AS_NSEC(100));
      clock_nanosleep(CLOCK_MONOTONIC, 0, &duration, 0);
    }

    rcu_leave(&rcu);
  }

  atomic_fetch_dec(&threads);
  syscall(SYS_tgkill, base_pid, base_tid, SIGUSR1);
  return 0;
}

static void *monitor(void *ctx)
{
  rcu_thread rcu;
  struct timespec duration;
  int x;

  tid = gettid();

  memset(&rcu, 0, sizeof(rcu));
  for(x = 0; x < 10; x++) {
    memset(&duration, 0, sizeof(duration));
    timespec_add(&duration, random() % MSEC_AS_NSEC(100));
    clock_nanosleep(CLOCK_MONOTONIC, 0, &duration, 0);

    rcu_synchronize();
  }

  atomic_fetch_dec(&threads);
  syscall(SYS_tgkill, base_pid, base_tid, SIGUSR1);
  return 0;
}

static void rcu_test_setup(xunit_unit * unit)
{
  bacon_load();

  fsent_root_setup();
  shadow_root_setup();
  fstree_root_setup();
  shadow_setup();
  fstree_setup();
  graph_setup();

  rcu_setup();

  extern __thread bool rcu_is_reader ; rcu_is_reader = true;
}

static void rcu_test_cleanup(xunit_unit * unit)
{
  bacon_unload();

  shadow_cleanup();
  fstree_cleanup();
  module_cleanup();
  fsent_cleanup();
  graph_cleanup();
}

static void rcu_test_entry(xunit_test * _test)
{
  rcu_test __attribute__((unused)) *test;
  pthread_t pthread_id;
  pthread_attr_t attr = {};
  sigset_t sigs;
  siginfo_t info;
  struct sigaction act;
  uint32_t x;

  test = containerof(_test, rcu_test, xu);

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  xpthread_sigmask(SIG_BLOCK, &sigs, 0);

  memset(&act, 0, sizeof(act));
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = sighandler;
  xsigaction(SIGUSR1, &act, 0);

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  base_pid = getpid();
  base_tid = gettid();

  threads = 0;
  for(x = 0; x < test->threads; x++) {
    if(x == (test->threads / 2)) {
      /* start the monitor in the middle */
      pthread_create(&pthread_id, &attr, monitor, test);
      threads++;
    }
    pthread_create(&pthread_id, &attr, worker, test);
    threads++;
  }

  pthread_attr_destroy(&attr);

  while(threads) {
    uxsigwaitinfo(&sigs, &info);
  }

  /* nondeterministic, no assertions */
  vertex_show(&sys_rcu.vertex);
}

//
// public
//

static xunit_unit xunit = {
    xu_setup : rcu_test_setup
  , xu_cleanup : rcu_test_cleanup
  , xu_entry : rcu_test_entry
  , xu_tests : (void*)(rcu_test*[]) {
      /* basic functional tests */
        /* empty */
        (rcu_test[]){{
            threads : 64
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
