/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <dirent.h>

#include "valyria/llist.h"

#include "bam/load.h"
#include "narrator/load.h"
#include "bacon/load.h"

#include "xlinux/xfutex.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "xlinux/xepoll.h"

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/events.h"
#include "bam/ipc.h"
#include "bam/sigutil.h"
#include "bam/lockfile.h"

#include "args.h"
#include "build.h"
#include "command.h"
#include "kill.h"
#include "params.h"
#include "info.h"
#include "params.h"

#include "barriers.h"
#include "times.h"

#if CONFIG_DEVEL
#include "backtracef.h"
#endif

__thread int32_t tid;
__thread int *errnop;
static uint64_t eventsubid;

static void client_acquired(bam_client * restrict client, bamipc_message * restrict msg)
{
  uint32_t h;
  uint32_t i;
  char buf[64];
  size_t z;
  struct bamipc_page *page;

  if(g_args.verbose < 2) {
    return;
  }

  page = containerof(msg, bamipc_page, msg);
  h = page->head;
  i = page - client->shm->server_ring.pages;
  z = attrs32_zcat(buf, sizeof(buf), bamipc_msg_type_attrs, msg->type);
  printf("rx head %5u idx %5u id 0x%016"PRIx64" code %8d type %.*s", h, i, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), &bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu", msg->size);
  if(g_args.verbose >= 3) {
    printf(" %.*s", (int)msg->size, msg->text);
  }
  if(msg->type == BAMIPC_MSG_RESPONSE || msg->type == BAMIPC_MSG_EVENTS)
  {
    switch(msg->evtype) {
      case BAMIPC_EVENT_GOALS_SET:
      case BAMIPC_EVENT_SYSTEM_STATE_CHANGE:
        printf(" %.*s", (int)msg->size, msg->text);
      break;
    }
  }
  printf("\n");
}

void client_post(struct bam_client * restrict client, bamipc_message * restrict msg)
{
  bam_client_post(client, msg);

  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose < 1) {
    return;
  }

  h = containerof(msg, bamipc_page, msg)->tail;
  z = attrs32_zcat(buf, sizeof(buf), bamipc_msg_type_attrs, msg->type);
  printf("tx tail %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), &bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu ", msg->size);
  printf("%.*s", (int)msg->size, msg->text);
  printf("\n");
}

struct proc {
  llist lln;
  int pfd;
  int64_t pid;
  char bamd_id[64];
  uint16_t bamd_id_len;
};

static int proc_list_wait(int epfd, llist * restrict list, llist * restrict freelist)
{
  struct epoll_event evs[16];
  int evs_len;
  int x;
  struct proc *proc;
  int r;

  r = 0;
  evs_len = uxepoll_wait(epfd, evs, ARRAY_LEN(evs), SEC_AS_MSEC(3));
  for(x = 0; x < evs_len; x++) {
    proc = evs[x].data.ptr;
    xclose(proc->pfd);
    llist_delete(&proc->lln);
    llist_push(freelist, &proc->lln);
  }
  if(evs_len == 0) {
    /* no one exited within the timeout */
    dprintf(2, "[%5"PRId64"] process(es) unresponsive\n", (int64_t)getpid());
    llist_foreach(list, proc, lln) {
      dprintf(2, "[%5"PRId64"] pid %ld id %.*s\n", (int64_t)getpid(), (int64_t)proc->pid, (int)proc->bamd_id_len, proc->bamd_id);
    }
    r = 1;
  }

  return r;
}

static void proc_list_add(int epfd, llist * restrict list, struct proc * restrict proc)
{
  struct epoll_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN | EPOLLONESHOT;
  ev.data.ptr = proc;
  xepoll_ctl(epfd, EPOLL_CTL_ADD, proc->pfd, &ev);

  llist_push(list, &proc->lln);
}

/* kill any bamd instances for a different basedir where,
 *  their basedir starts with our basedir (we subsume this dir) or
 *  our basedir starts with their basedir (they subsume our dir) */
