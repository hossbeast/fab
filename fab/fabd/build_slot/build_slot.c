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
#include "value/writer.h"
#include "value/bacon_narrator.h"

#include "fab/sigutil.h"
#include "fab/ipc.h"
#include "fab/build_slot.desc.h"

#include "build_slot.h"
#include "node.h"
#include "path.h"
#include "formula.h"
#include "path_cache.h"
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
#include "formula_value.h"
#include "events.h"
#include "handler.h"
#include "channel.h"

#include "atomics.h"
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

  // close fds
  for(x = 0; x < 1024; x++)
  {
    if(x == 0 || x == 1 || x == 2 || x == 1001 || x == bs->file_fd)
      continue;

    close(x);
  }

  // signals

  if(bs->file_fd != -1)
    fatal(xfexecve, bs->file_fd, bs->argv, bs->envp);
  else
    fatal(xexecve, bs->file_path, bs->argv, bs->envp);

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
    xapi_backtrace(2, 0);
  }

conclude(&R);
  xapi_teardown();

  exit(R);
}

static xapi build_slot_forked(build_slot * restrict bs)
{
  enter;

  uint16_t z;
  handler_context *handler;
  fabipc_message *msg;
  uint16_t len;
  uint16_t list_size;
  uint16_t list_size_off;
  int x;
  llist list;
  vertex *v;
  node *n;
  uint32_t tail;

  if(!events_would(FABIPC_EVENT_FORMULA_EXEC_FORKED, &handler, &msg, &tail)) {
    goto XAPI_FINALIZE;
  }

  z = 0;

  /* stage */
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_stage.id);
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, bs->stage);

  /* path */
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_path.id);
  if(bs->file_path)
  {
    len = strlen(bs->file_path);
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->file_path, len);
  }
  else  // path-cache entry
  {
    len = bs->file_pe->dir->len + 1 + bs->file_pe->len;
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->file_pe->dir->s, bs->file_pe->dir->len);
    z += znloadc(msg->text + z, sizeof(msg->text) - z, '/');
    z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->file_pe->s, bs->file_pe->len);
  }

  /* cwd */
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_pwd.id);
  len = strlen(bs->mod->dir_node_abspath);
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
  z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->mod->dir_node_abspath, len);

  /* args */
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_args.id);
  list_size_off = z;
  z += 2;
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, sentinel(bs->argv));
  for(x = 0; x < sentinel(bs->argv); x++)
  {
    len = strlen(bs->argv[x]);
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->argv[x], len);
  }
  list_size = z - list_size_off - 4;  // excluding the size itself and the element count
  znload_u16(msg->text + list_size_off, 2, list_size);

  /* envs */
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_envs.id);
  list_size_off = z;
  z += 2;
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, sentinel(bs->envp));
  for(x = 0; x < sentinel(bs->envp); x++)
  {
    len = strlen(bs->envp[x]);
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += znloadw(msg->text + z, sizeof(msg->text) - z, bs->envp[x], len);
  }
  list_size = z - list_size_off - 4;
  znload_u16(msg->text + list_size_off, 2, list_size);

  /* sources */
  llist_init_node(&list);
  bpe_sources(bs->bpe, &list);
  len = llist_count(&list);
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_sources.id);
  list_size_off = z;
  z += 2;
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);

  llist_foreach(&list, v, lln) {
    n = vertex_value(v);
    if((sizeof(msg->text) - z) < 3) {
      break;
    }

    // 2-byte alignment
    z += (z & 1);
    len = node_path_znload(msg->text + z + 2, sizeof(msg->text) - z - 4, n);
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += len;
  }
  list_size = z - list_size_off - 4;
  znload_u16(msg->text + list_size_off, 2, list_size);

  /* targets */
  llist_init_node(&list);
  bpe_targets(bs->bpe, &list);
  len = llist_count(&list);
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_targets.id);
  list_size_off = z;
  z += 2;
  z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);

  llist_foreach(&list, v, lln) {
    n = vertex_value(v);
    if((sizeof(msg->text) - z) < 3) {
      break;
    }

    // 2-byte alignment
    z += (z & 1);
    len = node_path_znload(msg->text + z + 2, sizeof(msg->text) - z - 4, n);
    z += znload_u16(msg->text + z, sizeof(msg->text) - z, len);
    z += len;
  }
  list_size = z - list_size_off - 4;
  znload_u16(msg->text + list_size_off, 2, list_size);

  msg->size = z;
  msg->id = bs->pid;
  events_publish(handler, msg, tail);

  finally : coda;
}

