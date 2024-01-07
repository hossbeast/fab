/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>

#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xprctl.h"
#include "valyria/hashtable.h"
#include "valyria/map.h"
#include "narrator.h"
#include "moria/edge.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "bacon/value.h"
#include "bacon/writer.h"

#include "bam/sigutil.h"
#include "bam/ipc.h"
#include "bam/events.h"
#include "bam/build_slot.h"
#include "bam/build_slot.desc.h"

#include "build_slot.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "formula.h"
#include "path_cache.h"
#include "build.h"
#include "notify_thread.h"
#include "buildplan.h"
#include "buildplan_entity.h"
#include "rule_module_association.h"
#include "rule.h"
#include "exec.h"
#include "exec_builder.h"
#include "params.h"
#include "module.h"
#include "stats.h"
#include "formula_value.h"
#include "events.h"
#include "channel.h"
#include "system_state.h"
#include "entlist.h"
#include "sysvar.h"

#include "atomics.h"
#include "common/assure.h"
#include "zcat.h"
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
    (void)!bsearch(&ctx, dst, dst_len, sizeof(*dst), insertion_sort_cmp);
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
static void bsexec(build_slot * restrict bs)
{
  int x;
  int fd = -1;

  // kill me if parent dies, prevent zombies
  xprctl(PR_SET_PDEATHSIG, 9, 0, 0, 0);

  // stdin
  xdup2(build_devnull_fd, 0);

  // stdout
  xdup2(bs->stdout_pipe[1], 1);

  // stderr
  xdup2(bs->stderr_pipe[1], 2);

  // auxout
  xdup2(bs->auxout_pipe[1], 1001);

  // owning modules directory
  xchdirs(bs->cwd);

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
    fd = xopenatf(O_RDONLY, 0, "/proc/self/fd/%d", bs->file_fd);
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
    x = fexecve(fd, bs->argv, bs->envp);
  } else {
    x = execve(bs->file_path, bs->argv, bs->envp);
  }
  if(x == 0) {
    return;
  }

  dprintf(1, "%s\n", strerror(errno));
  RTA;
  exit(1);
}

