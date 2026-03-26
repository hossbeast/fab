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
#include "xlinux/xtime.h"
#include "valyria/hashtable.h"
#include "valyria/map.h"
#include "narrator.h"
#include "moria/edge.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "bacon/make.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"

#include "bam/sigutil.h"
#include "bam/ipc.h"
#include "bam/events.h"
#include "bam/build_slot.h"

#include "build_slot.h"
#include "formula_record.h"
#include "fsent.h"
#include "vertex.h"
#include "edge.h"
#include "formula.h"
#include "build.h"
#include "build/events.h"
#include "notify_thread.h"
#include "buildplan.h"
#include "bpe.h"
#include "rma.h"
#include "rule.h"
#include "exec.h"
#include "exec_builder.h"
#include "params.h"
#include "module.h"
#include "stats.h"
#include "exec_value.h"
#include "events/events.h"
#include "channel.h"
#include "system.h"
#include "edges.h"
#include "vertices.h"
#include "sysvar.h"
#include "walker.h"

#include "atomics.h"
#include "common/assure.h"
#include "zcat.h"
#include "marshal.h"
#include "times.h"

struct insertion_sort_ctx {
  char **base;
  char *def;
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

static __attribute__((unused)) size_t insertion_sort(char ** restrict dst, size_t dst_len, char ** src, uint16_t src_len)
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
static void __attribute__((noreturn)) bsexec(build_slot * restrict bs)
{
  int x;
  struct exec *exec;

  exec = bs->rec->exec;

  // kill me if parent dies, prevent zombies
  xprctl(PR_SET_PDEATHSIG, 9, 0, 0, 0);

  /* std file descriptors */
  xdup2(build_devnull_fd, 0);
  xdup2(bs->stdout_pipe[1], 1);
  xdup2(bs->stderr_pipe[1], 2);

  // auxout
  xdup2(bs->auxout_pipe[1], 1001);

  // owning modules directory
  xchdirs(exec->cwd);

  /* fds */
  for(x = 0; x < 1024; x++)
  {
    if(x == 0 || x == 1 || x == 2 || x == 1001) {
      continue;
    }

    close(x);
  }

  /* signals ? */

  execve(exec->file_path, exec->args, exec->envs);

  /* unreachable */
  dprintf(1, "%s\n", strerror(errno));
  exit(1);
}

/* publish exec-forked events for this slot */
static void build_slot_forked_binary(build_slot * restrict bs)
{
  size_t z;
  events_context ectx;
  bamipc_message *msg;
  uint16_t len;
  uint32_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  size_t len_off;
  int x;
  fsent *n;
  char *dst;
  size_t sz;
  char path[512];
  formula_record *rec;
  struct exec *exec;

  /* lists tracking */
  uint16_t argv_count;
  uint16_t argv_x;
  uint16_t envp_count;
  uint16_t envp_x;

  vertices sources;
  uint16_t sources_count;
  uint16_t sources_x;
  moria_vertex *source;
  vertices *sources_eln;

  vertices targets;
  uint16_t targets_count;
  uint16_t targets_x;
  moria_vertex *target;
  vertices *targets_eln;

  /* struct bam_build_slot_info */
  if(!(msg = events_binary_would(&ectx, BAMIPC_EVENT_FORMULA_FORK_BIN))) {
    return;
  }

  rec = bs->rec;
  exec = rec->exec;

  argv_count = sentinel(exec->args);
  argv_x = 0;
  envp_count = sentinel(exec->envs);
  envp_x = 0;

  buildplan_entity_sources(bs->bpe, &sources);
  sources_count = llist_count(&sources.lln);
  sources_eln = llist_first(&sources.lln, typeof(*sources_eln), lln); // cursor
  sources_x = 0;

  buildplan_entity_targets(bs->bpe, &targets);
  targets_count = llist_count(&targets.lln);
  targets_eln = llist_first(&targets.lln, typeof(*targets_eln), lln); // cursor
  targets_x = 0;

  dst = msg->text;
  sz = msg->size;
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_build_slot_info.id);

