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
#include "configset.h"
#include "config.h"
#include "pattern.h"
#include "generate.h"
#include "match.h"
#include "reconcile.h"
#include "entlist.h"
#include "system.h"
#include "channel.h"
#include "events.h"
#include "entlist.h"
#include "config_system.h"
#include "notify.h"

#include "zcat.h"

typedef struct walker_entry {
  struct fsent *n;
  char abspath[PATH_MAX];
  uint16_t abspathl;

  llist lln;
} walker_entry;

llist walker_include_entry_list = LLIST_INITIALIZER(walker_include_entry_list);   // active list
static llist include_entry_freelist = LLIST_INITIALIZER(include_entry_freelist);  // freelist

/* exclude pattern list */
rbtree *walker_exclude_list;
rbtree *walker_special_list;

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
  fstree * fst;

  fst = vertex_fstree_get(&n->vertex);
  filesystem_stats.stathash++;
  fst->fstree_stats.stathash++;

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
  fstree * fst;

  fst = vertex_fstree_get(&n->vertex);
  filesystem_stats.contenthash++;
  fst->fstree_stats.contenthash++;

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

void walker_poke(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  uint32_t prev;
  const fstree *fst;
  char stor[PATH_MAX];

  fst = vertex_fstree_get(&n->vertex);
  vertex_exists_set(&n->vertex, true);

  /* honor filesystem setting */
  switch(fst->invalidation) {
    case INVALIDATION_NOTIFY:
      if(n->hash == 0) {
        n->hash = 1;
        vertex_mark_fresh(&n->vertex);
      }
      return;
    case INVALIDATION_NEVER:
      vertex_mark_fresh(&n->vertex);
      return;
    case INVALIDATION_ALWAYS:
      vertex_invalidate(&n->vertex, invalidation);
      return;
    case INVALIDATION_STAT:
    case INVALIDATION_CONTENT:
      if((n->vertex.attrs & VERTEX_INVALID_BIT) && n->hash != 0) {
        return;
      }
      if(!path) {
        path_len = fsent_absolute_path_cat(stor, sizeof(stor), n);
        path = stor;
      }
  }

  prev = n->hash;
  n->hash = 0xbeef;
  if(fst->invalidation == INVALIDATION_STAT) {
    stathash(n, path, path_len, &n->hash);
  } else if(fst->invalidation == INVALIDATION_CONTENT) {
    contenthash(n, path, path_len, &n->hash);
  }

  if(prev == 0) {
    vertex_mark_fresh(&n->vertex);
  } else if(n->hash != prev) {
    vertex_invalidate(&n->vertex, invalidation);
  }
}

/*
 * reconcile an fsent against the file system ; generally this is handled by walker_descend, so this
 * is for fsents which potentially lie outside of the descended fstrees
 */
