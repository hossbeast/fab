/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "bam/sigutil.h"
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

#include "plugin_thread.h"
#include "plugin.h"
#include "channel.h"
#include "reconcile.h"
#include "handler.h"
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
#include "events.h"
#include "goals.h"
#include "exec.h"
#include "exec_builder.h"
#include "plugin_process.h"
#include "plugin/probes.h"

#include "atomics.h"
#include "threads.h"
#include "times.h"

rcu_list plugin_thread_list = RCU_LIST_INITIALIZER(plugin_thread_list);
struct futexlock plugin_thread_list_lock;

static void plugin_exited_event(struct plugin * restrict plugin)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  event_bacon *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
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
  msg->size = fixed.l;
  events_publish(&ectx);
}

static void plugin_hung_event(struct plugin * restrict plugin)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  event_bacon *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
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
  msg->size = fixed.l;
  events_publish(&ectx);
}

static void plugin_badrequest_event(struct plugin * restrict plugin, const char *err, size_t err_len)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  event_bacon *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
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
  msg->size = fixed.l;
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

  execve(exec->path, exec->args, exec->envs);

  /* unreachable */
  dprintf(1, "%s\n", strerror(errno));
  RTA;
  exit(1);
}

static void plugin_system_envs(struct exec_builder * restrict eb)
{
  exec_builder_args ba;

  memset(&ba, 0, sizeof(ba));
  ba.val.s = g_params.pid_str;
  ba.name = "bm_pid";
  ba.name_len = strlen("bm_pid");
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ENVS;
  ba.position = -1;
  exec_builder_add(eb, &ba);

  memset(&ba, 0, sizeof(ba));
  ba.val.s = g_args.hash_str;
  ba.name = "bm_hash";
  ba.name_len = strlen("bm_hash");
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ENVS;
  ba.position = -1;
  exec_builder_add(eb, &ba);

  memset(&ba, 0, sizeof(ba));
  ba.val.s = g_base_dir;
  ba.name = "bm_basedir";
  ba.name_len = strlen("bm_basedir");
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ENVS;
  ba.position = -1;
  exec_builder_add(eb, &ba);

  memset(&ba, 0, sizeof(ba));
  ba.val.s = g_params.ipcdir;
  ba.name = "bm_ipcdir";
  ba.name_len = strlen("bm_ipcdir");
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ENVS;
  ba.position = -1;
  exec_builder_add(eb, &ba);

  memset(&ba, 0, sizeof(ba));
  ba.val.s = g_params.homedir;
  ba.name = "bm_homedir";
  ba.name_len = strlen("bm_homedir");
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ENVS;
  ba.position = -1;
  exec_builder_add(eb, &ba);
}

static void plugin_fork_and_exec(struct plugin_process *process)
{
  struct exec *exec = 0;
  bacon_value *v;
  exec_builder_args ba;

  exec_builder_reset(&process->eb);

  /* path */
  memset(&ba, 0, sizeof(ba));
  ba.val.s = process->plugin->self_node_abspath;
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_FILE;
  exec_builder_add(&process->eb, &ba);

  /* first argument is also the path */
  memset(&ba, 0, sizeof(ba));
  ba.val.s = process->plugin->self_node_abspath;
  ba.render_val = BUILDER_STRING;
  ba.mode = BUILDER_APPEND;
  ba.item = BUILDER_ARGS;
  ba.position = -1;
  exec_builder_add(&process->eb, &ba);

  /* args */
  if(process->plugin->args) {
    llist_foreach(process->plugin->args, v, lln) {
      memset(&ba, 0, sizeof(ba));
      ba.val.v = v;
      ba.render_val = BUILDER_VALUE;
      ba.mode = BUILDER_APPEND;
      ba.item = BUILDER_ARGS;
      ba.position = -1;
      exec_builder_add(&process->eb, &ba);
    }
  }

  /* envs - corresponding to params_describe and to bam metadata */
  plugin_system_envs(&process->eb);

  exec_builder_build(&process->eb, &exec);

  process->pid = xfork();
  if(process->pid == 0) {
    plugin_exec(process, exec);
    /* does not return */
  }

  plugin_exec_probe(process->plugin, exec);
}