static int lockfiles_kill_competing(bam_client * restrict client, int epfd, llist * restrict procs_list, llist * restrict procs_freelist)
{
  DIR *dd;
  char id[64];
  uint16_t id_len;
  int64_t pid;
  int pfd;
  char basedir[PATH_MAX];
  size_t basedir_len;
  int rv;

  struct proc *proc;

  /* locate any bamds for conflicting basedirs */
  dd = bamipc_lockfile_scan_start(client->ipcdir_root);
  rv = 0;
  pid = -1;
  while(!llist_empty(procs_list) || pid != 0)
  {
    /* wait on any killed processes */
    if(!llist_empty(procs_list)) {
      if(proc_list_wait(epfd, procs_list, procs_freelist) != 0) {
        rv = 1;
        break;
      }
    }

    while(1)
    {
      if(llist_empty(procs_freelist)) {
        break;
      }
      if((pid = bamipc_lockfile_scan_scan(dd, id, sizeof(id), &id_len, basedir, sizeof(basedir), &basedir_len)) == 0) {
        break;
      }
      if(memcmp(basedir, client->basedir, MIN(basedir_len, client->basedir_len)) != 0) {
        continue; /* disjoint from basedir */
      } else if(basedir_len == client->basedir_len) {
        continue; /* same as basedir */
      } else if(pid == -1) {
        uxunlinkf("%s/%.*s/bamd/lock", client->ipcdir_root, id_len, id);
        continue; /* bad lockfile */
      }

      if((pfd = uxpidfd_open(pid, 0)) == -1) {
        /* stale lockfile */
        uxunlinkf("%s/%.*s/bamd/lock", client->ipcdir_root, id_len, id);
        continue;
      }

      proc = llist_shift(procs_freelist, typeof(*proc), lln);
      proc->pfd = pfd;
      proc->pid = pid;
      proc->bamd_id_len = zcatw(proc->bamd_id, sizeof(proc->bamd_id), id, id_len);
      proc_list_add(epfd, procs_list, proc);
      uxkill(pid, SIGTERM); /* ask nicely */
    }
  }
  bamipc_lockfile_scan_stop(dd);

  llist_foreach(procs_list, proc, lln) {
    xclose(proc->pfd);
  }

  return rv;
}

static int connect(bam_client * restrict client)
{
  int channel_shmid;
  sigset_t sigs;
  siginfo_t info;
  int err;
  struct timespec interval;

  /* connect to bamd */
  if(bam_client_solicit(client) != 0) {
    dprintf(2, "[%5"PRId64"] bamd %"PRId64" not found\n", (int64_t)getpid(), (int64_t)client->bamd_pid);
    return 1;
  }

  /* wait for the acknowledgement signal carrying the channel id */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);

  /* 500 millis */
  interval.tv_sec = 0;
  interval.tv_nsec = 500000000;

  while(!g_sigterm && !g_params.shutdown)
  {
    err = sigutil_timedwait(&sigs, &info, &interval);
    if(err == 0) {                  // signal received
      break;
    } else if(err == EINTR) {       // interrupted by signal
      continue;
    }

    dprintf(2, "[%5"PRId64"] bamd %"PRId64" did not respond\n", (int64_t)getpid(), (int64_t)client->bamd_pid);
    return 1;
  }

  channel_shmid = info.si_value.sival_int;
  bam_client_attach(client, channel_shmid);

  return 0;
}

static void xmain()
{
  sigset_t sigs;
  bam_client client_s, *client = 0;
  bamipc_message * msg = 0;
  uint16_t pulse;
  int iter;
  int r = 0;
  struct timespec interval;
  int x;

  struct proc procs[16], *proc;
  llist procs_list;
  llist procs_freelist;

  llist_init(&procs_list);
  llist_init(&procs_freelist);
  for(x = 0; x < ARRAY_LEN(procs); x++) {
    llist_push(&procs_freelist, &procs[x].lln);
  }

  int epfd;
  int pfd;
  epfd = xepoll_create();

  sigutil_install_handlers();
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);
  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

#if CONFIG_DEVEL
  static char space[PATH_MAX];
  snprintf(space, sizeof(space), "%s", "/home/todd/bam-build/bamd/devel/bamd");
  g_bam_client_bamd_path = space;
  if(g_args.system_config_path.s) {
    g_bam_client_system_config_path = strndup(g_args.system_config_path.s, g_args.system_config_path.len);
  }
  if(g_args.user_config_path.s) {
    g_bam_client_user_config_path = strndup(g_args.user_config_path.s, g_args.user_config_path.len);
  }
  if(g_args.default_filesystem_invalidation.s) {
    g_bam_client_default_filesystem_invalidation = strndup(g_args.default_filesystem_invalidation.s, g_args.default_filesystem_invalidation.len);
  }
  if(g_args.sweeper_period_nsec.s) {
    g_bam_client_sweeper_period_nsec = strndup(g_args.sweeper_period_nsec.s, g_args.sweeper_period_nsec.len);
  }