/* publish exec-forked events for this slot */
static void build_slot_forked(build_slot * restrict bs)
{
  size_t z;
  channel *chan;
  bamipc_message *msg;
  uint16_t len;
  uint32_t list_size;
  uint16_t list_size_off;
  uint16_t list_len_off;
  uint16_t len_off;
  int x;
  fsent *n;
  char *dst;
  size_t sz;

  /* lists tracking */
  uint16_t argv_count;
  uint16_t argv_x;
  uint16_t envp_count;
  uint16_t envp_x;

  entlist sources;
  uint16_t sources_count;
  uint16_t sources_x;
  moria_vertex *source;
  entlist *sources_eln;

  entlist targets;
  uint16_t targets_count;
  uint16_t targets_x;
  moria_vertex *target;
  entlist *targets_eln;

  /* struct bam_build_slot_info */
  if(!events_would(BAMIPC_EVENT_FORMULA_EXEC_FORKED, &chan, &msg)) {
    return;
  }

  argv_count = sentinel(bs->argv);
  argv_x = 0;
  envp_count = sentinel(bs->envp);
  envp_x = 0;

  buildplan_entity_sources(bs->bpe, &sources);
  sources_count = llist_count(&sources.lln);
  sources_eln = llist_first(&sources.lln, typeof(*sources_eln), lln); // cursor
  sources_x = 0;

  buildplan_entity_targets(bs->bpe, &targets);
  targets_count = llist_count(&targets.lln);
  targets_eln = llist_first(&targets.lln, typeof(*targets_eln), lln); // cursor
  targets_x = 0;

  while(1)
  {
    dst = msg->text;
    sz = sizeof(msg->text);
    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_bam_build_slot_info.id);

    /* pid, stage */
    z += marshal_u32(dst + z, sz - z, bs->pid);
    z += marshal_u16(dst + z, sz - z, bs->stage);

    /* rule */
    z += marshal_u16(dst + z, sz - z, bs->bpe->rma->rule->id_len);
    z += zcatw(dst + z, sz - z, bs->bpe->rma->rule->id, bs->bpe->rma->rule->id_len);

    /* formula */
    z += marshal_u16(dst + z, sz - z, bs->bpe->fml->abspath_len);
    z += zcatw(dst + z, sz - z, bs->bpe->fml->abspath, bs->bpe->fml->abspath_len);

    /* path */
    if(bs->file_pe)
    {
      len = bs->file_pe->dir->len + 1 + bs->file_pe->len;
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, bs->file_pe->dir->s, bs->file_pe->dir->len);
      z += zcatc(dst + z, sz - z, '/');
      z += zcatw(dst + z, sz - z, bs->file_pe->s, bs->file_pe->len);
    }
    else
    {
      len = strlen(bs->file_path);
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, bs->file_path, len);
    }

    /* cwd */
    len = bs->cwd_len;// bs->mod->dir_node_abspath_len;
    z += marshal_u16(dst + z, sz - z, len);
    z += zcatw(dst + z, sz - z, bs->cwd, len); //  bs->mod->dir_node_abspath, len);

    /* args */
    z += marshal_u16(dst + z, sz - z, argv_count);          /* total items in argv */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);                   /* size of argv blob in this message */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);                   /* args items in this message */
    x = 0;
    for(; argv_x < argv_count; argv_x++)
    {
      len = strlen(bs->argv[argv_x]);
      if((z + len + 256) > (sz - z)) {
        break;
      }

      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, bs->argv[argv_x], len);
      x++;
    }
    list_size = z - list_size_off;
    marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
    marshal_u16(dst + list_len_off, sz - list_len_off, x);

    /* envs */
    z += marshal_u16(dst + z, sz - z, envp_count);          /* total items in envp  */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);                   /* size of envs blob in this message */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);                   /* env items in in this message */
    x = 0;
    for(; envp_x < envp_count; envp_x++)
    {
      len = strlen(bs->envp[envp_x]);
      if((z + len + 256) > (sz - z)) {
        break;
      }

      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, bs->envp[envp_x], len);
      x++;
    }
    list_size = z - list_size_off;
    marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
    marshal_u16(dst + list_len_off, sz - list_len_off, x);

    /* sources */
    z += marshal_u16(dst + z, sz - z, sources_count);   /* count */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);               /* size */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);               /* length */
    x = 0;

    for(; sources_eln; sources_eln = llist_next(&sources.lln, sources_eln, lln)) {
      if((z + 512 + 256) > (sz - z)) {
        break;
      }

      source = sources_eln->p;
      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      n = containerof(source, fsent, vertex);
      len_off = z;
      z += marshal_u16(dst + z, sz - z, 0);
      len = fsent_path_cat(dst + z, sz - z, n);
      marshal_u16(dst + len_off, sz - len_off, len);
      z += len;

      x++;
      sources_x++;
    }
    list_size = z - list_size_off;
    marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
    marshal_u16(dst + list_len_off, sz - list_len_off, x);

    /* targets */
    z += marshal_u16(dst + z, sz - z, targets_count);/* count */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);            /* size */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);            /* length */
    x = 0;
    for(; targets_eln; targets_eln = llist_next(&targets.lln, targets_eln, lln)) {
      if((z + 512 + 256) > (sz - z)) {
        break;
      }

      target = targets_eln->p;
      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      n = containerof(target, fsent, vertex);
      len_off = z;
      z += marshal_u16(dst + z, sz - z, 0);
      len = fsent_path_cat(dst + z, sz - z, n);
      marshal_u16(dst + len_off, sz - len_off, len);
      z += len;

      x++;
      targets_x++;
    }
    list_size = z - list_size_off;
    marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
    marshal_u16(dst + list_len_off, sz - list_len_off, x);

    msg->size = z;
    msg->id = bs->pid;
    events_publish(chan, msg);

    RUNTIME_ASSERT(argv_x <= argv_count);
    RUNTIME_ASSERT(envp_x <= envp_count);
    RUNTIME_ASSERT(sources_x <= sources_count);
    RUNTIME_ASSERT(targets_x <= targets_count);
    if(argv_x == argv_count && envp_x == envp_count && sources_x == sources_count && targets_x == targets_count) {
      break;
    }

    if(!events_would(BAMIPC_EVENT_FORMULA_EXEC_FORKED, &chan, &msg)) {
      return;
    }
  }

  entlist_destroy(&sources);
  entlist_destroy(&targets);
}