static int parse_request(
    struct plugin * restrict plugin
  , char * restrict text
  , uint16_t len
  , request_parser * restrict parser
  , request * restrict req
)
{
  int exit;
  char trace[512];
  size_t size;

  if((exit = request_parser_parse(parser, text, len, "request", req)) == 0) {
    return 0;
  }

  size = 0;
  if(parser->selector_parser->pattern_parser->yyu && parser->selector_parser->pattern_parser->yyu->error_len) {
    size += zcatw(trace, sizeof(trace), parser->selector_parser->pattern_parser->yyu->error_str, parser->selector_parser->pattern_parser->yyu->error_len);
  } else if(parser->selector_parser->yyu.error_len) {
    size += zcatw(trace, sizeof(trace), parser->selector_parser->yyu.error_str, parser->selector_parser->yyu.error_len);
  } else {
    size += zcatw(trace, sizeof(trace), parser->yyu.error_str, parser->yyu.error_len);
  }

  plugin_badrequest_event(plugin, trace, size);
  return -EINVAL;
}

/* scan the aux buffer
 *
 * returns
 *  -1 : parsing error
 *   0 : no request found
 *   1 : request was parsed
 */
static int scan_aux(plugin_process * restrict process, request_parser * restrict parser, request * restrict req)
{
  int x;
  int r;

  r = 0;
  for(x = 0; x < process->aux_len; x++)
  {
    if(x == 0 || process->aux[x - 1] != '\n' || process->aux[x] != '\n') {
      continue;
    }

    process->aux[x - 1] = 0;
    process->aux[x - 0] = 0;

    request_destroy(req);
    /* negative on error */
    r = parse_request(process->plugin, process->aux, x + 1, parser, req);
    if (r == 0) {
      r = 1;
    }
    memmove(
        &process->aux[0]
      , &process->aux[x + 1]
      , process->aux_len - (x + 1)
    );
    process->aux_len -= (x + 1);
    break;
  }

  return r;
}

static void timer_put(timer_t * restrict timerid, uint64_t nsec)
{
  struct itimerspec its;
  int r;

  /* fires just once */
  memset(&its, 0, sizeof(its));
  timespec_add(&its.it_value, nsec);
  r = timer_settime(*timerid, 0, &its, 0);
  RUNTIME_ASSERT(r == 0);
}

