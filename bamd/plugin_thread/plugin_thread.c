/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>

#include "bam/sigutil.h"
#include "bam/ipc.h"
#include "bam/pipe.h"
#include "bam/ring.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xepoll.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xprctl.h"
#include "xlinux/xsignal.h"
#include "xlinux/xwait.h"
#include "xlinux/xfcntl.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "bacon/parser.h"
#include "bacon/make.h"

#include "plugin_thread.h"
#include "plugin.h"
#include "channel.h"
#include "reconcile.h"
#include "request_handler.h"
#include "args.h"
#include "params.h"
#include "system.h"
#include "buildplan.h"
#include "build.h"
#include "request.h"
#include "build.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "request_parser.h"
#include "selector_parser.h"
#include "run_thread.h"
#include "pattern_parser.internal.h"
#include "events/events.h"
#include "goals.h"
#include "exec.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "plugin_process.h"
#include "plugin/probes.h"
#include "adaptor.h"
#include "sysvar.h"
#include "timers.h"

#include "atomics.h"
#include "threads.h"
#include "times.h"

#include "subsys.h"

static llist freelist = LLIST_INITIALIZER(freelist);

rcu_list plugin_thread_list = RCU_LIST_INITIALIZER(plugin_thread_list);
struct futexlock plugin_thread_list_lock;

