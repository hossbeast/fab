/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/vertex.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "common/attrs.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "common/hash.h"
#include "moria/connect.h"

#include "fstree.h"
#include "filesystem.h"
#include "graph.h"
#include "fsedge.h"
#include "vertex.h"
#include "args.h"
#include "config.h"
#include "pattern.h"
#include "pattern/section.h"
#include "generate.h"
#include "shadow.h"
#include "vertex/probes.h"
#include "fsent.h"
#include "config_system.h"
#include "events.h"
#include "system.h"
#include "entlist.h"

#include "zcat.h"

fstree fstree_root;
fstree fstree_shadow;

attrs16 *invalidation_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, n, r, y) + 1
    num : 0 INVALIDATION_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, n, r, y) { name : n, value : UINT16_C(y), range : UINT16_C(r) },
INVALIDATION_TYPE_TABLE
  }
}};

llist fstree_list = LLIST_INITIALIZER(fstree_list);   // active
static llist fstree_node_freelist = LLIST_INITIALIZER(fstree_node_freelist);
static llist fstree_freelist = LLIST_INITIALIZER(fstree_freelist);

static void __attribute__((constructor)) init()
{
  attrs16_init(invalidation_attrs);
}

static void fstree_init(fstree *fst)
{
  uint64_t hash;

  fst->base_path_len = fsent_absolute_path_cat(fst->base_path, sizeof(fst->base_path), fst->base);
  hash = hash64(0xface, fst->base_path, fst->base_path_len);
  fst->id_len = zcatf(fst->id, sizeof(fst->id), "%016"PRIx64, hash);
}

static fstree *fstree_alloc(fsent *base)
{
  fstree *fst;

  if((fst = llist_shift(&fstree_freelist, typeof(*fst), vertex.owner)) == 0) {
    xmalloc(&fst, sizeof(*fst));
  } else {
    memset(fst, 0, sizeof(*fst));
  }

  moria_vertex_init(&fst->vertex, &g_graph, VERTEX_FSTREE | VERTEX_OK);

  fst->mkdir = g_args.default_filesystem_mkdir;
  fst->readlink = g_args.default_filesystem_readlink;
  fst->index = g_args.default_filesystem_index;
  fst->invalidation = g_args.default_filesystem_invalidation;
  fst->mount = false;

  fst->base = base;
  base->fst = fst;
  fstree_init(fst);

  llist_append(&fstree_list, &fst->vertex.owner);

  shadow_link(g_shadow_fstrees, fst->id, fst->id_len, &fst->vertex, &fst->link);

  return fst;
}

static fstree *fstree_put(fsent *base)
{
  if(base->fst == &fstree_root || base->fstree_epoch != fsent_fstree_epoch) {
    base->fstree_epoch = fsent_fstree_epoch;
    fstree_alloc(base);
  }

  return base->fst;
}

static void fstree_dispose(fstree * restrict fst)
{
  llist_delete(&fst->vertex.owner);
  llist_append(&fstree_freelist, &fst->vertex.owner);
}

static void fstree_release(fstree * restrict fst)
{
  moria_vertex __attribute__((unused)) *v;

  v = &fst->vertex;

  if(fst->link) {
    moria_edge_disconnect(&g_graph, &fst->link->edge);
    fsedge_release(fst->link);
  }

  if(fst->base) {
    fst->base->fst = &fstree_root;
  }

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  vertex_deleted_probe(&fst->vertex);
  fstree_dispose(fst);
}

static void base_not_mounted_error(const config_fstree_value *fse)
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
    STRING("base must be mounted")
    STRING("-")
  }
  bacon_writer_close(&writer);

  msg->size = fixed.l;
  msg->code = -1;
  events_publish(&ectx);
}

void fstree_setup()
{
  fstree *fst;

  fst = &fstree_root;
  moria_vertex_init(&fst->vertex, &g_graph, VERTEX_FSTREE | VERTEX_OK);
  fst->base = g_root;
  fstree_init(fst);

  fst->invalidation = g_args.default_filesystem_invalidation;
  fst->mkdir = g_args.default_filesystem_mkdir;
  fst->readlink = g_args.default_filesystem_readlink;
  fst->index = g_args.default_filesystem_index;
  fst->mount = false;

  shadow_link(g_shadow_fstrees, fst->id, fst->id_len, &fst->vertex, &fst->link);

  fst = &fstree_shadow;
  moria_vertex_init(&fst->vertex, &g_graph, VERTEX_FSTREE | VERTEX_OK);
  fst->base = g_shadow;
  fstree_init(fst);
  fst->invalidation = INVALIDATION_NEVER;
  fst->mkdir = false;
  fst->readlink = false;
  fst->index = false;
  fst->mount = false;

  shadow_link(g_shadow_fstrees, fst->id, fst->id_len, &fst->vertex, &fst->link);
}

void fstree_cleanup()
{
  fstree *fst;
  llist *lln;
  llist *T;

  llist_foreach_safe(&fstree_list, fst, vertex.owner, T) {
    fstree_dispose(fst);
  }

  llist_foreach_safe(&fstree_freelist, fst, vertex.owner, lln) {
    free(fst);
  }

  fst = &fstree_root;
  if(fst->link) {
    moria_edge_disconnect(&g_graph, &fst->link->edge);
    fsedge_release(fst->link);
  }

  fst = &fstree_shadow;
  if(fst->link) {
    moria_edge_disconnect(&g_graph, &fst->link->edge);
    fsedge_release(fst->link);
  }
}

