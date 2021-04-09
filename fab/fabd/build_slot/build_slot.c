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

#include "fab/sigutil.h"
#include "fab/ipc.h"
#include "fab/events.h"
#include "fab/build_slot.h"
#include "fab/build_slot.desc.h"

#include "build_slot.h"
#include "fsent.h"
#include "formula.h"
#include "path_cache.h"
#include "build.h"
#include "notify_thread.h"
#include "buildplan.h"
#include "dependency.h"
#include "logging.h"
#include "exec.h"
#include "exec_builder.h"
#include "params.h"
#include "module.h"
#include "variant.h"
#include "stats.h"
#include "formula_value.h"
#include "events.h"
#include "channel.h"
#include "system_state.h"

#include "atomics.h"
#include "common/assure.h"
#include "zbuffer.h"
#include "marshal.h"
#include "hexdump.h"

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
    if(ctx.lc) {
      dst_len++;
    }
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
  int fd = -1;

  // kill me if parent dies, prevent zombies
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

  if(bs->file_fd != -1) {
    /*
     * It depends on the program, but with some interpreters (perl), when a shebang-script is
     * executed with fexec, the program is read from the current position of the file descriptor.
     * This causes two problems. 1) The fd is already the end, because we read and parsed the script
     * earlier, so if we do nothing then the interpreter just sees an empty program and exits
     * successfully after having done nothing. 2) It's not sufficient to simply seek to the
     * beginning, because the formula may be executed concurrently in several child processes, each
     * of which inherits the same open file description and seeks it to the end when reading the
     * program. Reopening a local copy of the file here solves both problems.
     */
    fatal(xopenatf, &fd, O_RDONLY, 0, "/proc/self/fd/%d", bs->file_fd);
  }

  // close fds
  for(x = 0; x < 1024; x++)
  {
    if(x == 0 || x == 1 || x == 2 || x == 1001 || x == fd) {
      continue;
    }

    close(x);
  }

  /* signals */

  if(fd != -1) {
    fatal(xfexecve, fd, bs->argv, bs->envp);
  } else {
    fatal(xexecve, bs->file_path, bs->argv, bs->envp);
  }

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

/* publish the exec-forked event */
static xapi build_slot_forked(build_slot * restrict bs)
{
  enter;

  size_t z;
  channel *chan;
  fabipc_message *msg;
  uint16_t len;
  uint32_t list_size;
  uint16_t list_size_off;
  uint16_t len_off;
  int x;
  llist list;
  moria_vertex *v;
  fsent *n;
  char *dst;
  size_t sz;

  /* struct fab_build_slot_info */
  if(!events_would(FABIPC_EVENT_FORMULA_EXEC_FORKED, &chan, &msg)) {
    goto XAPI_FINALIZE;
  }

  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_build_slot_info.id);

  /* pid, stage */
  z += marshal_u32(dst + z, sz - z, bs->pid);
  z += marshal_u16(dst + z, sz - z, bs->stage);

  /* path */
  if(bs->file_pe)
  {
    len = bs->file_pe->dir->len + 1 + bs->file_pe->len;
    z += marshal_u16(dst + z, sz - z, len);
    z += znloadw(dst + z, sz - z, bs->file_pe->dir->s, bs->file_pe->dir->len);
    z += znloadc(dst + z, sz - z, '/');
    z += znloadw(dst + z, sz - z, bs->file_pe->s, bs->file_pe->len);
  }
  else
  {
    len = strlen(bs->file_path);
    z += marshal_u16(dst + z, sz - z, len);
    z += znloadw(dst + z, sz - z, bs->file_path, len);
  }

  /* cwd */
  len = strlen(bs->mod->dir_node_abspath);
  z += marshal_u16(dst + z, sz - z, len);
  z += znloadw(dst + z, sz - z, bs->mod->dir_node_abspath, len);

  /* args */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);
  z += marshal_u16(dst + z, sz - z, sentinel(bs->argv));
  for(x = 0; x < sentinel(bs->argv); x++)
  {
    z += marshal_u32(dst + z, sz - z, descriptor_fab_build_string.id);
    len = strlen(bs->argv[x]);
    z += marshal_u16(dst + z, sz - z, len);
    z += znloadw(dst + z, sz - z, bs->argv[x], len);
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);

  /* envs */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);                   /* size */
  z += marshal_u16(dst + z, sz - z, sentinel(bs->envp));  /* length */
  for(x = 0; x < sentinel(bs->envp); x++)
  {
    z += marshal_u32(dst + z, sz - z, descriptor_fab_build_string.id);
    len = strlen(bs->envp[x]);
    z += marshal_u16(dst + z, sz - z, len);
    z += znloadw(dst + z, sz - z, bs->envp[x], len);
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);

  /* sources */
  llist_init_node(&list);
  dependency_sources(bs->bpe, &list);
  len = llist_count(&list);
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);   /* size */
  z += marshal_u16(dst + z, sz - z, len); /* length */
  llist_foreach(&list, v, lln) {
    z += marshal_u32(dst + z, sz - z, descriptor_fab_build_string.id);
    n = containerof(v, fsent, vertex);
    len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = fsent_path_znload(dst + z, sz - z, n);
    marshal_u16(dst + len_off, sz - len_off, len);
    z += len;
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);

  /* targets */
  llist_init_node(&list);
  dependency_targets(bs->bpe, &list);
  len = llist_count(&list);
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);
  z += marshal_u16(dst + z, sz - z, len);
  llist_foreach(&list, v, lln) {
    z += marshal_u32(dst + z, sz - z, descriptor_fab_build_string.id);
    n = containerof(v, fsent, vertex);
    len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = fsent_path_znload(dst + z, sz - z, n);
    marshal_u16(dst + len_off, sz - len_off, len);
    z += len;
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);

  msg->size = z;

  msg->id = bs->pid;
  events_publish(chan, msg);

  finally : coda;
}