  /* pid, stage */
  z += marshal_u32(dst + z, sz - z, rec->pid);
  z += marshal_u16(dst + z, sz - z, rec->stage);

  /* bpe */
  z += marshal_u16(dst + z, sz - z, bs->bpe->id_len);
  z += zcatw(dst + z, sz - z, bs->bpe->id, bs->bpe->id_len);
  len = vertex_path_cat(path, sizeof(path), &bs->bpe->vertex);
  z += marshal_u16(dst + z, sz - z, len);
  z += zcatw(dst + z, sz - z, path, len);

  /* rma */
  z += marshal_u16(dst + z, sz - z, bs->bpe->rma->id_len);
  z += zcatw(dst + z, sz - z, bs->bpe->rma->id, bs->bpe->rma->id_len);
  len = vertex_path_cat(path, sizeof(path), &bs->bpe->rma->vertex);
  z += marshal_u16(dst + z, sz - z, len);
  z += zcatw(dst + z, sz - z, path, len);

  /* rule */
  z += marshal_u16(dst + z, sz - z, bs->bpe->rma->rule->id_len);
  z += zcatw(dst + z, sz - z, bs->bpe->rma->rule->id, bs->bpe->rma->rule->id_len);
  len = vertex_path_cat(path, sizeof(path), &bs->bpe->rma->rule->vertex);
  z += marshal_u16(dst + z, sz - z, len);
  z += zcatw(dst + z, sz - z, path, len);

  /* module */
  z += marshal_u16(dst + z, sz - z, bs->bpe->rma->mod->id_len);
  z += zcatw(dst + z, sz - z, bs->bpe->rma->mod->id, bs->bpe->rma->mod->id_len);
  z += marshal_u16(dst + z, sz - z, bs->bpe->rma->mod->build_dir_abspath_len);
  z += zcatw(dst + z, sz - z, bs->bpe->rma->mod->build_dir_abspath, bs->bpe->rma->mod->build_dir_abspath_len);

  /* formula */
  z += marshal_u16(dst + z, sz - z, bs->bpe->fml->id_len);
  z += zcatw(dst + z, sz - z, bs->bpe->fml->id, bs->bpe->fml->id_len);
  z += marshal_u16(dst + z, sz - z, bs->bpe->fml->abspath_len);
  z += zcatw(dst + z, sz - z, bs->bpe->fml->abspath, bs->bpe->fml->abspath_len);

  /* exec_path */
  len = exec->file_path_len;
  z += marshal_u16(dst + z, sz - z, len);
  z += zcatw(dst + z, sz - z, exec->file_path, len);

  /* cwd */
  z += marshal_u16(dst + z, sz - z, exec->cwd_len);
  z += zcatw(dst + z, sz - z, exec->cwd, exec->cwd_len);

  while(1)
  {
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
      len = strlen(exec->args[argv_x]);
      if((z + len + 256) > (sz - z)) {
        break;
      }

      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, exec->args[argv_x], len);
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
      len = strlen(rec->exec->envs[envp_x]);
      if((z + len + 256) > (sz - z)) {
        break;
      }