/* publish the exec-waited event */
static void build_slot_waited(build_slot * restrict bs)
{
  channel *chan;
  bamipc_message *msg;
  uint16_t z;

  if(!events_would(BAMIPC_EVENT_FORMULA_EXEC_WAITED, &chan, &msg)) {
    return;
  }

  /* struct bam_build_slot_results */
  z = 0;
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, descriptor_bam_build_slot_results.id);
  z += marshal_i32(msg->text + z, sizeof(msg->text) - z, bs->status);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->stdout_total);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->stderr_total);
  z += marshal_u32(msg->text + z, sizeof(msg->text) - z, bs->auxout_total);

  msg->size = z;
  msg->id = bs->pid;
  events_publish(chan, msg);
}

//
// public
//

void build_slot_fork_and_exec(build_slot * restrict bs, buildplan_entity * restrict bpe, uint32_t stage_index)
{
  module * mod;
  const formula * fml;
  struct exec * exec = 0;
  size_t envp_count = 0;
  uint16_t new_env_len;
  size_t args_len;
  size_t envs_size;
  const char *filename;
  moria_vertex *v;
  fsent *n;
  entlist targets, *eln;
  const bacon_value *vars;
  const fsent *cwd;

  RUNTIME_ASSERT(bpe->fml);

  STATS_INC(g_stats.fml_exec);
  STATS_INC(bpe->fml->fml_stats.fml_exec);

  /* determine (optional) context module from targets */
  mod = 0;
  buildplan_entity_targets(bpe, &targets);
  entlist_foreach(&targets, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    if(mod && fsent_module_get(n) != mod) {
      mod = 0;
      break;
    }
    mod = fsent_module_get(n);
  }

  /* mark targets for modification to suppress notify events
   * until the next sweep epoch */
  entlist_foreach(&targets, n, eln) {
    n->notify_state = NOTIFY_SUPPRESS;
  }
  entlist_destroy(&targets);

  bs->stage_index = stage_index;
  bs->bpe = bpe;
  if(mod) {
    bs->cwd = mod->dir_node_abspath;
    bs->cwd_len = mod->dir_node_abspath_len;
    vars = mod->vars;
    cwd = mod->dir_node;
  } else {
    bs->cwd = g_base_path;
    bs->cwd_len = g_base_path_len;
    vars = 0;
    cwd = g_base;
  }

  fml = bs->bpe->fml;

  // render sysvars
  bacon_value_free(&bs->sysvar_parser, bs->sysvars);
  bs->sysvars = 0;
  sysvar_render(&bs->sysvars, &bs->sysvar_parser, bs->bpe, cwd);

  exec_builder_reset(&bs->exec_builder);
  exec_render_context_configure(&bs->exec_builder_context, &bs->exec_builder, cwd, mod, vars, bs->sysvars, bs);

  // render file if any
  if(fml->file) {
    exec_render_file(&bs->exec_builder_context, fml->file);
    if(system_error) {
      return;
    }
  }

  // render args if any
  if(fml->args) {
    exec_render_args(&bs->exec_builder_context, fml->args);
    if(system_error) {
      return;
    }
  }

  // render envs if any
  if(fml->envs) {
    exec_render_envs(&bs->exec_builder_context, fml->envs);
    if(system_error) {
      return;
    }
  }

  // render env sysvars
  exec_render_env_vars(&bs->exec_builder_context);

  // render
  exec_builder_build(&bs->exec_builder, &exec);

  // path setup
  bs->file_pe = 0;
  bs->file_path = 0;
  bs->file_fd = -1;
  if(exec->file_pe)   // execute file given by path-cache entry
  {
    RUNTIME_ASSERT(exec->file_pe->fd != -1);
    bs->file_pe = exec->file_pe;
    bs->file_fd = exec->file_pe->fd;
    filename = exec->file_pe->filename;
  }
  else if(exec->path) // execute file given by absolute path
  {
    bs->file_path = exec->path;
    if((filename = strrchr(exec->path, '/')))
      filename++;
    else
      filename = exec->path;
  }
  else                // execute formula file itself
  {
    bs->file_fd = fml->fd;
    bs->file_path = fml->abspath;
    filename = fml->self_node->name.name;
  }

  /*
   * args setup
   */

  args_len = 1;    // program name
  args_len += exec->args_size;

  assure(&bs->argv_stor, sizeof(*bs->argv_stor), args_len + 1 /* sentinel */, &bs->argv_stor_a);
  bs->argv_stor[0] = (void*)filename;
  memcpy(&bs->argv_stor[1], exec->args, sizeof(*exec->args) * exec->args_size);

  bs->argv_stor[args_len] = 0;
  bs->argv = bs->argv_stor;

  /*
   * env setup
   */

  envs_size = 0;
  if(mod && mod->exec) {
    envs_size += mod->exec->envs_size;
  }
  envs_size += exec->envs_size;

  envs_size += 1;  // PATH
  envs_size += 1;  // sentinel

  assure(&bs->env_stor, sizeof(*bs->env_stor), envs_size, &bs->env_stor_a);

  /* pre-rendered envs from mod->vars */
  if(mod && mod->exec)
  {
    memcpy(bs->env_stor, mod->exec->envs, sizeof(*mod->exec->envs) * mod->exec->envs_size);
    envp_count += mod->exec->envs_size;
  }

  new_env_len = insertion_sort(bs->env_stor, envp_count, exec->envs, exec->envs_size);
  envp_count = new_env_len;

  // $PATH if specified
  if(path_cache_env_path)
  {
    new_env_len = insertion_sort(bs->env_stor, envp_count, &path_cache_env_path, 1);
    envp_count = new_env_len;
  }

  bs->env_stor[envp_count] = 0;
  bs->envp = bs->env_stor;

  bs->pid = xfork();
  if(bs->pid == 0) {
    bsexec(bs);
    /* does not return */
  }

  build_slot_forked(bs);
}

