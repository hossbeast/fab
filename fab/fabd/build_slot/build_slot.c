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
#include <sys/wait.h>

#include "xapi.h"
#include "types.h"

#include "logger/config.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xprctl.h"
#include "valyria/hashtable.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "narrator.h"
#include "moria/edge.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "value.h"

#include "fab/sigutil.h"
#include "fab/ipc.h"

#include "build_slot.h"
#include "node.h"
#include "path.h"
#include "formula.h"
#include "build_thread.internal.h"
#include "notify_thread.h"
#include "buildplan.h"
#include "epoll_attrs.h"
#include "logging.h"
#include "exec_builder.h"
#include "params.h"
#include "module.h"
#include "variant.h"
#include "node_operations.h"
#include "stats.h"

#include "common/atomic.h"
#include "common/assure.h"
#include "zbuffer.h"

struct insertion_sort_ctx {
  char ** base;
  char * def;

  int lx;
  int lc;
};

static int insertion_sort_cmp(const void * _ctx, const void * _item)
{
  struct insertion_sort_ctx *ctx = (typeof(ctx))_ctx;
  char ** item = (typeof(item))_item;

  ctx->lx = item - ctx->base;
  return ctx->lc = env_def_cmp(ctx->def, *item);
}

static size_t insertion_sort(char ** restrict dst, size_t dst_len, char ** src, uint16_t src_len)
{
  uint16_t x;
  size_t idx;
  struct insertion_sort_ctx ctx = (typeof(ctx)) {
      lc : -1
    , base : dst
  };

  for(x = 0; x < src_len; x++)
  {
    ctx.def = src[x];
    bsearch(&ctx, dst, dst_len, sizeof(*dst), insertion_sort_cmp);
    idx = ctx.lx;
    if(ctx.lc)
    {
      if(ctx.lc > 0)
        idx++;

      memmove(
          dst + (idx + 1)
        , dst + idx
        , (dst_len - idx) * sizeof(*dst)
      );
    }

    dst[idx] = src[x];
    if(ctx.lc)
      dst_len++;
  }

  return dst_len;
}

/*
 * post-fork setup, exec
 */
static xapi bsexec(build_slot * restrict bs)
{
  enter;

  int x;

  // kill if parent dies
  fatal(xprctl, PR_SET_PDEATHSIG, 9, 0, 0, 0);

  // stdin
  fatal(xdup2, build_devnull_fd, 0);

  // stdout
  fatal(xdup2, bs->stdout_pipe[1], 1);

  // stderr
  fatal(xdup2, bs->stderr_pipe[1], 2);

  // auxout
  fatal(xdup2, bs->auxout_pipe[1], 1001);

  // chdir to the module directory
  fatal(xfchdir, bs->mod->dirnode_fd);

  for(x = 0; x < 1024; x++)
  {
    if(x == 0 || x == 1 || x == 2 || x == 1001)
      continue;

    close(x);
  }

// signals
// close high fds

  fatal(xexecve, bs->path, bs->argv, bs->envp);

  finally : coda;
}

static void bsexec_jump(build_slot * restrict bs)
{
  enter;

  xapi R = 0;
  fatal(bsexec, bs);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }

conclude(&R);
  xapi_teardown();

  exit(R);
}

//
// public
//