void walker_query(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  const fstree *fst;
  const fsent *parent;

  if(!(parent = fsent_parent(n))) {
    RTA;
    return;
  }
  fst = parent->fst;
  if(fst->mount) {
    return; /* already being handled via traversal */
  }

  if(uxeuidaccesss(path, F_OK) != 0) {
    fsedge_disintegrate(containerof(n->vertex.up_identity, fsedge, edge), invalidation);
    return;
  }

  walker_poke(n, path, path_len, invalidation);

  if(fst->invalidation != INVALIDATION_NOTIFY) {
    notify_rm_watch(n);
    return;
  }

  /* when the file is outside of a mounted fstree, it can be watched directly */
  notify_add_watch(n);
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
  const fstree *fst = 0;
  moria_vertex * lv;
  fsent * n = 0;
  fsent * parent;
  moria_edge *e;
  vertex_filetype filetype;
  fsent * Bn;
  moria_foreach_state st;
  entlist edges;
  uint32_t attrs;

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
      attrs = VERTEX_UNCREATED;
      if(info->type == FTWAT_D) {
        filetype = VERTEX_FILETYPE_DIR;
        /* Issue with UNCREATED here is that this directory is *known* to exist -- uncreated would mean
         * that, fs permitting, mkdir will be called on this directory, which will fail */
        attrs = VERTEX_OK;
      }

      fsent_create(&n, filetype, attrs, info->path + info->name_off, info->pathl - info->name_off);
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

  fst = vertex_fstree_get(&n->vertex);
  if(info->type == FTWAT_D && method == FTWAT_PRE)
  {
    // stop the traversal on a leaf inotify dir which has already been explored
    if(fst->invalidation == INVALIDATION_NOTIFY)
    {
      /* important that the watch happens in the PREORDER step */
      notify_add_watch(n);
    }
  }

  if(info->type == FTWAT_D && method == FTWAT_POST)
  {
    if(fst->invalidation != INVALIDATION_NOTIFY)
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
    walker_poke(n, info->path, info->pathl, ctx->invalidation);
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

static void special_query(
    fsent * restrict dirn
  , const char * restrict name
  , uint16_t name_len
  , graph_invalidation_context * restrict invalidation
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
    fsedge_connect(fse, dirn, invalidation);
  } else {
    return;
  }

  walker_query(fse, path, z, invalidation);
  return;
}

static void config_query(fsent * restrict dirn, const char *name, uint16_t name_len, graph_invalidation_context * restrict invalidation)
{
  char abspath[PATH_MAX];
  uint16_t abspath_len;

  /* create or delete as needed */
  abspath_len = fsent_absolute_path_cat(abspath, sizeof(abspath), dirn);
  special_query(dirn, name, name_len, invalidation, abspath, sizeof(abspath), abspath_len);
}

void walker_ascend(fsent * restrict dirn, uint16_t walk_id, graph_invalidation_context * restrict invalidation)
{
  char abspath[PATH_MAX];
  uint16_t abspath_len;
  moria_vertex *dirv;
  config_special_entry *wse;

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  abspath_len = fsent_absolute_path_cat(abspath, sizeof(abspath), dirn);
  dirv = &dirn->vertex;

  while(dirv)
  {
    dirn = containerof(dirv, fsent, vertex);
    if(dirn->ascend_walk_id == walk_id) {
      break;
    }

    /* look for special files */
    if(walker_special_list) {
      rbtree_foreach(walker_special_list, wse, rbn) {
        special_query(dirn, wse->s, wse->len, invalidation, abspath, sizeof(abspath), abspath_len);
      }
    }

    /* query for special files from a previous special list */

    dirn->ascend_walk_id = walk_id;
    abspath_len -= (dirv->label_len + 1);
    dirv = moria_vertex_up(dirv);
  }
}

void walker_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  uint16_t walk_id;
  fstree *fst;

  walk_id = ++walk_ids;

  /* refresh all fstrees */
  llist_foreach(&fstree_list, fst, vertex.owner) {
    walker_ascend(fst->base, walk_id, invalidation);
    if(fst->mount) {
      walker_descend(fst->base, fst->base_path, walk_id, invalidation);
    }
  }

  /* global config nodes */
  config_query(system_config_dirnode, system_config_name, system_config_name_len, invalidation);
  config_query(user_config_dirnode, user_config_name, user_config_name_len, invalidation);
}

void walker_system_reconfigure(config * restrict cfg, bool dryrun)
{
  graph_invalidation_context invalidation = { };
  fsent *n;
  config_fstree_mapping *fse;
  walker_entry *e;
  entlist *eln;

  if(dryrun) {
    return;
  }

  /* store the exclude list for later evaluation */
  walker_exclude_list = &cfg->filesystem.exclude.set;
  walker_special_list = &cfg->filesystem.special.set;

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
  rbtree_foreach(&cfg->filesystem.fstrees.set, fse, rbn) {
    pattern_generate(fse->key.v, 0, 0, &invalidation, 0, &generate_hits, 0);
  }

  entlist_foreach(&generate_hits, n, eln) {
    /* skip descendants of base */
    if(fsent_is_self_or_descendant(g_base, n)) {
      continue;
    }
    /* skip ancestors of config */
    if(fsent_is_self_or_descendant(n, system_config_dirnode)) {
      continue;
    }
    /* skip ancestors of config */
    if(fsent_is_self_or_descendant(n, user_config_dirnode)) {
      continue;
    }

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