void build_slot_read(build_slot * restrict bs, uint32_t stream)
{
  ssize_t bytes;
  int fd = 0;
  uint32_t * total = 0;
  bamipc_event_type event = 0;
  channel *chan;
  bamipc_message *msg;

  if(stream == 1)
  {
    fd = bs->stdout_pipe[0];
    total = &bs->stdout_total;
    event = BAMIPC_EVENT_FORMULA_EXEC_STDOUT;
  }
  else if(stream == 2)
  {
    fd = bs->stderr_pipe[0];
    total = &bs->stderr_total;
    event = BAMIPC_EVENT_FORMULA_EXEC_STDERR;
  }
  else if(stream == 1001)
  {
    fd = bs->auxout_pipe[0];
    total = &bs->auxout_total;
    event = BAMIPC_EVENT_FORMULA_EXEC_AUXOUT;
  }

  if(events_would(event, &chan, &msg))
  {
    msg->id = bs->pid;
    bytes = xread(fd, msg->text, sizeof(msg->text));
    msg->size = bytes;
    events_publish(chan, msg);
  }
  else
  {
    bytes = xsplice(fd, 0, build_devnull_fd, 0, 0xffff, SPLICE_F_MOVE);
  }
  *total += bytes;
}

void build_slot_reap(build_slot * restrict bs, siginfo_t *info)
{
  fsent *n;
  bool success;
  entlist targets, *eln;

  bs->status = info->si_status;
  success = true;
  if(bs->status != 0 || bs->stderr_total > 0) {
    success = false;
  }

  /* re-enable inotify monitoring for the buildplan_entity targets */
  buildplan_entity_targets(bs->bpe, &targets);
  entlist_foreach(&targets, n, eln) {
    n->in_epoch = notify_thread_epoch;
    n->notify_state = NOTIFY_EXPIRING;
    if(success) {
      vertex_mark_fresh(&n->vertex);
    }
  }
  entlist_destroy(&targets);

  if(success) {
    STATS_INC(g_stats.fml_exec_ok);
    STATS_INC(bs->bpe->fml->fml_stats.fml_exec_ok);
  }

  build_slot_waited(bs);
}