static void plugin_exited_event(struct plugin * restrict plugin)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(plugin->process->status || plugin->process->err_total) {
      STRING("plugin crashed")
    } else {
      STRING("plugin exited")
    }
    { SET
      { MAPPING
        STRING("plugin")
        { SET
          plugin_describe(plugin, builder_writer);
        }
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void plugin_hung_event(struct plugin * restrict plugin)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("plugin did not respond in time")
    { SET
      { MAPPING
        STRING("plugin")
        { SET
          plugin_describe(plugin, builder_writer);
        }
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void plugin_badrequest_event(struct plugin * restrict plugin, const char *err, size_t err_len)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("plugin issued an invalid request")
    { SET
      { MAPPING
        STRING("error")
        BYTES(err, err_len)
      }
      { MAPPING
        STRING("plugin")
        { SET
          plugin_describe(plugin, builder_writer);
        }
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void __attribute__((noreturn)) plugin_exec(const struct plugin_process * restrict process, struct exec * restrict exec)
{
  int x;
  sigset_t fsigs;

  /* prevent zombies */
  xprctl(PR_SET_PDEATHSIG, 9, 0, 0, 0);

  /* unblock signals */
  sigemptyset(&fsigs);
  xpthread_sigmask(SIG_SETMASK, &fsigs, 0);

  /* std file descriptors */
  xdup2(process->stdin_pipe[0], 0);
  xdup2(process->stdout_pipe[1], 1);
  xdup2(process->stderr_pipe[1], 2);
  xdup2(process->auxout_pipe[1], 1001);

  for(x = 3; x < 1000; x++) {
    close(x);
  }

  execve(exec->file_path, exec->args, exec->envs);

  /* unreachable */
  dprintf(1, "%s\n", strerror(errno));
  RTA;
  exit(1);
}

static void plugin_fork_and_exec(struct plugin_process *process)
{
  struct plugin *plugin;

  plugin = process->plugin;

  process->pid = xfork();
  if(process->pid == 0) {
    plugin_exec(process, plugin->exec);
    /* does not return */
    RTA;
  }

  plugin_exec_probe(process->plugin, plugin->exec);
}

static int parse_request(
    struct plugin * restrict plugin
  , char * restrict text
  , uint16_t len
  , request_parser * restrict parser
  , request * restrict req
)
{
  if((request_parser_parse(parser, text, len, "request", req)) == 0) {
    plugin->plugin_stats.tx_parse_ok++;
    return 0;
  }

  plugin->plugin_stats.tx_parse_err++;
  plugin_badrequest_event(plugin, parser->error, parser->error_len);
  return -EINVAL;
}

/* scan the auxout buffer
 *
 * returns
 *  -1 : parsing error
 *   0 : no request found
 *   1 : request was parsed
 */
static bool scan_auxout(char * restrict aux, size_t sz, uint32_t *cons, uint32_t tail)
{
  uint32_t x;
  uint32_t x0;
  uint32_t x1;

  x = *cons;
  do {
    x++;
    x0 = x - 1;
    x0 %= sz;
    x1 = x;
    x1 %= sz;

    if(aux[x0] == '\n' && aux[x1] == '\n') {
      *cons = x + 1;
      return true;
    }
  } while(x != tail);

  *cons = x;
  return false;
}

static void plugin_tx(plugin_process * restrict process, struct adaptor * restrict adaptor, const struct iovec *iov, int iovcnt)
{
  size_t z;

  /* tee a copy off to the visibility buffer */
  z = adaptor_overwritev(adaptor, iov, iovcnt);

  (void)!writev(process->stdin_pipe[1], iov, iovcnt);
  plugin_tx_probe(process->plugin, iov, iovcnt - 1);

  process->in_total += z;
  process->plugin->plugin_stats.rx_messages++;
  process->plugin->plugin_stats.in_bytes += z;
}

static void plugin_thread_fn(plugin_thread * restrict thread)
{
  sigset_t fsigs;
  sigset_t esigs;
  rcu_thread rcu;
  int r;
  int epfd = -1;
  struct epoll_event ev;
  struct plugin_process *process;
  ssize_t rv;
  uint32_t stream;
  siginfo_t siginfo;
  request_parser *parser;
  command *cmd;
  request_handler *handler;
  struct timespec interval;
  pid_t child;
  timer_t timerid;
  struct itimerspec its;
  struct iovec iovs[2];
  bamipc_message *e;
  uint32_t delta;
  adaptor in_adaptor;
  adaptor out_adaptor;
  adaptor err_adaptor;
  adaptor auxout_adaptor;
  uint32_t auxout_cons;
  struct plugin *plugin;

  rcu_enter(&rcu);

  // signals handled on this thread
  sigfillset(&fsigs);
  sigdelset(&fsigs, SIGUSR1); // in-process scheduling
  sigdelset(&fsigs, SIGUSR2); // timers_once
  sigdelset(&fsigs, SIGCHLD);

  sigemptyset(&esigs);
  sigaddset(&esigs, SIGUSR1);
  sigaddset(&esigs, SIGUSR2);
  sigaddset(&esigs, SIGCHLD);

  plugin = thread->plugin;
  process = plugin->process;

  xpipe(process->stdin_pipe);
  xpipe(process->stdout_pipe);
  xpipe(process->stderr_pipe);
  xpipe(process->auxout_pipe);

  in_adaptor.buf = process->in;
  in_adaptor.sz = process->in_size;
  in_adaptor.head = &process->in_head;
  in_adaptor.tail = &process->in_tail;

  out_adaptor.buf = process->out;
  out_adaptor.sz = process->out_size;
  out_adaptor.head = &process->out_head;
  out_adaptor.tail = &process->out_tail;

  err_adaptor.buf = process->err;
  err_adaptor.sz = process->err_size;
  err_adaptor.head = &process->err_head;
  err_adaptor.tail = &process->err_tail;

  auxout_adaptor.buf = process->auxout;
  auxout_adaptor.sz = process->auxout_size;
  auxout_adaptor.head = &process->auxout_head;
  auxout_adaptor.tail = &process->auxout_tail;
  auxout_cons = 0;

  handler = request_handler_alloc();

  /* setup epoll */
  epfd = xepoll_create();

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 1  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->stdout_pipe[0], &ev);

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 2  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->stderr_pipe[0], &ev);

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 1001  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->auxout_pipe[0], &ev);

  request_parser_create(&parser);
  request_init(&process->last_req);
  cmd = 0;

  /* launch plugin process */
  plugin_fork_and_exec(process);
  child = process->pid;

  process->running = true;
  clock_gettime(CLOCK_MONOTONIC, &process->started_at);
  plugin->plugin_stats.fork++;

  /* plugin child process has one second to respond */
  timers_once_start(&timerid, tid, SEC_AS_NSEC(1), 0, 0);

  interval.tv_sec = 1;
  interval.tv_nsec = 0;

  plugin->thread_state = PLUGIN_THREAD_UNREADY;

  while(!g_params.shutdown && child != -1)
  {
    rcu_quiesce(&rcu);

    if(!process->ready) {
      timer_gettime(timerid, &its);
      if(its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) {
        /* the timer has expired */
        thread->shutdown = true;
        plugin_hung_event(plugin);
      }
    }

    /* kill child if requested */
    if(thread->shutdown) {
      if(!process->killed_term) {
        process->killed_term = true;
        uxkill(child, SIGTERM);
      } else if(!process->killed_waited) {
        process->killed_waited = true;
        sigutil_timedwait(&esigs, &siginfo, &interval);
      } else if(!process->killed_kill) {
        process->killed_term = true;
        uxkill(child, SIGKILL);
      }
    }

    /* process the next command if any */
    if(cmd) {
      if(cmd->type == COMMAND_GOALS) {
        goals_set(
            child
          , cmd->goals.token
          , cmd->goals.label
          , cmd->goals.label_len
          , cmd->goals.reconcile
          , cmd->goals.build
          , cmd->goals.target_direct
          , cmd->goals.target_transitive
        );

        /* goals takes ownership of the selectors */
        cmd->goals.target_direct = 0;
        cmd->goals.target_transitive = 0;

        /* invalidates //goals which triggers autorun */
        handler->invalidated_any = true;
      } else if(cmd->type == COMMAND_EVENTSUB) {
        delta = cmd->mask & ~thread->event_mask;
        thread->event_mask = cmd->mask;
        events_trigger(handler, delta);
      } else {
        request_handler_process_command(handler, cmd);
      }

      /* unblock the run thread, which should be waiting for this */
      if(!process->ready && cmd->type == COMMAND_PLUGIN_READY) {
        timers_once_put(&timerid, 0);
        process->ready = true;
        vertex_mark_fresh(&plugin->vertex);
        vertex_mark_fresh(&plugin->self_node->vertex);
        plugin->plugin_stats.ready++;
      }

      if(cmd->type == COMMAND_PLUGIN_READY) {
        if(plugin->thread_state != PLUGIN_THREAD_READY_RECEIVED) {
          plugin_ready_probe(plugin);
        }
        plugin->thread_state = PLUGIN_THREAD_READY_RECEIVED;
      } else {
        plugin->plugin_stats.commands++;
      }

      if((cmd = llist_next(&process->last_req.commands, cmd, lln))) {
        /* next command, on the next iteration */
      } else {
        if(handler->invalidated_any && run_thread_autorun) {
          run_thread_launch(0, 0, COMMAND_AUTORUN);
        }
      }
      continue;
    } else if(auxout_cons != process->auxout_tail) {
      if(!scan_auxout(process->auxout, process->auxout_size, &auxout_cons, process->auxout_tail)) {
        /* two-newlines not found in the buffer */
        continue;
      }

      process->last_buf_len = adaptor_pop(&auxout_adaptor, auxout_cons, process->last_buf, sizeof(process->last_buf));
      process->last_buf[process->last_buf_len - 1] = 0; // null out the newlines
      process->last_buf[process->last_buf_len - 2] = 0;

      request_destroy(&process->last_req);
      if((r = parse_request(process->plugin, process->last_buf, process->last_buf_len, parser, &process->last_req)) != 0) {
        /* parsing error - discard any further data */
        thread->shutdown = true;
        continue;
      }

      plugin->plugin_stats.tx_messages++;
      plugin_rx_probe(process->plugin, process->last_buf, process->last_buf_len);
      cmd = llist_first(&process->last_req.commands, typeof(*cmd), lln);
      continue;
    }

    /* ask for readiness */
    if(plugin->thread_state == PLUGIN_THREAD_UNREADY) {
      plugin->thread_state = PLUGIN_THREAD_READY_REQUESTED;

      iovs[0].iov_base = "plugin-ready-request";
      iovs[0].iov_len = strlen(iovs[0].iov_base);
      iovs[1].iov_base = "\n\n";
      iovs[1].iov_len = 2;
      plugin_tx(process, &in_adaptor, iovs, 2);
      plugin_unready_probe(plugin);
      continue;
    }

    /* propagate events */
    if(bamipc_peek(&thread->pipe->ring) == BAMIPC_OK) {
      if(bamipc_rx(thread->pipe, &interval, &e) == BAMIPC_OK) {
        iovs[0].iov_base = e->text;
        iovs[0].iov_len = e->len;
        iovs[1].iov_base = "\n\n";
        iovs[1].iov_len = 2;
        plugin_tx(process, &in_adaptor, iovs, 2);
        plugin->plugin_stats.events++;
        bamipc_release(thread->pipe, e);
        continue;
      }
    }

    /* receive a signal, or output from the process */
    if(plugin->thread_state == PLUGIN_THREAD_READY_RECEIVED) {
      plugin->thread_state = PLUGIN_THREAD_READY_REPORTED;
      sigutil_uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1); // plugin_system_reconcile on the run thread
    }
    r = uxepoll_pwait(epfd, &ev, 1, MSEC_AS_MSEC(125), &fsigs);
    rcu_quiesce(&rcu);
    if(r > 0)
    {
      stream = ev.data.u32 & 0xffff;
      switch(stream) {
        case 1:
          rv = adaptor_push_read(&out_adaptor, process->stdout_pipe[0], UINT32_MAX);
          process->out_total += rv;
          plugin->plugin_stats.out_bytes += rv;
          break;
        case 2:
          rv = adaptor_push_read(&err_adaptor, process->stderr_pipe[0], UINT32_MAX);
          process->err_total += rv;
          plugin->plugin_stats.err_bytes += rv;
          break;
        case 1001:
          rv = adaptor_push_read(&auxout_adaptor, process->auxout_pipe[0], UINT32_MAX);
          process->auxout_total += rv;
          plugin->plugin_stats.aux_bytes += rv;
          break;
      }
    }

    /* NOTE : this thread is competing with other threads calling wait (run_thread)
     * WEXITED - check for exited child processes
     * WNOHANG - dont block */
    rcu_quiesce(&rcu);
    uxwaitid(P_PID, child, &siginfo, WEXITED | WNOHANG);
    if(siginfo.si_pid == 0) {
      continue;
    }
    RUNTIME_ASSERT(siginfo.si_pid == child);

    child = -1;
    process->running = false;
    process->status = siginfo.si_status;
    clock_gettime(CLOCK_MONOTONIC, &process->stopped_at);
    plugin->plugin_stats.reap++;
    if(process->err_total || siginfo.si_status) {
      plugin->plugin_stats.crashed++;
    }
    plugin_reap_probe(plugin);

    if(!thread->shutdown) {
      plugin_exited_event(plugin);
    }
  }

  thread->event_mask = 0;
  rcu_quiesce(&rcu);
  vertex_mark_stale(&plugin->vertex);

  rcu_leave(&rcu);
  close(epfd);
  request_parser_free(parser);
  request_destroy(&process->last_req);
  request_handler_release(handler);

  plugin->thread_state = PLUGIN_THREAD_EXITED;
}

static void *plugin_thread_jump(void * arg)
{
  struct plugin *plugin;
  plugin_thread *thread;
  plugin_process *process;
  bamipc_pipe *pipe;

  plugin = arg;
  tid = gettid();
  errnop = &errno;

  xmalloc(&pipe, sizeof(*pipe));
  bamipc_pipe_init(pipe);

  /* the thread is lazily reclaimed, so that it can be
   * inspected if necessary before the plugin is restarted */
  if(plugin->thread) {
    plugin_thread_release(plugin->thread);
  }
  thread = plugin_thread_alloc();
  thread->tid = tid;
  thread->plugin = plugin;
  thread->pipe = pipe;
  plugin->thread = thread;

  /* the process is lazily reclaimed, so that it can be
   * inspected if necessary before the plugin is restarted */
  if(plugin->process) {
    plugin_process_free(plugin->process);
  }

  process = plugin_process_alloc();
  process->plugin = plugin;
  plugin->process = process;

  futexlock_acquire(&plugin_thread_list_lock);
  rcu_list_push(&plugin_thread_list, &thread->stk);
  futexlock_release(&plugin_thread_list_lock);

  /* plugin_system is waiting for this thread to be added to the list */
  sigutil_uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);

  plugin_thread_fn(thread);

  /* plugin_system is waiting until this thread is removed from the list */
  futexlock_acquire(&plugin_thread_list_lock);
  rcu_list_delete(&thread->stk);
  futexlock_release(&plugin_thread_list_lock);

  rcu_synchronize();

  free(pipe);

  atomic_fetch_dec(&g_params.thread_count);
  sigutil_uxtgkill(g_params.pid, g_params.thread_monitor, SIGUSR1);
  sigutil_uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);

  return 0;
}