/* publish the exec-waited event */
static xapi build_slot_waited(build_slot * restrict bs)
{
  enter;

  channel *chan;
  fabipc_message *msg;
  uint16_t z;

  if(!events_would(FABIPC_EVENT_FORMULA_EXEC_WAITED, &chan, &msg)) {
    goto XAPI_FINALIZE;
  }

  /* struct fab_build_slot_results */
  z = 0;
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, descriptor_fab_build_slot_results.id);
  z += marshal_i32(msg->text + z, sizeof(msg->text) - z, bs->status);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->stdout_total);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->stderr_total);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->auxout_total);

  msg->size = z;
  msg->id = bs->pid;
  events_publish(chan, msg);

  finally : coda;
}

//
// public
//

xapi build_slot_prep(build_slot * restrict bs, dependency * restrict dep, uint32_t stage_index)
{
  enter;

  fsent *n = 0;
  const moria_edge *e;
  int x;

  RUNTIME_ASSERT(dep->fml);

  bs->stage_index = stage_index;
  bs->bpe = dep;

  e = &dep->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(e->A, fsent, vertex); /* target */
    bs->var = n->var;
    bs->mod = fsent_module_get(n);
  }
  else
  {
    n = containerof(e->Alist[0].v, fsent, vertex);
    bs->mod = fsent_module_get(n);
    for(x = 0; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      if(fsent_module_get(n) != bs->mod)
      {
        bs->mod = 0;
        break;
      }
    }

    n = containerof(e->Alist[0].v, fsent, vertex);
    bs->var = n->var;
    for(x = 0; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      if(n->var != bs->var)
      {
        bs->var = 0;
        break;
      }
    }
  }

  if(bs->mod == 0)
  {
    /* fallback to the project module for e.g. tasks under // */
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
  e = &dep->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(e->A, fsent, vertex);
    n->notify_state = NOTIFY_SUPPRESS;
  }
  else
  {
    for(x = 0; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      n->notify_state = NOTIFY_SUPPRESS;
    }
  }

  finally : coda;
}

