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

#include <sys/syscall.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"
#include "valyria/set.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/stream.h"
#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "moria/graph.h"

#include "logger/arguments.h"

#include "server_thread.internal.h"
#include "config.h"
#include "errtab/FABD.errtab.h"
#include "handler.h"
#include "logging.h"
#include "module.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "request_parser.h"
#include "request.internal.h"
#include "config.internal.h"
#include "usage.h"
#include "stats.h"
#include "walker.internal.h"
#include "buildplan.h"
#include "goals.h"
#include "build_thread.internal.h"
#include "rule.h"
#include "match.h"
#include "path.h"
#include "pattern.h"
#include "node_operations.h"
#include "formula.h"

#include "common/attrs.h"
#include "macros.h"
#include "common/atomic.h"

bool g_server_autorun;

#if DEVEL
bool g_server_no_initial_client;
const char *g_server_initial_request;
#endif

static xapi load_client_pid(pid_t * client_pid)
{
  enter;

  int fd = -1;

  // load fab/pid
  fatal(xopenf, &fd, O_RDONLY, "%s/client/pid", g_params.ipcdir);
  fatal(axread, fd, client_pid, sizeof(*client_pid));
  if(*client_pid <= 0)
  {
    xapi_info_pushs("expected client pid", "> 0");
    xapi_info_pushf("actual client pid", "%ld", (long)*client_pid);
    fail(FABD_BADIPC);
  }

finally:
  fatal(ixclose, &fd);
coda;
}

static xapi __attribute__((unused)) redirect()
{
  enter;

  int fd = -1;
  char space[512];

  // redirect stdout/stderr to the client
  snprintf(space, sizeof(space), "%s/client/out", g_params.ipcdir);
  fatal(xopens, &fd, O_RDWR, space);
  fatal(xdup2, fd, 1);
  fatal(ixclose, &fd);

  snprintf(space, sizeof(space), "%s/client/err", g_params.ipcdir);
  fatal(xopens, &fd, O_RDWR, space);
  fatal(xdup2, fd, 2);

finally:
  fatal(ixclose, &fd);
coda;
}

static xapi receive_request(void ** request_shm)
{
  enter;

  int shmid = -1;
  int fd = -1;

  // get the request shm
  fatal(xopenf, &fd, O_RDONLY, "%s/client/request_shmid", g_params.ipcdir);
  fatal(axread, fd, &shmid, sizeof(shmid));
  fatal(xshmat, shmid, 0, 0, request_shm);

finally:
  fatal(xclose, fd);
coda;
}

static xapi __attribute__((nonnull)) prepare_response_shm(
    void ** restrict response_shm
  , int * restrict response_shm_id
  , size_t * restrict response_shm_size
)
{
  enter;

  int fd = -1;
  int shmid = -1;
  size_t RESPONSE_SIZE = 0xfff;

  // create fixed-size shm for the response buffer
  fatal(xshmget, ftok(g_params.ipcdir, FABIPC_TOKRES), RESPONSE_SIZE, IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &shmid);
  fatal(xshmat, shmid, 0, 0, response_shm);
  fatal(xshmctl, shmid, IPC_RMID, 0);

  // save the shmid for the client
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/fabd/response_shmid", g_params.ipcdir);
  fatal(axwrite, fd, &shmid, sizeof(shmid));

  *response_shm_id = shmid;
  *response_shm_size = RESPONSE_SIZE;

  shmid = -1;

finally:
  if(shmid != -1)
    fatal(xshmctl, shmid, IPC_RMID, 0);
  fatal(xclose, fd);
coda;
}

static xapi __attribute__((nonnull(2, 3))) send_response(
    pid_t client_pid
  , const sigset_t * restrict sigs
  , void * restrict response_shm
  , size_t response_shm_size
  , int response_shm_id
)
{
  enter;

  siginfo_t siginfo;

  struct {
    uint32_t len;
    char text[];
  } *msg = response_shm;
  logf(L_PROTOCOL, "response(%"PRIu32")\n%s", msg->len, msg->text);

  // awaken client and await response
  if(client_pid)
  {
    fatal(sigutil_exchange, FABIPC_SIGACK, client_pid, sigs, &siginfo);
    fatal(sigutil_assert, FABIPC_SIGACK, client_pid, &siginfo);
  }

  finally : coda;
}

/**
 * run invalidated rules to quiesence
 */
