/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include "common/hash.h"
#include "lorien/nftwat.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xmman.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "moria/foreach.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"

#include "walker.internal.h"
#include "filesystem.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "edge.h"
#include "params.h"
#include "stats.h"
#include "config.h"
#include "configuration.h"
#include "pattern.h"
#include "generate.h"
#include "match.h"
#include "reconcile.h"
#include "entlist.h"
#include "system_state.h"
#include "channel.h"
#include "events.h"
#include "entlist.h"

#include "zcat.h"

llist walker_include_entry_list = LLIST_INITIALIZER(walker_include_entry_list);   // active list
static llist include_entry_freelist = LLIST_INITIALIZER(include_entry_freelist);  // freelist

/* exclude pattern list */
rbtree * walker_exclude_list;
rbtree * walker_special_list;

static uint16_t walk_ids;
static entlist generate_hits;

//
// static
//

static void stathash(fsent * restrict n, char * restrict path, uint16_t path_len, uint32_t * restrict hash)
{
  struct stat stb;
  int r;
  char c;

  STATS_INC(g_stats.stathash);

  *hash += 0xbeef;

  c = path[path_len];
  path[path_len] = 0;
  r = uxfstatats(g_base_dirfd, 0, &stb, path);
  if(r == 0)
  {
    /* accumulate specific fields we care about - surely this can be optimized alot */
    *hash = hash32(*hash, &stb.st_dev, sizeof(stb.st_dev));
    *hash = hash32(*hash, &stb.st_ino, sizeof(stb.st_ino));
    *hash = hash32(*hash, &stb.st_mode, sizeof(stb.st_mode));
    *hash = hash32(*hash, &stb.st_nlink, sizeof(stb.st_nlink));
    *hash = hash32(*hash, &stb.st_uid, sizeof(stb.st_uid));
    *hash = hash32(*hash, &stb.st_gid, sizeof(stb.st_gid));
    *hash = hash32(*hash, &stb.st_size, sizeof(stb.st_size));
    *hash = hash32(*hash, &stb.st_mtim, sizeof(stb.st_mtim));
    *hash = hash32(*hash, &stb.st_ctim, sizeof(stb.st_ctim));
  }

  path[path_len] = c;
}