static void timer_once_start(timer_t * restrict timerid, int32_t threadid, uint64_t nsec)
{
  struct sigevent evt;
  int r;

  /* create periodic timer to send signals to this thread */
  memset(&evt, 0, sizeof(evt));
  evt.sigev_notify = SIGEV_THREAD_ID;
  evt.sigev_signo = SIGUSR1;
  evt._sigev_un._tid = threadid;
  r = timer_create(CLOCK_MONOTONIC, &evt, timerid);
  RUNTIME_ASSERT(r == 0);

  /* fires just once */
  timer_put(timerid, nsec);
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
  ssize_t bytes = 0;
  ssize_t rv;
  uint32_t stream;
  int fd = 0;
  char *buf = 0;
  size_t sz = 0;
  siginfo_t siginfo;
  request_parser *parser;
  request req;
  command *cmd;
  handler_context *handler;
  event_bacon *pb;
  struct timespec interval;
  llist events_txq;
  pid_t child;
  timer_t timerid;
  struct itimerspec its;
  uint32_t aux_len = 0;

  rcu_enter(&rcu);

  // signals handled on this thread
  sigfillset(&fsigs);
  sigdelset(&fsigs, SIGUSR1);
  sigdelset(&fsigs, SIGCHLD);

  sigemptyset(&esigs);
  sigaddset(&esigs, SIGUSR1);
  sigaddset(&esigs, SIGCHLD);

  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(126);

  /* setup process state */
  if(thread->plugin->process) {
    process = thread->plugin->process;
    plugin_process_reset(process);
  } else {
    process = plugin_process_alloc();
    thread->plugin->process = process;
    process->plugin = thread->plugin;

    xpipe(process->stdin_pipe);
    xpipe(process->stdout_pipe);
    xpipe(process->stderr_pipe);
    xpipe(process->auxout_pipe);
  }

  handler = thread->handler;

  /* setup epoll */
  epfd = xepoll_create();

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 1  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->stdout_pipe[0], &ev);

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 2  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->stderr_pipe[0], &ev);

  ev = (typeof(ev)) { .events = EPOLLIN, .data.u32 = 1001  };
  xepoll_ctl(epfd, EPOLL_CTL_ADD, process->auxout_pipe[0], &ev);

  llist_init(&events_txq);
  request_parser_create(&parser);
  request_init(&req);
  cmd = 0;

  /* launch plugin process */
  plugin_fork_and_exec(process);
  child = process->pid;

  process->running = true;
  clock_gettime(CLOCK_MONOTONIC, &process->started_at);
  thread->plugin->plugin_stats.fork++;

  /* plugin child process has 500ms to respond */
  timer_once_start(&timerid, tid, MSEC_AS_NSEC(500));

  while(!g_params.shutdown && child != -1)
  {
    rcu_quiesce(&rcu_self);

    if(!process->ready) {
      timer_gettime(timerid, &its);
      if(its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) {
        /* the timer has expired */
        thread->shutdown = true;
        plugin_hung_event(thread->plugin);
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
      if(cmd->type == COMMAND_GOALS)
      {
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
      } else {
        handler_process_command(handler, cmd);
      }

      /* unblock the run thread, which should be waiting for this */
      if(!process->ready && cmd->type == COMMAND_PLUGIN_READY) {
        timer_put(&timerid, 0);
        process->ready = true;
        vertex_mark_fresh(&thread->plugin->vertex);
        vertex_mark_fresh(&thread->plugin->self_node->vertex);
        thread->plugin->plugin_stats.ready++;
      }

      if(cmd->type == COMMAND_PLUGIN_READY) {
        thread->ready_reported = true;
        plugin_ready_probe(thread->plugin);
      }

      if((cmd = llist_next(&req.commands, cmd, lln))) {
        /* next command, on the next iteration */
      } else if(handler->invalidated_any && run_thread_autorun) {
        /* request completed */
        run_thread_launch(0, COMMAND_AUTORUN);
      }
      continue;
    } else if(process->aux_len != aux_len) {
      if((r = scan_aux(process, parser, &req)) < 0) {
        /* parsing error */
        aux_len = process->aux_len;
        thread->shutdown = true;
        continue;
      } else if(r > 0) {
        aux_len = 0;
        thread->plugin->plugin_stats.rx_messages++;
        cmd = llist_first(&req.commands, typeof(*cmd), lln);
        continue;
      } else {
        /* no request */
        aux_len = process->aux_len;
      }
    }

    /* ask for readiness */
    if(!thread->ready_requested) {
      thread->ready_requested = true;
      (void)!write(process->stdin_pipe[1], MMS("plugin-ready-request\n\n"));
      plugin_unready_probe(thread->plugin);
      continue;
    }

    /* propagate events */
    while((pb = llist_shift(&events_txq, typeof(*pb), lln))) {
      (void)!write(process->stdin_pipe[1], pb->text, pb->size);
      (void)!write(process->stdin_pipe[1], "\n\n", 2);

      thread->plugin->plugin_stats.tx_messages++;
      thread->plugin->plugin_stats.in_bytes += pb->size + 2;
      event_bacon_free(pb);
    }

    /* poll the event queue */
    if(!llist_empty(&handler->events_txq)) {
      futexlock_acquire(&handler->events_txq_lock);
      llist_splice_tail(&events_txq, &handler->events_txq);
      futexlock_release(&handler->events_txq_lock);
    }
    if(!llist_empty(&events_txq)) {
      continue;
    }

    /* receive events or a signal */
    /* receive a signal, or output from the process */
    if(thread->ready_reported && !thread->ready_quiescent) {
      thread->ready_quiescent = true;
      sigutil_uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);
      plugin_quiesce_probe(thread->plugin);
    }
    r = uxepoll_pwait(epfd, &ev, 1, SEC_AS_MSEC(1), &fsigs);
    rcu_quiesce(&rcu_self);
    if(r > 0)
    {
      stream = ev.data.u32 & 0xffff;
      switch(stream) {
        case 1:
          fd = process->stdout_pipe[0];
          buf = process->out;
          sz = sizeof(process->out);
          bytes = process->out_len;
          break;
        case 2:
          fd = process->stderr_pipe[0];
          buf = process->err;
          sz = sizeof(process->err);
          bytes = process->err_len;
          break;
        case 1001:
          fd = process->auxout_pipe[0];
          buf = process->aux;
          sz = sizeof(process->aux);
          bytes = process->aux_len;
          break;
      }
      if(bytes == sz) {
        /* buffer is full ; discard overflow */
        rv = xsplice(fd, 0, build_devnull_fd, 0, 0xffff, SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
      } else {
        rv = xread(fd, buf + bytes, sz - bytes);
        switch(stream) {
          case 1:
            process->out_len += rv;
            /* stdout is just retained in the buffer */
            break;
          case 2:
            process->err_len += rv;
            /* stderr is just retained in the buffer */
            break;
          case 1001:
            process->aux_len += rv;
            break;
        }
      }
      switch(stream) {
        case 1:
          process->out_total += rv;
          thread->plugin->plugin_stats.out_bytes += rv;
          break;
        case 2:
          process->err_total += rv;
          thread->plugin->plugin_stats.err_bytes += rv;
          break;
        case 1001:
          process->aux_total += rv;
          thread->plugin->plugin_stats.aux_bytes += rv;
          break;
      }
    }

    /* reap any children */
    /* NOTE : this thread is competing with other threads calling wait (run thread) */
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
    thread->plugin->plugin_stats.reap++;
    if(process->err_total || siginfo.si_status) {
      thread->plugin->plugin_stats.crashed++;
    }
    plugin_reap_probe(thread->plugin);

    if(!thread->shutdown) {
      plugin_exited_event(thread->plugin);
    }
  }


  rcu_quiesce(&rcu);
  vertex_mark_stale(&thread->plugin->vertex);

  /* remove event subscriptions */
  handler->event_mask = 0;

  futexlock_acquire(&handler->events_txq_lock);
  llist_splice_tail(&events_txq, &handler->events_txq);
  futexlock_release(&handler->events_txq_lock);

  /* discard any unprocessed events */
  event_bacon_free_list(&events_txq);

  rcu_leave(&rcu);
  close(epfd);
  request_parser_free(parser);
  request_destroy(&req);
}