static xapi build_slot_waited(build_slot * restrict bs)
{
  enter;

  handler_context *handler;
  fabipc_message *msg;
  uint16_t z;
  uint32_t tail;

  if(!events_would(FABIPC_EVENT_FORMULA_EXEC_WAITED, &handler, &msg, &tail)) {
    goto XAPI_FINALIZE;
  }

  z = 0;

  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_status.id);
  z += znload_i32(msg->text + z, sizeof(msg->text) - z, bs->status);

  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_stdout_total.id);
  z += znload_u32(msg->text + z, sizeof(msg->text) - z, bs->stdout_total);

  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_stderr_total.id);
  z += znload_u32(msg->text + z, sizeof(msg->text) - z, bs->stderr_total);

  z += znload_u16(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_auxout_total.id);
  z += znload_u32(msg->text + z, sizeof(msg->text) - z, bs->auxout_total);

  msg->size = z;
  msg->id = bs->pid;
  events_publish(handler, msg, tail);

  finally : coda;
}

//
// public
//

xapi build_slot_prep(build_slot * restrict bs, buildplan_entity * restrict bpe, uint32_t stage_index)
{
  enter;

  node *n = 0;
  edge *e;
  node_edge_dependency *ne;
  int x;

  bs->stage_index = stage_index;
  bs->bpe = bpe;
  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    bs->var = n->var;
  }
  else if(bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
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
  else if(bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
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
  const char *filename;

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

//printf("fml name %s\n", fml->fml_node->name->name);
//char space[512];
//node_get_absolute_path(fml->fml_node, space, sizeof(space));
//printf("fml path %s\n", space);
//printf("fml file %p ", fml->file);
//fflush(stdout);
//if(fml->file)
//  fatal(formula_value_say, fml->file, g_narrator_stdout);
//printf("\n");
//printf("fml envs %p ", fml->envs);
//fflush(stdout);
//if(fml->envs)
//  fatal(formula_value_say, fml->envs, g_narrator_stdout);
//printf("\n");
//printf("fml args %p ", fml->args);
//fflush(stdout);
//if(fml->args)
//  fatal(formula_value_say, fml->args, g_narrator_stdout);
//printf("\n");
  // file to execute if any
  if(fml->file)
    fatal(exec_render_file, &bs->exec_builder_context, fml->file);

//  if(fml->path)
//    fatal(exec_render_path, &bs->exec_builder_context, fml->path);

  // args to pass if any
  if(fml->args)
    fatal(exec_render_args, &bs->exec_builder_context, fml->args);

  // env vars to pass if any
  if(fml->envs)
    fatal(exec_render_envs, &bs->exec_builder_context, fml->envs);

  // builtins
  fatal(exec_render_env_sysvars, &bs->exec_builder_context, bs);

  // render
  fatal(exec_builder_build, &bs->exec_builder, &local_exec);

  /*
   * path setup
   */

  bs->file_pe = 0;
  bs->file_path = 0;
  bs->file_fd = -1;
  if(local_exec->file_pe)   // execute file given by path-cache entry
  {
    bs->file_pe = local_exec->file_pe;
    bs->file_fd = local_exec->file_pe->fd;
    filename = local_exec->file_pe->filename;
  }
  else if(local_exec->file) // execute file given by absolute path
  {
    bs->file_path = local_exec->file;
    if((filename = strrchr(local_exec->file, '/')))
      filename++;
    else
      filename = local_exec->file;
  }
  else                      // execute a formula directly
  {
    bs->file_fd = fml->fd;
    bs->file_path = fml->abspath;
    filename = fml->fml_node->name->name;
  }

//  if(local_exec->file && local_exec->file[0] == '/')
//  {
//    bs->file_path = local_exec->file;
//printf("abs file %s\n", local_exec->file);
//  }
//  else if(local_exec->file)
//  {
//    fatal(formula_path_search, &bs->file_fd, local_exec->file, local_exec->file_len);
//printf("path search %s -> %d\n", local_exec->file, bs->file_fd);
//  }
//  else
//  {
//    bs->file_path = fml->abspath;
//printf("fml abspath %s\n", fml->abspath);
//  }

  /*
   * args setup
   */

  args_len += 1;    // program name
  args_len += local_exec->args_size;

  args_size = args_len;
  args_size += 1;   // sentinel

  fatal(assure, &bs->argv_stor, sizeof(*bs->argv_stor), args_size, &bs->argv_stor_a);
//  if(local_exec->filename)
//    bs->argv_stor[0] = local_exec->filename;
//  else
//    bs->argv_stor[0] = fml->fml_node->name->name;

  bs->argv_stor[0] = (void*)filename;
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

  // $PATH if specified
  if(path_cache_env_path)
  {
    new_env_len = insertion_sort(bs->env_stor, envp_len, &path_cache_env_path, 1);
    envp_len = new_env_len;
  }

  bs->env_stor[envp_len] = 0;
  bs->envp = bs->env_stor;

  fatal(xfork, &bs->pid);
  if(bs->pid == 0)
  {
    bsexec_jump(bs);
  }

  fatal(build_slot_forked, bs);

  finally : coda;
}

xapi build_slot_read(build_slot * restrict bs, uint32_t stream)
{
  enter;

  ssize_t bytes;
  int fd = 0;
  uint32_t * total = 0;
  fabipc_event_type event = 0;
  handler_context *handler;
  fabipc_message *msg;
  uint32_t tail;

  if(stream == 1)
  {
    fd = bs->stdout_pipe[0];
    total = &bs->stdout_total;
    event = FABIPC_EVENT_FORMULA_EXEC_STDOUT;
  }
  else if(stream == 2)
  {
    fd = bs->stderr_pipe[0];
    total = &bs->stderr_total;
    event = FABIPC_EVENT_FORMULA_EXEC_STDERR;
  }
  else if(stream == 1001)
  {
    fd = bs->auxout_pipe[0];
    total = &bs->auxout_total;
    event = FABIPC_EVENT_FORMULA_EXEC_AUXOUT;
  }

  if(events_would(event, &handler, &msg, &tail))
  {
    msg->id = bs->pid;
    fatal(xread, fd, msg->text, sizeof(msg->text), &bytes);
    msg->size = bytes;
    events_publish(handler, msg, tail);
  }
  else
  {
    fatal(xsplice, &bytes, fd, 0, build_devnull_fd, 0, 0xffff, SPLICE_F_MOVE);
  }
  *total += bytes;

  finally : coda;
}

xapi build_slot_reap(build_slot * restrict bs, siginfo_t *info)
{
  enter;

  int x;
  buildplan_entity *bpe;
  node *n;
  node_edge_dependency *ne;
  edge *e;
  bool success;

  bs->status = info->si_status;

  success = true;
  if(bs->status != 0 || bs->stderr_total > 0)
    success = false;

  if(success)
  {
    /* mark targets as up to date */
    bpe = bs->bpe;
    if(bpe->typemark == BPE_NODE)
    {
      n = containerof(bpe, typeof(*n), self_bpe);
      fatal(node_ok, n);
    }
    else if(bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
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
  else if(bpe->typemark == BPE_NODE_EDGE_DEPENDENCY)
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

  fatal(build_slot_waited, bs);

  finally : coda;
}