      z += marshal_u32(dst + z, sz - z, descriptor_bam_build_string.id);
      z += marshal_u16(dst + z, sz - z, len);
      z += zcatw(dst + z, sz - z, rec->exec->envs[envp_x], len);
      x++;
    }
    list_size = z - list_size_off;
    marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
    marshal_u16(dst + list_len_off, sz - list_len_off, x);

    /* sources */
    z += marshal_u16(dst + z, sz - z, sources_count);       /* total sources count */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);                   /* size of sources blob in this message */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);                   /* sources items in this message */
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
    z += marshal_u16(dst + z, sz - z, targets_count);       /* total targets count */
    z += align(z, 4);
    list_size_off = z;
    z += marshal_u32(dst + z, sz - z, 0);                   /* size of targets blob in this message */
    z += align(z, 2);
    list_len_off = z;
    z += marshal_u16(dst + z, sz - z, 0);                   /* targets items in this message */
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

    msg->len = z;
    msg->id = rec->pid;
    events_publish(&ectx);

    RUNTIME_ASSERT(argv_x <= argv_count);
    RUNTIME_ASSERT(envp_x <= envp_count);
    RUNTIME_ASSERT(sources_x <= sources_count);
    RUNTIME_ASSERT(targets_x <= targets_count);
    if(argv_x == argv_count && envp_x == envp_count && sources_x == sources_count && targets_x == targets_count) {
      break;
    }

    if(!(msg = events_binary_would(&ectx, BAMIPC_EVENT_FORMULA_FORK_BIN))) {
      break;
    }

    dst = msg->text;
    sz = msg->size;
    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_bam_build_slot_info.id);

    /* pid, stage */
    z += marshal_u32(dst + z, sz - z, rec->pid);
    z += marshal_u16(dst + z, sz - z, rec->stage);

    /* bpe */
    z += marshal_u16(dst + z, sz - z, 0);
    z += marshal_u16(dst + z, sz - z, 0);
    /* rma */
    z += marshal_u16(dst + z, sz - z, 0);
    z += marshal_u16(dst + z, sz - z, 0);
    /* rule */
    z += marshal_u16(dst + z, sz - z, 0);
    z += marshal_u16(dst + z, sz - z, 0);
    /* module */
    z += marshal_u16(dst + z, sz - z, 0);
    z += marshal_u16(dst + z, sz - z, 0);
    /* formula */
    z += marshal_u16(dst + z, sz - z, 0);
    z += marshal_u16(dst + z, sz - z, 0);
    /* path */
    z += marshal_u16(dst + z, sz - z, 0);
    /* cwd */
    z += marshal_u16(dst + z, sz - z, 0);
  }

  vertices_destroy(&sources);
  vertices_destroy(&targets);
}

static void build_slot_forked_bacon(build_slot * restrict bs)
{
  events_context ectx;
  bamipc_message *msg;
  fsent *n;

  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  /* lists tracking */
  vertices sources;
  moria_vertex *source;
  vertices *sources_eln;

  vertices targets;
  moria_vertex *target;
  vertices *targets_eln;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_FORMULA_FORK))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  buildplan_entity_sources(bs->bpe, &sources);
  sources_eln = llist_first(&sources.lln, typeof(*sources_eln), lln); // cursor

  buildplan_entity_targets(bs->bpe, &targets);
  targets_eln = llist_first(&targets.lln, typeof(*targets_eln), lln); // cursor