#endif
  client = &client_s;
  bam_client_init(client, g_params.cwd, g_params.cwd_len, g_params.ipcdir, g_params.ipcdir_len);

  /* open the lockfile */
  bam_client_lockfile_query(client);
  bam_client_lockfile_validate(client);

  /* kill an existing bamd if --kill */
  if(client->bamd_pid != -1 && g_args.kill) {
    if((pfd = uxpidfd_open(client->bamd_pid, 0)) == -1) {
      /* it exited after all */
    } else {
      proc = llist_shift(&procs_freelist, typeof(*proc), lln);
      proc->pfd = pfd;
      proc->pid = client->bamd_pid;
      proc->bamd_id_len = zcatw(proc->bamd_id, sizeof(proc->bamd_id), client->bamd_id, client->bamd_id_len);
      proc_list_add(epfd, &procs_list, proc);
      uxkill(proc->pid, SIGTERM); /* ask nicely */
      if(proc_list_wait(epfd, &procs_list, &procs_freelist) != 0) {
        goto err;
      }
    }

    /* re-load the lockfile */
    ixclose(&client->lockfd);
    client->bamd_pid = -1;
    bam_client_lockfile_query(client);
  }

  if(g_cmd->name && strcmp(g_cmd->name, "kill") == 0) {
    goto end;
  }

  /* stop here if --no-launch */
  if(client->bamd_pid == -1 && g_args.no_launch) {
    dprintf(2, "[%5"PRId64"] bamd not found\n", (int64_t)getpid());
    goto err;
  }

  /* launch a new bamd if needed */
  if(client->bamd_pid == -1) {
    if(client->lockfd == -1) {
      dprintf(2, "[%5"PRId64"] unable to obtain lockfile %s/bamd/lock\n", (int64_t)getpid(), client->ipcdir);
      goto err;
    }

    if(lockfiles_kill_competing(client, epfd, &procs_list, &procs_freelist) != 0) {
      goto err;
    }
    bam_client_launch(client);
  }

  if(connect(client) != 0) {
    goto err;
  }

  if(g_cmd->attached) {
    g_cmd->attached(g_cmd, client);
  }

  /* always-on event subscriptions */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_SYSTEM_STATE_CHANGE
    ;
  client_post(client, msg);

  /* event subscription and send the request */
  g_cmd->connected(g_cmd, client);

  /* 125 millis */
  interval.tv_sec = 0;
  interval.tv_nsec = 125 * 1000 * 1000;
  iter = 0;
  pulse = client->shm->server_pulse;
  while(!g_sigterm && !g_params.shutdown)
  {
    client->shm->client_pulse++;
    if(!(msg = bam_client_acquire(client))) {
      if(client->shm->server_exit) {
        /* normally the daemon will send a response message with nonzero code on error */
        dprintf(2, "bamd %"PRId64" exited\n", client->bamd_pid);
        break;
      }

      atomic_store(&client->shm->server_ring.waiters, 1);
      r = uxfutex(&client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(r == EINTR || r == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((++iter) % 25) == 0) {
        if(pulse == client->shm->server_pulse) {
          /* watchdog timeout - unresponsive daemon */
          dprintf(2, "bamd %"PRId64" unresponsive\n", client->bamd_pid);
          g_params.waserror = true;
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    iter = 0;
    RUNTIME_ASSERT(msg->type);
    client_acquired(client, msg);
    if(msg->type == BAMIPC_MSG_RESPONSE && msg->id == eventsubid)
    {
      RUNTIME_ASSERT(msg->code == 0);
      bam_client_consume(client, msg);
      continue;
    }
    if((msg->type == BAMIPC_MSG_RESULT || msg->type == BAMIPC_MSG_RESPONSE) && msg->code != 0)
    {
      /* suppress errors for message ids in this list */
      for(x = 0; x < message_no_error_list_len; x++) {
        if(message_no_error_list[x] == msg->id) {
          break;
        }
      }
      if(x >= message_no_error_list_len) {
        (void)!write(2, msg->text, msg->size);
        (void)!write(2, "\n", 1);
        g_params.waserror = true;
      }
    }
    else if((msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE_CHANGE && msg->code != 0))
    {
      (void)!write(2, msg->text, msg->size);
      (void)!write(2, "\n", 1);
      g_params.waserror = true;
    }
    g_cmd->process(g_cmd, client, msg);
    bam_client_consume(client, msg);
  }

  if(g_cmd->disconnected) {
    g_cmd->disconnected(g_cmd);
  }

  goto end;
err:
  g_params.waserror = true;
end:
  bam_client_disconnect(client);
  bam_client_destroy(client);

  xclose(epfd);
}

int main(int argc, char ** argv)
{
  tid = syscall(SYS_gettid);
  errnop = &errno;

#ifdef CONFIG_DEVEL
  backtracef_setup();
#endif

  // load libraries
  bam_load();
  narrator_load();
  bacon_load();

  // load modules
  params_setup();
  args_parse(argc, argv);
  build_command_setup();

  xmain();

  // modules
  params_teardown();
  build_command_cleanup();
  args_cleanup();

  // libraries
  bam_unload();
  narrator_unload();
  bacon_unload();

  return !!(g_params.waserror || build_was_failed_slot);
}