static void *plugin_thread_jump(void * arg)
{
  struct plugin *plugin;
  plugin_thread thread;
  handler_context *handler;
  rcu_thread rcu;

  plugin = arg;
  tid = gettid();
  errnop = &errno;

  handler_alloc(&handler);
  handler->events_tid = tid;

  memset(&thread, 0, sizeof(thread));
  thread.tid = tid;
  thread.plugin = plugin;
  thread.handler = handler;
  plugin->thread = &thread;
  rcu_list_init(&thread.stk);

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &handler->stk);
  futexlock_release(&handlers_lock);

  futexlock_acquire(&plugin_thread_list_lock);
  rcu_list_push(&plugin_thread_list, &thread.stk);
  futexlock_release(&plugin_thread_list_lock);

  plugin_thread_fn(&thread);

  futexlock_acquire(&plugin_thread_list_lock);
  rcu_list_delete(&thread.stk);
  futexlock_release(&plugin_thread_list_lock);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&handler->stk);
  futexlock_release(&handlers_lock);

  rcu_synchronize(&rcu);

  plugin->thread = 0;

  /* discard any remaining events */
  event_bacon_free_list(&handler->events_txq);

  handler_release(handler);

  atomic_fetch_dec(&g_params.thread_count);
  sigutil_uxtgkill(g_params.pid, g_params.thread_monitor, SIGUSR1);
  sigutil_uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);

  return 0;
}

//
// public
//

void plugin_thread_launch(struct plugin * restrict plugin)
{
  pthread_t pthread_id;
  pthread_attr_t attr = {};

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

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
  thread->ready_requested = false;
  thread->ready_reported = false;
  thread->ready_quiescent = false;
  sigutil_uxtgkill(g_params.pid, thread->tid, SIGUSR1);
}