//
// public
//

void plugin_thread_cleanup(void)
{
  plugin_thread *thread;

  while((thread = llist_shift(&freelist, typeof(*thread), lln))) {
    free(thread);
  }
}

plugin_thread *plugin_thread_alloc()
{
  plugin_thread *thread;

  if((thread = llist_shift(&freelist, typeof(*thread), lln)) == 0) {
    xmalloc(&thread, sizeof(*thread));
  } else {
    memset(thread, 0, sizeof(*thread));
  }

  rcu_list_init(&thread->stk);
  llist_init(&thread->lln);

  return thread;
}

void plugin_thread_release(plugin_thread *thread)
{
  llist_delete(&thread->lln);
  llist_append(&freelist, &thread->lln);
}

void plugin_thread_launch(struct plugin * restrict plugin)
{
  pthread_t pthread_id;
  pthread_attr_t attr = {};

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  plugin->thread_state = PLUGIN_THREAD_STARTING;

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, plugin_thread_jump, plugin);

  pthread_attr_destroy(&attr);
}

void plugin_thread_kill(struct plugin_thread * restrict thread)
{
  thread->shutdown = true;
  sigutil_uxtgkill(g_params.pid, thread->tid, SIGUSR1);
}

void plugin_thread_unready(struct plugin_thread * restrict thread)
{
  struct plugin *plugin;

  plugin = thread->plugin;
  plugin->thread_state = PLUGIN_THREAD_UNREADY;
  sigutil_uxtgkill(g_params.pid, thread->tid, SIGUSR1);
}