xapi build_slot_prep(build_slot * restrict bs, buildplan_entity * restrict bpe, uint32_t stage_index)
{
  enter;

  node *n = 0;
  edge *e;
  node_edge *ne;
  int x;

  bs->stage_index = stage_index;
  bs->bpe = bpe;
  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    bs->var = n->var;
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    ne = containerof(bpe, typeof(*ne), bpe);
    e = edge_containerof(ne);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      n = vertex_value(e->A);
      bs->var = n->var;
    }
    else
    {
      /* all nodes in a hyperedge must belong to the same module */
      if(e->Alen) {
        n = vertex_value(e->Alist[0].v);
      } else {
        n = vertex_value(e->Blist[0].v);
      }
      bs->var = n->var;

      /* they need not all belong to the same variant ; only set the variant
       * on the buildslot if all nodes agree */
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        if(n->var != bs->var)
        {
          bs->var = 0;
          break;
        }
      }
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        if(n->var != bs->var)
        {
          bs->var = 0;
          break;
        }
      }

      if(e->Alen) {
        n = vertex_value(e->Alist[0].v);
      } else {
        n = vertex_value(e->Blist[0].v);
      }
    }
  }

  if((bs->mod = node_module_get(n)) == 0)
  {
    /* use the project module for tasks under // */
    bs->mod = g_project_root->mod;
  }
  if(bs->var)
  {
    bs->vars = map_get(bs->mod->variant_var, MM(bs->var));
    bs->exec = map_get(bs->mod->variant_envs, MM(bs->var));
  }
  else
  {
    bs->vars = bs->mod->novariant_var;
    bs->exec = bs->mod->novariant_envs;
  }

  /* mark targets for modification to suppress notify events
   * until the next sweep epoch */
  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    n->notify_state = NOTIFY_SUPPRESS;
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    ne = containerof(bpe, typeof(*ne), bpe);
    e = edge_containerof(ne);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(ne->dir == EDGE_TGT_SRC)
        n = vertex_value(e->A);
      else
        n = vertex_value(e->B);

      n->notify_state = NOTIFY_SUPPRESS;
    }
    else if(ne->dir == EDGE_TGT_SRC)
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        n->notify_state = NOTIFY_SUPPRESS;
      }
    }
    else if(ne->dir == EDGE_SRC_TGT)
    {
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        n->notify_state = NOTIFY_SUPPRESS;
      }
    }
  }

  finally : coda;
}

xapi build_slot_fork_and_exec(build_slot * restrict bs)
{
  enter;

  const module * mod;
  const formula * fml;
  const value * vars;
  const exec * variant_exec = 0;
  exec * local_exec = 0;
  size_t envp_len = 0;
  uint16_t new_env_len;
  size_t args_size = 0;
  size_t args_len = 0;
  size_t envs_size = 0;

  RUNTIME_ASSERT(bs->bpe);
  RUNTIME_ASSERT(bs->bpe->fml);
  RUNTIME_ASSERT(bs->bpe->fml->self_fml);

  STATS_INC(bsexec);

  mod = bs->mod;
  fml = bs->bpe->fml->self_fml;
  vars = bs->vars;
  variant_exec = bs->exec;

  // pre-rendered envs, if any
  fatal(exec_builder_xreset, &bs->exec_builder);
  exec_render_context_configure(&bs->exec_builder_context, &bs->exec_builder, mod, vars, bs);

  // path if any
  if(fml->path)
    fatal(exec_render_path, &bs->exec_builder_context, fml->path);

  // formula args if any
  if(fml->args)
    fatal(exec_render_args, &bs->exec_builder_context, fml->args);

  // formula env vars
  if(fml->envs)
  {
    fatal(exec_render_envs, &bs->exec_builder_context, fml->envs);
  }

  // builtins
  fatal(exec_render_env_sysvars, &bs->exec_builder_context, bs);

  // build it
  fatal(exec_builder_build, &bs->exec_builder, &local_exec);

  /*
   * path setup
   */

  if(local_exec->path)
    bs->path = local_exec->path;
  else
    bs->path = fml->abspath;

  /*
   * args setup
   */

  args_len += 1;    // program name
  args_len += local_exec->args_size;

  args_size = args_len;
  args_size += 1;   // sentinel

  fatal(assure, &bs->argv_stor, sizeof(*bs->argv_stor), args_size, &bs->argv_stor_a);
  if(local_exec->filename)
    bs->argv_stor[0] = local_exec->filename;
  else
    bs->argv_stor[0] = fml->fml_node->name->name;

  memcpy(&bs->argv_stor[1], local_exec->args, sizeof(*local_exec->args) * local_exec->args_size);

  bs->argv_stor[args_len] = 0;
  bs->argv = bs->argv_stor;

  /*
   * env setup
   */

  if(variant_exec)
    envs_size += variant_exec->envs_size;
  envs_size += local_exec->envs_size;

  envs_size += 1;  // PATH
  envs_size += 1;  // sentinel

  fatal(assure, &bs->env_stor, sizeof(*bs->env_stor), envs_size, &bs->env_stor_a);

  if(variant_exec)
  {
    memcpy(bs->env_stor, variant_exec->envs, sizeof(*variant_exec->envs) * variant_exec->envs_size);
    envp_len += variant_exec->envs_size;
  }

  new_env_len = insertion_sort(bs->env_stor, envp_len, local_exec->envs, local_exec->envs_size);
  envp_len = new_env_len;

  if(g_params.searchpath)
  {
    new_env_len = insertion_sort(bs->env_stor, envp_len, &g_params.searchpath, 1);
    envp_len = new_env_len;
  }

  bs->env_stor[envp_len] = 0;
  bs->envp = bs->env_stor;

  fatal(xfork, &bs->pid);
  if(bs->pid == 0)
    bsexec_jump(bs);

  finally : coda;
}