/* this can very clearly overflow the single message, it has to be a loop */

  { MAPPING
    STRING("forked")
    { SET
      { MAPPING STRING("pid") UINT(bs->rec->pid) }
      { MAPPING STRING("stage") UINT(bs->rec->stage) }
      { MAPPING STRING("bpe") BYTES(bs->bpe->id, bs->bpe->id_len) }
      { MAPPING STRING("rma") BYTES(bs->bpe->rma->id, bs->bpe->rma->id_len) }
      { MAPPING STRING("rule") BYTES(bs->bpe->rma->rule->id, bs->bpe->rma->rule->id_len) }
      { MAPPING STRING("formula") BYTES(bs->bpe->fml->id, bs->bpe->fml->id_len) }
      { MAPPING STRING("path") BYTES(bs->rec->exec->file_path, bs->rec->exec->file_path_len) }
      { MAPPING
        STRING("sources")
        { LIST
          for(; sources_eln; sources_eln = llist_next(&sources.lln, sources_eln, lln)) {
            source = sources_eln->p;
            n = containerof(source, fsent, vertex);
            bacon_writer_push_string(builder_writer);
            fsent_path_say(n, builder_writer->N);
            bacon_writer_pop_string(builder_writer);
          }
        }
      }
      { MAPPING
        STRING("targets")
        { LIST
          for(; targets_eln; targets_eln = llist_next(&targets.lln, targets_eln, lln)) {
            target = targets_eln->p;
            n = containerof(target, fsent, vertex);
            bacon_writer_push_string(builder_writer);
            fsent_path_say(n, builder_writer->N);
            bacon_writer_pop_string(builder_writer);
          }
        }
      }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;
  events_publish(&ectx);

  vertices_destroy(&sources);
  vertices_destroy(&targets);
}

/* publish exec-forked events for this slot */
static void build_slot_forked(build_slot * restrict bs)
{
  build_slot_forked_binary(bs);
  build_slot_forked_bacon(bs);
}

static void build_slot_reap_binary(build_slot * restrict bs)
{
  events_context ectx;
  bamipc_message *msg;
  uint16_t z;

  if(!(msg = events_binary_would(&ectx, BAMIPC_EVENT_FORMULA_REAP_BIN))) {
    return;
  }

  /* struct bam_build_slot_results */
  z = 0;
  z += marshal_u32(msg->text + z, msg->size - z, descriptor_bam_build_slot_results.id);
  z += marshal_i32(msg->text + z, msg->size - z, bs->rec->status);
  z += marshal_u64(msg->text + z, msg->size - z, timespec_delta(&bs->rec->forked_time, &bs->rec->waited_time));
  z += marshal_u32(msg->text + z, msg->size - z, bs->rec->stdout_total);
  z += marshal_u32(msg->text + z, msg->size - z, bs->rec->stderr_total);
  z += marshal_u32(msg->text + z, msg->size - z, bs->rec->auxout_total);

  msg->len = z;
  msg->id = bs->rec->pid;
  events_publish(&ectx);
}

static void build_slot_reap_bacon(build_slot * restrict bs)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  bamipc_message *msg;
  events_context ectx;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_FORMULA_REAP))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("waited")
    { SET
      { MAPPING STRING("pid") UINT(bs->rec->pid) }
      { MAPPING STRING("status") UINT(bs->rec->status) }
      { MAPPING STRING("runtime") UINT(timespec_delta(&bs->rec->forked_time, &bs->rec->waited_time)) }
      { MAPPING STRING("stdout-total") UINT(bs->rec->stdout_total) }
      { MAPPING STRING("stderr-total") UINT(bs->rec->stderr_total) }
      { MAPPING STRING("auxout-total") UINT(bs->rec->auxout_total) }
    }
  }

  msg->id = 0;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void render_targets(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const buildplan_entity * restrict bpe
  , const fsent * restrict base
)
{
  size_t count;
  const moria_vertex *v;
  const fsent *fse;
  struct vertices list;
  struct vertices *eln;
  bacon_value *key, *val, *mapping;
  char path[PATH_MAX];
  size_t z;
  int x;

  buildplan_entity_targets(bpe, &list);
  count = llist_count(&list.lln);

  bacon_string_mks(parser, 0, 0, &key, "bm_targets_len");
  bacon_string_mkf(parser, 0, 0, &val, "%zu", count);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  x = 0;
  vertices_foreach(&list, v, eln) {
    fse = containerof(v, typeof(*fse), vertex);
    z = fsent_relative_path_cat(path, sizeof(path), fse, base);
    bacon_string_mkf(parser, 0, 0, &key, "bm_target%d", x++);
    bacon_string_mkw(parser, 0, 0, &val, path, z);
    bacon_mapping_mk(parser, 0, &mapping, key, val);
    bacon_set_mkv(parser, 0, *vars, vars, mapping);
  }

  vertices_destroy(&list);
}