static void contenthash(fsent * restrict n, char * restrict path, uint16_t path_len, uint32_t * restrict hash)
{
  int fd = -1;
  off_t end = 0;
  void *buf = MAP_FAILED;
  char c;

  STATS_INC(g_stats.contenthash);

  *hash += 0xface;

  c = path[path_len];
  path[path_len] = 0;
  fd = xopenats(O_RDONLY, g_base_dirfd, path);
  if(fd != -1)
  {
    end = xlseek(fd, 0, SEEK_END);
    if(end > 0)
    {
      buf = xmmap(0, end, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
      *hash = hash32(*hash, buf, end);
    }
  }

  ixmunmap(&buf, end);
  xclose(fd);
  path[path_len] = c;
}

static void poke(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  uint32_t content_hash;
  const filesystem *fs;
  moria_vertex *v;
  moria_edge *e;
  moria_foreach_state st;
  vertex_state prev;

  v = &n->vertex;
  fs = fsent_filesystem_get(n);

  prev = vertex_state_get(&n->vertex);
  vertex_exists_set(&n->vertex, true);

  /* special case for the transition from UNCREATED -> OK for watched files */
  if(prev == VERTEX_UNCREATED) {
    if(fs->invalidate == INVALIDATE_NOTIFY) {
      goto invalidate;
    }
  }

  /* skip nodes that are already invalid */
  if(vertex_invalid_get(&n->vertex)) {
    return;
  }

  /* skip non-leaf nodes */
  moria_foreach_edge_down(&st, v, e) {
    switch(edge_type_get(e)) {
      case EDGE_RULES_SPECIAL:
      case EDGE_MODULE_SPECIAL:
      case EDGE_VAR_SPECIAL:
      case EDGE_CONFIG_SPECIAL:
      case EDGE_FML_SPECIAL:
        continue;
      default:
        return;
    }
  }

  /* honor filesystem setting */
  switch(fs->invalidate) {
    case INVALIDATE_ALWAYS:
      goto invalidate;
    case INVALIDATE_NEVER:
    case INVALIDATE_NOTIFY:
      return;
    case INVALIDATE_STAT:
    case INVALIDATE_CONTENT:
  }

  content_hash = 0;
  if(fs->invalidate == INVALIDATE_STAT)
  {
    stathash(n, path, path_len, &content_hash);
  }
  else if(fs->invalidate == INVALIDATE_CONTENT)
  {
    contenthash(n, path, path_len, &content_hash);
  }

  if(content_hash != n->content_hash)
  {
    n->content_hash = content_hash;
    goto invalidate;
  }
  return;

invalidate:
  vertex_invalidate(&n->vertex, invalidation);
}

/*
 * reconcile an fsent against the file system ; generally this is handled by walker_descend, so this
 * is for fsents which potentially lie outside of those descended trees
 */
void walker_query(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  const filesystem *fs;
  const fsent *parent;

  if(!(parent = fsent_parent(n))) {
    RTA;
    return;
  }
  if(fsent_is_mounted(parent)) {
    return; /* already being handled thru the directory */
  }

  if(uxeuidaccesss(path, F_OK) != 0) {
    fsedge_disintegrate(containerof(n->vertex.up_identity, fsedge, edge), invalidation);
    return;
  }

  poke(n, path, path_len, invalidation);

  fs = fsent_filesystem_get(n);
  if(fs->invalidate != INVALIDATE_NOTIFY) {
    notify_thread_rm_watch(n);
    return;
  }

  notify_thread_add_watch(n);
}

static void include_entry_list_append(fsent * n)
{
  walker_entry *e;

  if((e = llist_shift(&include_entry_freelist, typeof(*e), lln)) == 0) {
    xmalloc(&e, sizeof(*e));
    llist_init(&e->lln);
  }

  e->n = n;
  e->abspathl = fsent_absolute_path_cat(e->abspath, sizeof(e->abspath), n);

  llist_append(&walker_include_entry_list, &e->lln);
}

static bool exclude_list_evaluate(fsent * restrict n)
{
  bool matched;
  const pattern *pat;

  if(!walker_exclude_list) {
    return false;
  }

  rbtree_foreach(walker_exclude_list, pat, rbn) {
    pattern_match(pat, n, &matched);
    if(matched) {
      fsent_prune_candidate(n);
      return true;
    }
  }

  return false;
}

void walker_visit(int method, ftwinfo * info, void * arg, int * stop)
{
  walker_context * ctx = arg;
  const filesystem * fs = 0;
  moria_vertex * lv;
  fsent * n = 0;
  fsent * parent;
  moria_edge *e;
  vertex_filetype filetype;
  fsent * Bn;
  moria_foreach_state st;
  entlist edges;
  vertex_state state;

  if(!info->parent)
  {
    /* this is the top-level directory */
    if(method == FTWAT_PRE)
    {
      n = ctx->base;
    }
    else
    {
      n = info->udata;
    }
  }
  else if(info->udata)
  {
    n = info->udata;
  }
  else
  {
    parent = info->parent->udata;

    if((lv = moria_vertex_downs(&parent->vertex, info->path + info->name_off)))
    {
      n = containerof(lv, fsent, vertex);
    }
    else
    {
      filetype = 0;
      state = VERTEX_OK;
      if(info->type == FTWAT_D) {
        filetype = VERTEX_FILETYPE_DIR;
        state = VERTEX_OK;
      }
      fsent_create(&n, filetype, state, info->path + info->name_off, info->pathl - info->name_off);
      fsedge_connect(n, parent, ctx->invalidation);
      vertex_alloc_probe(&n->vertex);
    }
  }

  info->udata = n;

  if(info->type == FTWAT_D && method == FTWAT_PRE) {
    if(exclude_list_evaluate(n)) {
      *stop = 1;
      return;
    }
  }

  // PRE (for directories), and 0 (for files)
  if(method != FTWAT_POST) {
    /* already been visited */
    if(n->descend_walk_id == ctx->walk_id) {
      *stop = 1;
      return;
    }

    n->descend_walk_id = ctx->walk_id;
  }

  fs = fsent_filesystem_get(n);
  if(info->type == FTWAT_D && method == FTWAT_PRE)
  {
    // stop the traversal on a leaf inotify dir which has already been explored
    if(fs->invalidate == INVALIDATE_NOTIFY)
    {
      /* important that the watch happens in the PREORDER step */
      notify_thread_add_watch(n);
    }
  }

  if(info->type == FTWAT_D && method == FTWAT_POST)
  {
    if(fs->invalidate != INVALIDATE_NOTIFY)
    {
      /* disintegrate edges which were not visited during this walk */
      entlist_init(&edges);
      moria_foreach_edge_down(&st, &n->vertex, e) {
        if(edge_type_get(e) != EDGE_FSTREE) {
          continue;
        }

        Bn = containerof(e->B, fsent, vertex);
        if(Bn->descend_walk_id == ctx->walk_id) {
          continue;
        }

        entlist_push(&edges, e);
      }

      fsedge_disintegrate_bulk(&edges, ctx->invalidation);
      entlist_destroy(&edges);
    }
  }
  else if(info->type == FTWAT_F)
  {
    poke(n, info->path, info->pathl, ctx->invalidation);
  }
}

//
// public
//

void walker_descend(
    fsent * restrict base
  , const char * restrict abspath
  , uint16_t walk_id
  , graph_invalidation_context * restrict invalidation
)
{
  walker_context ctx = { };

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  ctx.walk_id = walk_id;
  ctx.invalidation = invalidation;
  ctx.base = base;

  nftwat(AT_FDCWD, abspath, walker_visit, 64, FTWAT_SYMLINKS | FTWAT_FOLLOW, &ctx);
}

static void ascend_special_query(
    fsent * restrict dirn
  , const char * restrict name
  , uint16_t name_len
  , walker_context * restrict ctx
  , char * restrict path
  , size_t path_size
  , uint16_t path_len
)
{
  moria_vertex *dirv;
  moria_vertex *lv;
  fsent *fse;
  size_t z;

  z = path_len;
  z += zcats(path + z, path_size - 1 - z, "/");
  z += zcatw(path + z, path_size - 1 - z, name, name_len);
  path[z] = 0;

  dirv = &dirn->vertex;
  if((lv = moria_vertex_downw(dirv, name, name_len))) {
    fse = containerof(lv, fsent, vertex);
  } else if(uxeuidaccesss(path, F_OK) == 0) {
    fsent_create(&fse, VERTEX_FILETYPE_REG, VERTEX_OK, name, name_len);
    fsedge_connect(fse, dirn, ctx->invalidation);
  } else {
    return;
  }

  walker_query(fse, path, z, ctx->invalidation);
}

void walker_ascend(fsent * restrict dirn, uint16_t walk_id, graph_invalidation_context * restrict invalidation)
{
  char abspath[PATH_MAX];
  uint16_t abspath_len;
  moria_vertex *dirv;
  walker_context ctx = { };
  configuration_walker_special_entry *wse;

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  ctx.invalidation = invalidation;

  abspath_len = fsent_absolute_path_cat(abspath, sizeof(abspath), dirn);
  dirv = &dirn->vertex;

  while(dirv)
  {
    dirn = containerof(dirv, fsent, vertex);
    if(dirn->ascend_walk_id == walk_id) {
      break;
    }

    /* check for special files */
    if(walker_special_list) {
      rbtree_foreach(walker_special_list, wse, rbn) {
        ascend_special_query(dirn, wse->s, wse->len, &ctx, abspath, sizeof(abspath), abspath_len);
      }
    }
    dirn->ascend_walk_id = walk_id;
    abspath_len -= (dirv->label_len + 1);
    dirv = moria_vertex_up(dirv);
  }
}

void walker_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  walker_entry *e;
  fsent *n;
  uint16_t walk_id;

  walk_id = ++walk_ids;

  /* up and down from cwd */
  walker_descend(g_base, g_base_path, walk_id, invalidation);
  walker_ascend(fsent_parent(g_base), walk_id, invalidation);

  /* global config nodes */
  walker_query(system_config_node, system_config_node->self_config->self_node_abspath, system_config_node->self_config->self_node_abspath_len, invalidation);
  walker_query(user_config_node, user_config_node->self_config->self_node_abspath, user_config_node->self_config->self_node_abspath_len, invalidation);

  /* other configured trees */
  llist_foreach(&walker_include_entry_list, e, lln) {
    n = e->n;
    walker_descend(n, e->abspath, walk_id, invalidation);
    walker_ascend(fsent_parent(n), walk_id, invalidation);
  }
}