bool fstree_in_ancestry(const fsent * restrict fse)
{
  const fstree *fst;

  llist_foreach(&fstree_list, fst, vertex.owner) {
    /* in the ancestry of any fstree ? */
    if(fsent_is_self_or_descendant(fse, fst->base)) {
      return true;
    }
  }

  return false;
}

void fstree_reconfigure(config * restrict cfg, bool dryrun)
{
  fstree *fst = 0;
  config_fstree_value *fse;
  config_fstree_mapping *fsm;
  const struct pattern *pat;
  char path[PATH_MAX];
  llist *T;
  narrator_fixed fixed;
  narrator *N;
  graph_invalidation_context invalidation = { };
  entlist hits, *eln;
  fsent *n;

  /* check for invalid config */
  if(dryrun) {
    rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
      fse = &fsm->val;
      pat = fsm->key.v;
      N = narrator_fixed_init(&fixed, path, sizeof(path));
      pattern_say(pat, N);

      if(memncmp(fixed.s, fixed.l, g_base_dir, g_base_dir_len) != 0) {
        continue;
      }
      if(!fse->mount.v) {
        continue;
      }
      if(fse->mount.b) {
        continue;
      }

      /* the base dir cannot be unmounted */
      base_not_mounted_error(fse);
      break;
    }

    return;
  }

  /* rebuild fstrees */
  fsent_fstree_epoch++;
  llist_foreach_safe(&fstree_list, fst, vertex.owner, T) {
    fstree_release(fst);
  }

  /* add an fstree for global config dirs */
  fst = fstree_put(user_config_dirnode);
  fst->mount = false;

  fst = fstree_put(system_config_dirnode);
  fst->mount = false;

  /* add a mounted fstree for the basedir */
  fst = fstree_put(g_base);
  fst->mount = true;

  /* append fstrees from config */
  rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
    fse = &fsm->val;
    entlist_init(&hits);
    graph_invalidation_begin(&invalidation);
    pattern_generate(fsm->key.v, 0, 0, &invalidation, 0, &hits, 0);
    entlist_foreach(&hits, n, eln) {
      fst = fstree_put(n);
      fst->mkdir = g_args.default_filesystem_mkdir;
      if(fse->mkdir.v) {
        fst->mkdir = fse->mkdir.b;
      }
      fst->readlink = g_args.default_filesystem_readlink;
      if(fse->readlink.v) {
        fst->readlink = fse->readlink.b;
      }
      fst->index = g_args.default_filesystem_index;
      if(fse->index.v) {
        fst->index = fse->index.b;
      }
      fst->invalidation = g_args.default_filesystem_invalidation;
      if(fse->invalidation.v) {
        fst->invalidation = fse->invalidation.e;
      }
      fst->mount = false;
      if(fse->mount.v) {
        fst->mount = fse->mount.b;
      }
    }
    entlist_destroy(&hits);
    graph_invalidation_end(&invalidation);
  }
}

void fstree_stats_build(fstree * restrict fst, bool reset, struct bacon_writer * restrict builder_writer)
{
  stats_build(&fst->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&fst->fstree_stats.record, &descriptor_stats_fstree, builder_writer);
}

void fstree_describe(const fstree *fst, bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fst->vertex, builder_writer);
    { SET
      { MAPPING STRING("address") HEX(fst) }
      { MAPPING STRING("attrs")
        attrs32_describe(fst->vertex.attrs, graph_vertex_attrs, builder_writer);
      }
      { MAPPING STRING("id")
        if(fst->id_len) {
          BYTES(fst->id, fst->id_len)
        } else {
          STRING("_")
        }
      }
      { MAPPING STRING("mount")
        BOOL(fst->mount)
      }
      { MAPPING STRING("invalidation")
        STRING(attrs16_name_byvalue(invalidation_attrs, fst->invalidation));
      }
      { MAPPING STRING("mkdir")
        BOOL(fst->mkdir)
      }
      { MAPPING STRING("readlink")
        BOOL(fst->readlink)
      }
      { MAPPING STRING("index")
        BOOL(fst->index)
      }
      { MAPPING STRING("base")
        BYTES(fst->base_path, fst->base_path_len)
      }
      { MAPPING STRING("stats")
        { SET
          fstree_stats_build((struct fstree *)fst, false, builder_writer);
        }
      }
    }
  }
}

void fstree_show(const fstree *fst)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  fstree_describe(fst, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

size_t fstree_path_cat(void * restrict dst, size_t sz, const fstree * restrict fst)
{
  return zcatf(dst, sz, "//fstree/%.*s", fst->id_len, fst->id);
}

void fstree_identity_say(const struct fstree * restrict fst, struct narrator * restrict N)
{
  attrs32_say(fst->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s mount %3s path %.*s", fst->id_len, fst->id, fst->mount ? "yes" : "no", fst->base_path_len, fst->base_path);
}
