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
#include "xlinux/xsignal.h"

#include "ipc.h"
#include "pipe.h"
#include "macros.h"

typedef struct ipc_test {
  XUNITTEST;

  uint32_t producer_count;
  uint32_t message_count;
} ipc_test;

static struct bamipc_pipe bampipe;
static pid_t base_pid;
static pid_t base_tid;
static uint32_t threads;

typedef struct record {
  uint32_t prod_slot;
  uint32_t prod_index;
  uint32_t cons_slot;
  uint32_t cons_index;
} record;

static void sighandler(int signo, siginfo_t *info, void *ctx)
{
}

static void *producer(void *ctx)
{
  int x;
  bamipc_message *msg;
  ipc_test *test = ctx;
  record *rec;

  for(x = 0; x < test->message_count; x++) {
    bamipc_tx(&bampipe, 0, &msg);
    rec = (void*)msg->text;
    rec->prod_slot = msg->slot;
    rec->prod_index = msg->index;
    bamipc_publish(&bampipe, msg);
  }

  atomic_fetch_dec(&threads);
  syscall(SYS_tgkill, base_pid, base_tid, SIGUSR1);
  return 0;
}

static void *consumer(void *ctx)
{
  bamipc_message *msg;
  ipc_test *test = ctx;
  uint32_t n;
  record *rec;

  for(n = 0; n < (test->message_count * test->producer_count); n++) {
    bamipc_rx(&bampipe, 0, &msg);
    rec = (void*)msg->text;
    rec->cons_slot = msg->slot;
    rec->cons_index = msg->index;
    bamipc_release(&bampipe, msg);
  }

  atomic_fetch_dec(&threads);
  syscall(SYS_tgkill, base_pid, base_tid, SIGUSR1);
  return 0;
}

static void ipc_test_entry(xunit_test * _test)
{
  ipc_test __attribute__((unused)) *test;
  pthread_t pthread_id;
  pthread_attr_t attr = {};
  sigset_t sigs;
  siginfo_t info;
  struct sigaction act;
  bamipc_message *msg;
  uint32_t index;
  uint32_t x;
  record *rec;

  test = containerof(_test, ipc_test, xu);

  bamipc_pipe_init(&bampipe);

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
  pthread_create(&pthread_id, &attr, consumer, test);
  threads = 1;

  for(x = 0; x < test->producer_count; x++) {
    pthread_create(&pthread_id, &attr, producer, test);
  }
  threads += x;

  pthread_attr_destroy(&attr);

  while(threads) {
    uxsigwaitinfo(&sigs, &info);
  }

  for(x = 0; x < MIN(test->message_count * test->producer_count, 512); x++) {
    index = x;
    msg = &bampipe.messages[index];
    rec = (void*)msg->text;
    assert_eq_u32(x, rec->prod_index);
    assert_eq_u32(x, rec->cons_index);
  }
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : ipc_test_entry
  , xu_tests : (void*)(ipc_test*[]) {
      /* basic functional tests */
        /* empty */
        (ipc_test[]){{
            message_count : 512
          , producer_count : 2
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