void walker_system_reconfigure(configuration * restrict cfg, bool dryrun)
{
  graph_invalidation_context invalidation = { };
  fsent *n;
  pattern *pat;
  walker_entry *e;
  entlist *eln;

  if(dryrun) {
    return;
  }

  /* store the exclude list for later evaluation */
  walker_exclude_list = &cfg->walker.exclude.set;
  walker_special_list = &cfg->walker.special.set;

  /* process the include list up-front */
  graph_invalidation_begin(&invalidation);

  /* fsent that was previously included, but may no longer be */
  llist_foreach(&walker_include_entry_list, e, lln) {
    fsent_prune_candidate(e->n);
  }

  llist_splice_head(&include_entry_freelist, &walker_include_entry_list);

  /* these patterns are absolute paths to directory nodes
   * the dir nodes are in the uncreated state
   * pattern_generate will leave the final node as an fsent instead of a dir */
  entlist_destroy(&generate_hits);
  rbtree_foreach(&cfg->walker.include.set, pat, rbn) {
    pattern_generate(pat, 0, 0, &invalidation, 0, &generate_hits, 0);
  }

  entlist_foreach(&generate_hits, n, eln) {
    /* exclude-match takes precedence over include */
    if(exclude_list_evaluate(n)) {
      continue;
    }

    fsent_promote_dir(n);
    include_entry_list_append(n);
  }

  graph_invalidation_end(&invalidation);
}