static void render_sources(
    bacon_value ** restrict vars
  , bacon_parser * restrict parser
  , const buildplan_entity * restrict bpe
  , const fsent * restrict base
)
{
  size_t count;
  const moria_vertex *v;
  const fsent *fse;
  vertices list;
  vertices *eln;
  bacon_value *key, *val, *mapping;
  char path[PATH_MAX];
  size_t z;
  int x;

  buildplan_entity_sources(bpe, &list);
  count = llist_count(&list.lln);

  bacon_string_mks(parser, 0, 0, &key, "bm_sources_len");
  bacon_string_mkf(parser, 0, 0, &val, "%zu", count);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, *vars, vars, mapping);

  x = 0;
  vertices_foreach(&list, v, eln) {
    fse = containerof(v, typeof(*fse), vertex);
    z = fsent_relative_path_cat(path, sizeof(path), fse, base);
    bacon_string_mkf(parser, 0, 0, &key, "bm_source%d", x++);
    bacon_string_mkw(parser, 0, 0, &val, path, z);
    bacon_mapping_mk(parser, 0, &mapping, key, val);
    bacon_set_mkv(parser, 0, *vars, vars, mapping);
  }

  vertices_destroy(&list);
}

static bacon_value *build_slot_sysvar_render(
    bacon_parser * restrict parser
  , const build_slot * restrict bs
  , const fsent * restrict base
)
{
  bacon_value *key, *val, *mapping;
  const buildplan_entity * restrict bpe;
  const formula_record * restrict rec;
  bacon_value *vars;

  bpe = bs->bpe;
  rec = bs->rec;

  /* properties that pertain to this bpe */
  vars = 0;
  render_sources(&vars, parser, bpe, base);
  render_targets(&vars, parser, bpe, base);

  /* properties that pertain to the build plan */
  bacon_string_mks(parser, 0, 0, &key, "bm_stage");
  bacon_string_mkf(parser, 0, 0, &val, "%"PRIu16, rec->stage);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_stage_index");
  bacon_string_mkf(parser, 0, 0, &val, "%"PRIu16, rec->stage_index);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  bacon_string_mks(parser, 0, 0, &key, "bm_stage_count");
  bacon_string_mkf(parser, 0, 0, &val, "%"PRIu16, rec->stage_count);
  bacon_mapping_mk(parser, 0, &mapping, key, val);
  bacon_set_mkv(parser, 0, vars, &vars, mapping);

  return vars;
}

//
// public
//

void build_slot_fork_and_exec(build_slot * restrict bs)
{
  module *mod;
  formula *fml;
  moria_vertex *v;
  fsent *n;
  vertices targets, *eln;
  const bacon_value *vars;
  const fsent *cwd;
  exec_builder *eb;
  exec_render_context *ebc;
  buildplan_entity *bpe;
  formula_record *rec;
  struct bacon_value *locals;

  bpe = bs->bpe;
  fml = bpe->fml;
  mod = bpe->rma->mod;
  rec = bs->rec;

  RUNTIME_ASSERT(bpe);
  RUNTIME_ASSERT(fml);
  RUNTIME_ASSERT(mod);
  RUNTIME_ASSERT(rec);

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &rec->forked_time);

  /* mark targets for modification to suppress notify events
   * until the next sweep epoch */
  buildplan_entity_targets(bpe, &targets);
  vertices_foreach(&targets, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    n->notify_state = NOTIFY_SUPPRESS;
  }
  vertices_destroy(&targets);

  vars = mod->vars;
  cwd = mod->build_dir;

  // append locals
  locals = build_slot_sysvar_render(&bs->sysvar_parser, bs, cwd);

  eb = &bs->exec_builder;
  ebc = &bs->exec_builder_context;
  exec_builder_reset(eb);
  exec_render_context_reset(ebc);
  exec_render_context_configure(ebc, eb, cwd, mod, vars, locals, bs);
  ebc->lookups = &bpe->rma->lookups;

  /* exec cwd is the module dir */
  ebc->builder_args.item = BUILDER_CWD;
  ebc->builder_args.val.s = mod->build_dir_abspath;
  ebc->builder_args.mode = BUILDER_REPLACE;
  ebc->builder_args.render_val = BUILDER_STRING;
  exec_builder_add(eb, &ebc->builder_args);

  /* render file from formula */
  if(fml->file) {
    exec_render_file(ebc, fml->file);
  } else {
    /* otherwise, we are executing the formula file itself */
    ebc->builder_args.item = BUILDER_FILE;
    ebc->builder_args.val.fse = fml->self_node;
    ebc->builder_args.mode = BUILDER_REPLACE;
    ebc->builder_args.render_val = BUILDER_FSENT;
    exec_builder_add(eb, &ebc->builder_args);
  }

  // append args from formula, if any
  if(fml->args) {
    exec_render_args(ebc, fml->args);
  }

  // append envs from formula, if any
  if(fml->envs) {
    exec_render_envs(ebc, fml->envs);
  }

  // append envs from sysvars, module vars, and locals
  exec_render_env_vars(ebc);

  // finalize all the strings
  exec_builder_build(eb, 0);
  exec_builder_take(eb, &rec->exec);

  bs->rec->pid = xfork();
  if(bs->rec->pid == 0) {
    bsexec(bs);
    /* does not return */
  }

  build_slot_forked(bs);

  bacon_value_free(&bs->sysvar_parser, locals);
}

