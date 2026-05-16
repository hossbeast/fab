/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/vertex.h"
#include "moria/foreach.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "bacon/value.h"
#include "common/attrs.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "moria/connect.h"

#include "fstree.h"
#include "filesystem.h"
#include "graph.h"
#include "disintegrate.h"
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
#include "events/events.h"
#include "system.h"
#include "edges.h"
#include "vertices.h"
#include "namespace.h"
#include "ns_edge.h"
#include "edge.h"
#include "identity.h"

#include "zcat.h"

fstree *fstree_root;
fstree *fstree_shadow;

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

attrs16 *readlink_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s, v) + 1
    num : 0 READLINK_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : UINT16_C(x), range : UINT16_C(0xff) },
READLINK_TYPE_TABLE
  }
}};

llist fstree_list = LLIST_INITIALIZER(fstree_list);   // active
static llist fstree_node_freelist = LLIST_INITIALIZER(fstree_node_freelist);
static llist fstree_freelist = LLIST_INITIALIZER(fstree_freelist);

static void __attribute__((constructor)) init()
{
  attrs16_init(invalidation_attrs);
}

static fstree *fstree_alloc(fsent *base)
{
  fstree *fst;
  identity_ctx id;

  if((fst = llist_shift(&fstree_freelist, typeof(*fst), vertex.owner))) {
    ASAN_CLEAN(fst);
    memset(fst, 0, sizeof(*fst));
  } else {
    xmalloc(&fst, sizeof(*fst));
  }

  base->fst = fst;
  fst->base = base;

  fst->base_path_len = fsent_absolute_path_cat(fst->base_path, sizeof(fst->base_path), fst->base);
  identity_init(&id);
  fstree_compute_id(fst, &id);
  fst->id_len = identity_mk(&id, fst->id, sizeof(fst->id));

  moria_vertex_initw(&fst->vertex, &g_graph, VERTEX_FSTREE | VERTEX_OK, fst->id, fst->id_len);

  llist_append(&fstree_list, &fst->vertex.owner);
  vertex_alloc_probe(&fst->vertex);

  return fst;
}

static fstree *fstree_put(fsent *base)
{
  fstree *fst;

  fst = base->fst;
  if(!fst || fst->base != base) {
    fst = fstree_alloc(base);
    shadow_mkref(g_shadow_fstrees, fst->id, fst->id_len, &fst->vertex);
  }

  fst = base->fst;
  fst->mount = false;
  fst->mkdir = g_args.default_filesystem_mkdir;
  fst->readlink = g_args.default_filesystem_readlink;
  fst->invalidation = g_args.default_filesystem_invalidation;
  base->fstree_epoch = fsent_fstree_epoch;

  return fst;
}

static void fstree_dispose(fstree * restrict fst)
{
  llist_delete(&fst->vertex.owner);
  llist_append(&fstree_freelist, &fst->vertex.owner);
  ASAN_TAINT(fst);
}

void fstree_unlink(fstree * restrict fst, struct graph_disintegrate_context * restrict ctx)
{
  if(fst->base) {
    fst->base->fst = fstree_root;
  }
}

void fstree_release(fstree * restrict fst)
{
  moria_vertex *v;

  v = &fst->vertex;

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  fstree_dispose(fst);
}

static void namespace_enoent_error(const config_fstree_value *fse, const bacon_value * restrict val)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;
  const bacon_location *loc;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);
  loc = val->loc;

  { MAPPING
    STRING("ENOENT")
    { SET
      { MAPPING
        STRING("namespace")
        BYTES(val->s, val->len)
      }
      { MAPPING
        STRING("location")
        LOADF("%s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
      }
    }
  }
  bacon_writer_close(&writer);

  msg->len = fixed.l;
  msg->code = -1;
  events_publish(&ectx);
}

static void base_not_mounted_error(const config_fstree_value *fse)
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
    STRING("base fstree must be mounted")
    STRING("_")
  }
  bacon_writer_close(&writer);

  msg->len = fixed.l;
  msg->code = -1;
  events_publish(&ectx);
}