xapi build_slot_read(build_slot * restrict bs, uint32_t stream)
{
  enter;

  int stream_part;
  int fd;
  char * buf;
  uint16_t * buf_len;
  uint32_t * total;
  uint16_t buf_size;
  ssize_t bytes;

  stream_part = 0;
  buf_size = 0;
  if(stream == 1)
  {
    stream_part = build_thread_cfg.capture_stdout;
    fd = bs->stdout_pipe[0];
    buf = bs->stdout_buf;
    buf_len = &bs->stdout_buf_len;
    buf_size = build_thread_cfg.stdout_buffer_size;
    total = &bs->stdout_total;
  }
  else if(stream == 2)
  {
    stream_part = build_thread_cfg.capture_stderr;
    fd = bs->stderr_pipe[0];
    buf = bs->stderr_buf;
    buf_len = &bs->stderr_buf_len;
    buf_size = build_thread_cfg.stderr_buffer_size;
    total = &bs->stderr_total;
  }
  else if(stream == 1001)
  {
    stream_part = build_thread_cfg.capture_auxout;
    fd = bs->auxout_pipe[0];
    buf = bs->auxout_buf;
    buf_len = &bs->auxout_buf_len;
    buf_size = build_thread_cfg.auxout_buffer_size;
    total = &bs->auxout_total;
  }

  if(stream_part == STREAM_PART_LEADING)
  {
    if(*buf_len < buf_size)
    {
      fatal(xread, fd, buf + *buf_len, buf_size - *buf_len, &bytes);
      *buf_len += bytes;
      *total += bytes;
    }
    else
    {
      stream_part = STREAM_PART_NONE;
    }
  }

  // discard excess
  if(stream_part == STREAM_PART_NONE)
  {
    fatal(xsplice, &bytes, fd, 0, build_devnull_fd, 0, 0xffff, SPLICE_F_MOVE);
    *total += bytes;
  }

  finally : coda;
}