void walker_setup()
{
  entlist_init(&generate_hits);
}

void walker_cleanup()
{
  entlist_destroy(&generate_hits);
}

uint16_t walker_begin()
{
  return ++walk_ids;
}

static void filesystem_describe_node(bacon_writer * restrict builder_writer, const fsent * restrict fse)
{
  const walker_entry *entry;
  const struct moria_edge *edge;
  moria_foreach_state st;
  const fsent *child;
  char path[PATH_MAX];
  size_t len;

  len = fsent_absolute_path_cat(path, sizeof(path), fse);

  /* base case */
  if(fse == g_base) {
    goto ent;
  }
  if(fse == system_config_node) {
    goto ent;
  }
  if(fse == user_config_node) {
    goto ent;
  }
  llist_foreach(&walker_include_entry_list, entry, lln) {
    if(fse == entry->n) {
      goto ent;
    }
  }
  goto dir;
ent:
  BYTES(path, len)
  return;

dir:
  { MAPPING
    BYTES(path, len)
    { LIST
      moria_foreach_edge_down(&st, &fse->vertex, edge) {
        if(edge_type_get(edge) != EDGE_FSTREE) {
          continue;
        }

        child = containerof(edge->B, typeof(*child), vertex);
        filesystem_describe_node(builder_writer, child);
      }
    }
  }
}

void filesystem_describe(bacon_writer * restrict writer)
{
  filesystem_describe_node(writer, g_root);
}

void filesystem_show()
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  filesystem_describe(builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}