xapi build_slot_fork_and_exec(build_slot * restrict bs)
{
  enter;

  module * mod;
  const formula * fml;
  const value * vars;
  const exec * variant_exec = 0;
  exec * local_exec = 0;
  size_t envp_len = 0;
  uint16_t new_env_len;
  size_t args_len;
  size_t envs_size;
  const char *filename;

  RUNTIME_ASSERT(bs->bpe);
  RUNTIME_ASSERT(bs->bpe->fml);
  RUNTIME_ASSERT(bs->bpe->fml->self_fml);
  RUNTIME_ASSERT(bs->mod);

  STATS_INC(g_stats.bsexec);

  mod = bs->mod;
  fml = bs->bpe->fml->self_fml;
  vars = bs->vars;
  variant_exec = bs->exec;

  fatal(exec_builder_xreset, &bs->exec_builder);
  exec_render_context_configure(&bs->exec_builder_context, &bs->exec_builder, mod, vars, bs);

  // render file if any
  if(fml->file) {
    fatal(exec_render_file, &bs->exec_builder_context, fml->file);
    if(system_error) {
      goto XAPI_FINALLY;
    }
  }

  // render args if any
  if(fml->args) {
    fatal(exec_render_args, &bs->exec_builder_context, fml->args);
    if(system_error) {
      goto XAPI_FINALLY;
    }
  }

  // render envs if any
  if(fml->envs) {
    fatal(exec_render_envs, &bs->exec_builder_context, fml->envs);
    if(system_error) {
      goto XAPI_FINALLY;
    }
  }

  // render sysvars
  fatal(exec_render_env_sysvars, &bs->exec_builder_context, bs);

  // render
  fatal(exec_builder_build, &bs->exec_builder, &local_exec);

  // path setup
  bs->file_pe = 0;
  bs->file_path = 0;
  bs->file_fd = -1;
  if(local_exec->file_pe)   // execute file given by path-cache entry
  {
    bs->file_pe = local_exec->file_pe;
    bs->file_fd = local_exec->file_pe->fd;
    filename = local_exec->file_pe->filename;
  }
  else if(local_exec->path) // execute file given by absolute path
  {
    bs->file_path = local_exec->path;
    if((filename = strrchr(local_exec->path, '/')))
      filename++;
    else
      filename = local_exec->path;
  }
  else                      // execute formula file itself
  {
    bs->file_fd = fml->fd;
    bs->file_path = fml->self_node_abspath;
    filename = fml->self_node->name.name;
  }

  /*
   * args setup
   */

  args_len = 1;    // program name
  args_len += local_exec->args_size;

  fatal(assure, &bs->argv_stor, sizeof(*bs->argv_stor), args_len + 1 /* sentinel */, &bs->argv_stor_a);
  bs->argv_stor[0] = (void*)filename;
  memcpy(&bs->argv_stor[1], local_exec->args, sizeof(*local_exec->args) * local_exec->args_size);

  bs->argv_stor[args_len] = 0;
  bs->argv = bs->argv_stor;

  /*
   * env setup
   */

  envs_size = 0;
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
  if(bs->pid == 0) {
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
  channel *chan;
  fabipc_message *msg;

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

  if(events_would(event, &chan, &msg))
  {
    msg->id = bs->pid;
    fatal(xread, fd, msg->text, sizeof(msg->text), &bytes);
    msg->size = bytes;
    events_publish(chan, msg);
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
  const dependency *bpe;
  fsent *n;
  const moria_edge *e;
  bool success;

  bs->status = info->si_status;
  success = true;
  if(bs->status != 0 || bs->stderr_total > 0)
    success = false;

  /* re-enable inotify monitoring for the dependency targets */
  bpe = bs->bpe;
  e = &bpe->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(e->A, fsent, vertex);
    n->notify_epoch = notify_thread_epoch;
    n->notify_state = NOTIFY_EXPIRING;
    if(success) {
      fatal(fsent_ok, n);
    }
  }
  else
  {
    for(x = 0; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      n->notify_epoch = notify_thread_epoch;
      n->notify_state = NOTIFY_EXPIRING;
      if(success) {
        fatal(fsent_ok, n);
      }
    }
  }

  fatal(build_slot_waited, bs);

  finally : coda;
}