void fstree_root_setup()
{
  fstree *fst;

  /* root */
  fstree_root = fstree_alloc(g_root);

  fst = fstree_root; fst->invalidation = g_args.default_filesystem_invalidation;
  fst->mkdir = g_args.default_filesystem_mkdir;
  fst->unlink = false;
  fst->readlink = g_args.default_filesystem_readlink;
  fst->mount = false;

  /* shadow */
  fstree_shadow = fstree_alloc(g_shadow);

  fst = fstree_shadow;
  fst->invalidation = INVALIDATION_NEVER;
  fst->mkdir = false;
  fst->unlink = false;
  fst->readlink = false;
  fst->mount = false;
}

void fstree_setup()
{
  shadow_mkref(g_shadow_fstrees, fstree_root->id, fstree_root->id_len, &fstree_root->vertex);
  shadow_mkref(g_shadow_fstrees, fstree_shadow->id, fstree_shadow->id_len, &fstree_shadow->vertex);
}

void fstree_cleanup()
{
  fstree *fst;

  while((fst = llist_shift(&fstree_list, typeof(*fst), vertex.owner))) {
    graph_demolish(&fst->vertex);
  }

  while((fst = llist_shift(&fstree_freelist, typeof(*fst), vertex.owner))) {
    ASAN_CLEAN(fst);
    free(fst);
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

static void include_hit(void *ctx, const moria_vertex *v)
{
  struct vertices *hits = ctx;
  vertices_push(hits, v);
}

void fstree_reconfigure(config * restrict cfg, bool dryrun)
{
  fstree *fst = 0;
  moria_edge *e;
  config_fstree_value *fse;
  config_fstree_mapping *fsm;
  const include_pattern *pat;
  char path[PATH_MAX];
  llist *T;
  narrator_fixed fixed;
  narrator *N;
  graph_invalidation_context invalidation = { };
  struct vertices hits, *eln;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  fsent *n;
  moria_vertex *vertex;
  bacon_value *v;
  namespace *fsi;
  ns_edge *nse;
  moria_foreach_state st;

  /* check for invalid config */
  if(dryrun) {
    rbtree_foreach(&cfg->filesystem.fstrees.set, fsm, rbn) {
      /* check that the base dir is mounted */
      fse = &fsm->val;
      pat = fsm->key.v;
      N = narrator_fixed_init(&fixed, path, sizeof(path));
      pattern_say(&pat->pattern, N);

      if(memncmp(fixed.s, fixed.l, g_basedir, g_basedir_len) == 0) {
        if(fse->mount.v) {
          if(!fse->mount.b) {
            base_not_mounted_error(fse);
            return;
          }
        }
      }
    }

    return;
  }

  /* rebuild fstrees */
  fsent_fstree_epoch++;
  fstree_root->base->fstree_epoch = fsent_fstree_epoch;
  fstree_shadow->base->fstree_epoch = fsent_fstree_epoch;

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
    vertices_init(&hits);
    graph_invalidation_begin(&invalidation);
    pattern_include_generate(fsm->key.v, 0, 0, 0, 0, &invalidation, 0, 0, include_hit, &hits);
    vertices_foreach(&hits, vertex, eln) {
      n = containerof(vertex, typeof(*n), vertex);
      fst = fstree_put(n);
      if(fse->mkdir.v) {
        fst->mkdir = fse->mkdir.b;
      }
      if(fse->unlink.v) {
        fst->unlink = fse->unlink.b;
      }
      if(fse->readlink.v) {
        fst->readlink = fse->readlink.e;
      }
      if(fse->invalidation.v) {
        fst->invalidation = fse->invalidation.e;
      }
      if(fse->mount.v) {
        fst->mount = fse->mount.b;
      }

      if(fse->namespaces.list) {
        llist_foreach(&fse->namespaces.list->list, v, lln) {
          if((fsi = namespace_lookup(v->s, v->len)) == NULL) {
            namespace_enoent_error(fse, v);
            graph_invalidation_end(&invalidation);
            vertices_destroy(&hits);
            return;
          }
          nse = ns_connect(&fst->vertex, &fsi->vertex, EDGE_FSTREE_NS);
          nse->epoch = fsent_fstree_epoch;
        }
      }
    }
    vertices_destroy(&hits);
    graph_invalidation_end(&invalidation);
  }

  /* cleanup any fstree not re-freshed */
  llist_foreach_safe(&fstree_list, fst, vertex.owner, T) {
    if(fst->base->fstree_epoch != fsent_fstree_epoch) {
      graph_demolish(&fst->vertex);
    }
  }

  /* cleanup namespace associations not refreshed */
  edges_init(&edges);
  llist_foreach(&fstree_list, fst, vertex.owner) {
    moria_foreach_edge_up(&st, &fst->vertex, e) {
      if(edge_type_get(e) != EDGE_FSTREE_NS) {
        continue;
      }
      nse = containerof(e, typeof(*nse), edge);
      if(nse->epoch == fsent_fstree_epoch) {
        continue;
      }
      edges_push(&edges, e);
    }
  }

  vertices_init(&unlinking);
  vertices_init(&releasing);
  graph_invalidation_begin(&invalidation);
  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
  graph_invalidation_end(&invalidation);
}

void fstree_stats_build(fstree * restrict fst, bool reset, struct bacon_writer * restrict builder_writer)
{
  // fst->stats not relevant for fstree
  stats_build(&fst->fstree_stats.record, &descriptor_stats_fstree, builder_writer);
}

void fstree_describe(const fstree *fst, bacon_writer * restrict builder_writer)
{
  edges list, *eln;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&fst->vertex, builder_writer);
    { SET
      vertex_mappings(&fst->vertex, builder_writer);
      { MAPPING STRING("mount")
        BOOL(fst->mount)
      }
      { MAPPING STRING("invalidation")
        STRING(attrs16_name_byvalue(invalidation_attrs, fst->invalidation));
      }
      { MAPPING STRING("mkdir")
        BOOL(fst->mkdir)
      }
      { MAPPING STRING("unlink")
        BOOL(fst->unlink)
      }
      { MAPPING STRING("readlink")
        STRING(attrs16_name_byvalue(readlink_attrs, fst->readlink));
      }
      if(edges_gather_up(&fst->vertex, EDGE_FSTREE_NS, &list)) {
        MAPPING STRING("namespaces")
        { SET
          edges_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        edges_destroy(&list);
      }
      { MAPPING STRING("base")
        BYTES(fst->base_path, fst->base_path_len)
      }
      { MAPPING STRING("stats")
        { SET
          fstree_stats_build((struct fstree *)fst, false, builder_writer);
        }
      }

      vertex_relations_describe(&fst->vertex, builder_writer);
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
  return zcatf(dst, sz, "//bm/fstrees/%.*s", fst->id_len, fst->id);
}

void fstree_identity_describe(const struct fstree * restrict fst, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(fst->vertex.attrs, graph_vertex_attrs, builder_writer);
  { MAPPING
    STRING("id")
    BYTES(fst->id, fst->id_len);
  }
  { MAPPING
    STRING("mount")
    BOOL(fst->mount)
  }
  { MAPPING
    STRING("path")
    STRING(fst->base_path);
  }
}

void fstree_id(const struct fstree * restrict fstree, const char ** restrict id, uint16_t * restrict idl)
{
  *id = fstree->id;
  *idl = fstree->id_len;
}

void fstree_compute_id(const struct fstree * restrict fst, identity_ctx * restrict id)
{
  identity_type(id, VERTEX_FSTREE);
  identity_add(id, " ", 1);
  identity_add(id, fst->base_path, fst->base_path_len);
}

#if CONFIG_DEVEL
void fstrees_show()
{
  fstree *fst;
  int x = 0;

  printf("fstrees, live : %zu free : %zu\n", llist_count(&fstree_list), llist_count(&fstree_freelist));
  llist_foreach(&fstree_list, fst, vertex.owner) {
    dprintf(1, "%3u ", x++);
    vertex_identity_show(&fst->vertex);
    if(x == 1) {
      vertex_edges_show(&fst->vertex);
    }
  }
}
#endif