xapi build_slot_reap(build_slot * restrict bs, uint32_t slot_index, siginfo_t *info)
{
  enter;

  int x;
  int exit;
  int sig;
  bool core;
  bool abnormal;
  uint32_t color = 0;
  char space[4096];
  size_t z;
  uint16_t blen;
  int lines;

  buildplan_entity *bpe;
  node *n;
  node_edge *ne;
  edge *e;

  bs->status = info->si_status;

  exit = 0;
  sig = 0;
  core = false;
  abnormal = false;
  if(WIFEXITED(bs->status))
  {
    exit = WEXITSTATUS(bs->status);
  }
  else if(WIFSIGNALED(bs->status))
  {
    sig = WTERMSIG(bs->status);
    core = WCOREDUMP(bs->status);
  }
  else
  {
    abnormal = true;
  }

  bs->success = true;
  if(bs->status != 0 || bs->stderr_buf_len > 0)
    bs->success = false;

  if(bs->success)
  {
    /* mark targets as up to date */
    bpe = bs->bpe;
    if(bpe->typemark == BPE_NODE)
    {
      n = containerof(bpe, typeof(*n), self_bpe);
      fatal(node_ok, n);
    }
    else if(bpe->typemark == BPE_NODE_EDGE)
    {
      ne = containerof(bpe, typeof(*ne), bpe);
      e = edge_containerof(ne);
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(ne->dir == EDGE_TGT_SRC)
          n = vertex_value(e->A);
        else
          n = vertex_value(e->B);
        fatal(node_ok, n);
      }
      else if(ne->dir == EDGE_TGT_SRC)
      {
        for(x = 0; x < e->Alen; x++)
        {
          n = vertex_value(e->Alist[x].v);
          fatal(node_ok, n);
        }
      }
      else if(ne->dir == EDGE_SRC_TGT)
      {
        for(x = 0; x < e->Blen; x++)
        {
          n = vertex_value(e->Blist[x].v);
          fatal(node_ok, n);
        }
      }
    }
  }

  /* re-enable inotify monitoring */
  bpe = bs->bpe;
  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    n->notify_epoch = notify_thread_epoch;
    n->notify_state = NOTIFY_EXPIRING;
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    ne = containerof(bpe, typeof(*ne), bpe);
    e = edge_containerof(ne);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(ne->dir == EDGE_TGT_SRC)
        n = vertex_value(e->A);
      else
        n = vertex_value(e->B);
      n->notify_epoch = notify_thread_epoch;
      n->notify_state = NOTIFY_EXPIRING;
    }
    else if(ne->dir == EDGE_TGT_SRC)
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        n->notify_epoch = notify_thread_epoch;
        n->notify_state = NOTIFY_EXPIRING;
      }
    }
    else if(ne->dir == EDGE_SRC_TGT)
    {
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        n->notify_epoch = notify_thread_epoch;
        n->notify_state = NOTIFY_EXPIRING;
      }
    }
  }

  if(log_would(L_BUILD))
  {
    if(bs->success)
      color |= L_GREEN;
    else
      color |= L_RED;

    // always show the program name, and targets
    fatal(log_xstart, L_BUILD, color, &N);
    fatal(narrator_xsayf, N, "%s", bs->argv[0]);
    fatal(narrator_xsays, N, " -> ");
    fatal(bpe_say_targets, bs->bpe, N);
    fatal(log_finish);

    if((bs->success && build_thread_cfg.success.show_path) || (!bs->success && build_thread_cfg.error.show_path))
    {
      xlogf(L_BUILD, color, " path %s", bs->path);
    }

    if((bs->success && build_thread_cfg.success.show_arguments) || (!bs->success && build_thread_cfg.error.show_arguments))
    {
      xlogf(L_BUILD, color, " argv %2d", (int)sentinel(bs->argv));
      for(x = 0; x < sentinel(bs->argv); x++)
        xlogf(L_BUILD, color, "  [%2d] %s", x, bs->argv[x]);
    }

    if((bs->success && build_thread_cfg.success.show_cwd) || (!bs->success && build_thread_cfg.error.show_cwd))
    {
      z = node_get_absolute_path(bs->mod->dir_node, space, sizeof(space));
      xlogf(L_BUILD, color, " cwd %.*s", (int)z, space);
    }

    if((bs->success && build_thread_cfg.success.show_command) || (!bs->success && build_thread_cfg.error.show_command))
    {
      z = 0;
      for(x = 0; x < sentinel(bs->argv); x++)
      {
        z += znloads(space + z, sizeof(space) - z, " ");
        z += znloads(space + z, sizeof(space) - z, bs->argv[x]);
      }

      xlogf(L_BUILD, color, " command %.*s", (int)z, space);
    }

    if((bs->success && build_thread_cfg.success.show_environment) || (!bs->success && build_thread_cfg.error.show_environment))
    {
      xlogf(L_BUILD, color, " envp %2d", (int)sentinel(bs->envp));
      for(x = 0; x < sentinel(bs->envp); x++)
        xlogf(L_BUILD, color, "  [%2d] %s", x, bs->envp[x]);
    }

    if((bs->success && build_thread_cfg.success.show_sources) || (!bs->success && build_thread_cfg.error.show_sources))
    {
      xlogf(L_BUILD, color, " sources %2d", 1);
      fatal(log_xstart, L_BUILD, color, &N);
      fatal(narrator_xsayf, N, "  [%2d] ", 0);
      fatal(bpe_say_sources, bs->bpe, N);
      fatal(log_finish);
    }

    if((bs->success && build_thread_cfg.success.show_targets) || (!bs->success && build_thread_cfg.error.show_targets))
    {
      xlogf(L_BUILD, color, " targets %2d", 1);
      fatal(log_xstart, L_BUILD, color, &N);
      fatal(narrator_xsayf, N, "  [%2d] ", 0);
      fatal(bpe_say_targets, bs->bpe, N);
      fatal(log_finish);
    }

    if((bs->success && build_thread_cfg.success.show_status) || (!bs->success && build_thread_cfg.error.show_status))
    {
      xlogf(L_BUILD, color, " status %d", bs->status);
      xlogf(L_BUILD, color, " exit %d signal %d core %s abnormal %s", exit, sig, core ? "true" : "false", abnormal ? "true" : "false");
    }

    if((bs->success && build_thread_cfg.success.show_stdout) || (!bs->success && build_thread_cfg.error.show_stdout))
    {
      blen = bs->stdout_buf_len;
      if(build_thread_cfg.success.show_stdout_limit_bytes > 0)
      {
        blen = MIN(blen, build_thread_cfg.success.show_stdout_limit_bytes);
      }

      if(build_thread_cfg.success.show_stdout_limit_lines > 0)
      {
        lines = 0;
        for(x = 0; x < blen; x++)
        {
          if(bs->stdout_buf[x] == '\n')
          {
            if(lines++ > build_thread_cfg.success.show_stdout_limit_lines)
              break;
          }
        }

        blen = MIN(blen, x);
      }

      if(blen || !bs->success)
      {
        fatal(log_xstart, L_BUILD, color, &N);
        if((bs->stdout_total > blen) || !bs->success)
          fatal(narrator_xsayf, N, " stdout - showing %"PRIu16" of %"PRIu32" bytes", blen, bs->stdout_total);
        if(blen)
        {
          fatal(narrator_xsays, N, "\n");
          fatal(narrator_xsayw, N, bs->stdout_buf, blen);
        }
        fatal(log_finish);
      }
    }

    if((bs->success && build_thread_cfg.success.show_stderr) || (!bs->success && build_thread_cfg.error.show_stderr))
    {
      blen = bs->stderr_buf_len;
      if(build_thread_cfg.success.show_stderr_limit_bytes > 0)
      {
        blen = MIN(blen, build_thread_cfg.success.show_stderr_limit_bytes);
      }

      if(build_thread_cfg.success.show_stderr_limit_lines > 0)
      {
        lines = 0;
        for(x = 0; x < blen; x++)
        {
          if(bs->stderr_buf[x] == '\n')
          {
            if(lines++ > build_thread_cfg.success.show_stderr_limit_lines)
              break;
          }
        }

        blen = MIN(blen, x);
      }

      if(blen || !bs->success)
      {
        fatal(log_xstart, L_BUILD, color, &N);
        if((bs->stderr_total > blen) || !bs->success)
          fatal(narrator_xsayf, N, " stderr - showing %"PRIu16" of %"PRIu32" bytes", blen, bs->stderr_total);
        if(blen)
        {
          fatal(narrator_xsays, N, "\n");
          fatal(narrator_xsayw, N, bs->stderr_buf, blen);
        }
        fatal(log_finish);
      }
    }

    if((bs->success && build_thread_cfg.success.show_auxout) || (!bs->success && build_thread_cfg.error.show_auxout))
    {
      blen = bs->auxout_buf_len;
      if(build_thread_cfg.success.show_auxout_limit_bytes > 0)
      {
        blen = MIN(blen, build_thread_cfg.success.show_auxout_limit_bytes);
      }

      if(build_thread_cfg.success.show_auxout_limit_lines > 0)
      {
        lines = 0;
        for(x = 0; x < blen; x++)
        {
          if(bs->auxout_buf[x] == '\n')
          {
            if(lines++ > build_thread_cfg.success.show_auxout_limit_lines)
              break;
          }
        }

        blen = MIN(blen, x);
      }

      if(blen || !bs->success)
      {
        if((bs->auxout_total > blen) || !bs->success)
          xlogf(L_BUILD, color, " auxout - showing %"PRIu16" of %"PRIu32" bytes", blen, bs->auxout_total);
        if(blen)
        {
          fatal(log_start, L_BUILD, &N);
          fatal(narrator_xsays, N, "\n");
          fatal(narrator_xsayw, N, bs->auxout_buf, blen);
          fatal(log_finish);
        }
      }
    }
  }

  finally : coda;
}