size_t build_slot_read(build_slot * restrict bs, uint32_t stream)
{
  size_t bytes;
  int fd = 0;
  uint32_t *total = 0;
  bamipc_event_type event = 0;
  events_context ectx;
  bamipc_message *msg;
  struct adaptor *adaptor = 0;

  RUNTIME_ASSERT(bs->rec);

  switch(stream) {
    case 1:
      fd = bs->stdout_pipe[0];
      adaptor = &bs->stdout_adaptor;
      total = &bs->rec->stdout_total;
      event = BAMIPC_EVENT_FORMULA_STDOUT_BIN;
      break;
    case 2:
      fd = bs->stderr_pipe[0];
      adaptor = &bs->stderr_adaptor;
      total = &bs->rec->stderr_total;
      event = BAMIPC_EVENT_FORMULA_STDERR_BIN;
      break;
    case 1001:
      fd = bs->auxout_pipe[0];
      adaptor = &bs->auxout_adaptor;
      total = &bs->rec->auxout_total;
      event = BAMIPC_EVENT_FORMULA_AUXOUT_BIN;
      break;
  }

  /* potentially overwrite the entire ring buffer, but not more than
   * can fit in a single ipc message */
  bytes = adaptor_overwrite_read(adaptor, fd, BAMIPC_PAGESIZE);

  /* only report nonzero reads */
  if(!bytes) {
    return bytes;
  }

  *total += bytes;

  if(!(msg = events_binary_would(&ectx, event))) {
    return bytes;
  }

  msg->id = bs->rec->pid;
  msg->len = adaptor_last(adaptor, bytes, msg->text, msg->size);
  events_publish(&ectx);

  return bytes;
}

void build_slot_reap(build_slot * restrict bs)
{
  fsent *n;
  bool success;
  vertices targets, *eln;
  moria_vertex *v;
  graph_invalidation_context invalidation = { };

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &bs->rec->waited_time);

  success = true;
  if(bs->rec->status != 0 || bs->rec->stderr_total > 0) {
    success = false;
  }

  graph_invalidation_begin(&invalidation);

  /* the targets of this slot should now be updated */
  buildplan_entity_targets(bs->bpe, &targets);
  vertices_foreach(&targets, v, eln) {
    n = containerof(v, typeof(*n), vertex);
    n->in_epoch = notify_thread_epoch;
    n->notify_state = NOTIFY_EXPIRING;
    if(success) {
      n->hash = 0;
      walker_poke(n, 0, 0, &invalidation);
    }
  }
  vertices_destroy(&targets);

  graph_invalidation_end(&invalidation);

  if(success) {
    build_event_slot_ok(bs);
  } else {
    build_event_slot_err(bs);
  }

  build_slot_reap_binary(bs);
  build_slot_reap_bacon(bs);
}