static xapi rules_full_refresh(rule_run_context * restrict rule_ctx)
{
  enter;

  rule_module_association *rma;

  /* re-run invalidated rules */
  fatal(rule_run_context_begin, rule_ctx);
  fatal(graph_full_refresh, rule_ctx);

  /* warn about rules that have no effect */
  if(log_would(L_WARN))
  {
    rbtree_foreach(&rule_ctx->nohits, rma, nohits_rbn) {
      fatal(log_start, L_WARN, &N);
      xsays("0 matches : ");
      fatal(rule_say, rma->rule, N);
      xsays(" @ module ");
      fatal(node_absolute_path_say, rma->mod->dir_node, N);
      fatal(log_finish);
    }
  }

finally:
  rule_run_context_end(rule_ctx);
coda;
}

static xapi server_thread()
{
  enter;

  sigset_t sigs;
  pid_t client_pid = 0;
  siginfo_t siginfo;
  handler_context * handler_ctx = 0;
  request_parser * request_parser = 0;
  int iteration;
  bool building;
  rule_run_context rule_ctx = { 0 };

  char * request_buf = 0;
  size_t request_buf_sz;
  void * request_shm = 0;
  request * request = 0;

  void * response_shm = 0;
  int response_shm_id = -1;
  size_t response_shm_size;
  uint32_t response_len;
  char response_narrator_space[NARRATOR_STATIC_SIZE];
  narrator * response_narrator = 0;
  value_writer response_writer;
  graph_invalidation_context invalidation = { 0 };
  uint32_t invalidation_counter;

#if DEVEL
  char request_data[512 << 2];
  size_t rl;
#endif

  g_params.thread_server = gettid();

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, FABIPC_SIGSYN);
  sigaddset(&sigs, FABIPC_SIGACK);
  sigaddset(&sigs, FABIPC_SIGSCH);
  sigaddset(&sigs, FABIPC_SIGINTR);

  fatal(handler_context_create, &handler_ctx);
  fatal(request_parser_create, &request_parser);
  fatal(rule_run_context_xinit, &rule_ctx);
  rule_ctx.modules = &g_modules;

  value_writer_init(&response_writer);

  // extern_reconfigure loads areas of the filesystem referenced via extern
  fatal(config_begin_staging);
  fatal(config_reconfigure);
  if(!config_reconfigure_result)
  {
    fprintf(stderr, "config was not applied\n");
    exit(1);
  }

  // load the filesystem rooted at the project dir
  int walk_id = walker_descend_begin();
  fatal(graph_invalidation_begin, &invalidation);
  fatal(walker_descend, 0, g_project_root, 0, g_params.proj_dir, walk_id, &invalidation);
  fatal(walker_ascend, g_project_root, walk_id, &invalidation);
  graph_invalidation_end(&invalidation);

  // load the module in this directory (and nested modules, recursively)
  fatal(graph_invalidation_begin, &invalidation);
  fatal(module_load_project, g_project_root, g_params.proj_dir, &invalidation);
  graph_invalidation_end(&invalidation);

  if(g_project_root->mod == 0) {
    fprintf(stderr, "module.bam not found\n");
    exit(1);
  }

  if(g_project_root->mod->self_node->not_parsed) {
    fprintf(stderr, "module.bam was not parsed\n");
    exit(1);
  }

  fatal(rules_full_refresh, &rule_ctx);
  fatal(formula_full_refresh);

  // signal to the client readiness to receive requests
#if DEVEL
  if(!g_server_no_initial_client)
  {
#endif
    fatal(load_client_pid, &client_pid);
    fatal(sigutil_kill, client_pid, FABIPC_SIGACK);
#if DEVEL
  }
#endif

#if DEVEL
  if(g_server_no_initial_client)
  {
    rl = strlen(g_server_initial_request);
    memcpy(request_data, g_server_initial_request, rl);
    request_data[rl + 0] = 0;
    request_data[rl + 1] = 0;

    request_buf = request_data;
    request_buf_sz = rl + 2;

    goto process_request;
  }
#endif

  for(iteration = 0; !g_params.shutdown; iteration++)
  {
    fatal(sigutil_wait, &sigs, &siginfo);
    if(siginfo.si_signo == FABIPC_SIGSCH)
      continue;

    /* kicked by sweeper thread */
    if(siginfo.si_signo == FABIPC_SIGINTR)
    {
      RUNTIME_ASSERT(siginfo.si_pid == g_params.pid);
      RUNTIME_ASSERT(g_server_autorun);

      /* update the graph - check all nodes, re-run invalidated rules */
      fatal(node_full_refresh);
      fatal(graph_invalidation_begin, &invalidation);
      fatal(module_full_refresh, &invalidation);
      graph_invalidation_end(&invalidation);
      fatal(rules_full_refresh, &rule_ctx);
      fatal(formula_full_refresh);

      bool building;
      fatal(goals_run, false, &building);
      continue;
    }

    // validate the client interaction
    fatal(load_client_pid, &client_pid);
    fatal(xkill, client_pid, 0);
    fatal(sigutil_assert, FABIPC_SIGSYN, client_pid, &siginfo);

#if DEVEL
    g_logging_skip_reconfigure = false;
#endif

    if(!g_server_autorun) {
      fatal(redirect);
    }

    // release previous request, if any
    fatal(ixshmdt, &request_shm);

    // receive request from the client
    fatal(receive_request, &request_shm);

    struct {
      uint32_t len;
      char text[];
    } * msg = request_shm;

    request_buf = msg->text;
    request_buf_sz = msg->len;

#if DEVEL
process_request:
#endif
    /* request must end with two null bytes */
    RUNTIME_ASSERT(request_buf_sz > 2);
    RUNTIME_ASSERT(request_buf[request_buf_sz - 1] == 0);
    RUNTIME_ASSERT(request_buf[request_buf_sz - 2] == 0);

    // parse the request
    fatal(request_parser_parse, request_parser, request_buf, request_buf_sz, "fab-request", &request);

    if(log_would(L_PROTOCOL))
    {
      fatal(log_start, L_PROTOCOL, &N);
      xsayf("request (%zu) >>\n", request_buf_sz);
      fatal(request_say, request, N);
      fatal(log_finish);
    }

    // prepare shm for the response
    fatal(prepare_response_shm, &response_shm, &response_shm_id, &response_shm_size);

    // save a spot for the response length
    response_narrator = narrator_fixed_init(response_narrator_space, response_shm, response_shm_size);
    fatal(narrator_xsayw, response_narrator, (char[]) { 0xde, 0xad, 0xbe, 0xef }, 4);

    fatal(value_writer_open, &response_writer, response_narrator);
    fatal(value_writer_push_list, &response_writer);

#if DEVEL
if(g_server_no_initial_client && iteration != 0)
{
#endif
    /* update the graph - check all nodes, re-run invalidated rules */
    fatal(node_full_refresh);
    fatal(graph_invalidation_begin, &invalidation);
    fatal(module_full_refresh, &invalidation);
    graph_invalidation_end(&invalidation);
    fatal(rules_full_refresh, &rule_ctx);
#if DEVEL
}
#endif

    /* execute the client request */
    fatal(handler_context_reset, handler_ctx);
    handler_ctx->invalidation = &invalidation;
    fatal(graph_invalidation_begin, &invalidation);
    invalidation_counter = node_invalidation_counter;
    fatal(handler_dispatch, handler_ctx, request, &response_writer);
    graph_invalidation_end(&invalidation);

    if(request->final_command)
    {
      fatal(goals_run, true, &building);
      fatal(value_writer_push_mapping, &response_writer);
      fatal(value_writer_string, &response_writer, attrs32_name_byvalue(command_type_attrs, COMMAND_TYPE_OPT & request->final_command));

      if(building)
      {
        fatal(sigutil_wait, &sigs, &siginfo);
        if(g_params.shutdown)
          continue;

        if(build_stage_failure)
        {
          fatal(value_writer_string, &response_writer, "failed");
        }
        else
        {
          fatal(value_writer_string, &response_writer, "success");
        }
      }
      else
      {
        fatal(value_writer_string, &response_writer, "noop");
      }

      fatal(value_writer_pop_mapping, &response_writer);
    }

    fatal(value_writer_pop_list, &response_writer);
    fatal(value_writer_close, &response_writer);

    // stitch up the response length
    response_len = narrator_fixed_size(response_narrator) - 4;
    fatal(narrator_xseek, response_narrator, 0, NARRATOR_SEEK_SET, 0);
    fatal(narrator_xsayw, response_narrator, &response_len, sizeof(response_len));

    // send and release the response
    fatal(send_response
      , client_pid
      , &sigs
      , response_shm
      , response_shm_size
      , response_shm_id
    );

    if(client_pid == 0)
    {
      fatal(request_ixfree, &request);
    }
    else
    {
      fatal(ixshmdt, &response_shm);
    }

    /* autorun */
    if(node_invalidation_counter != invalidation_counter && g_server_autorun)
    {
      fatal(goals_run, true, &building);
      if(building) {
        fatal(sigutil_wait, &sigs, &siginfo);
      }
    }

    fatal(usage_report);
    fatal(stats_report);
  }

finally:
  // locals
  fatal(request_xfree, request);
  fatal(xshmdt, request_shm);
  fatal(xshmdt, response_shm);
  fatal(handler_context_ixfree, &handler_ctx);
  fatal(request_parser_xfree, request_parser);
  graph_invalidation_end(&invalidation);
  fatal(rule_run_context_xdestroy, &rule_ctx);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * server_thread_main(void * arg)
{
  enter;

  xapi R;

  logger_set_thread_name("server");
  logger_set_thread_categories(L_SERVER);
  fatal(server_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/server");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);
  return 0;
}

//
// public
//

xapi server_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, server_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
