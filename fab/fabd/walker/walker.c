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

#include "common/hash.h"
#include "lorien/nftwat.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xmman.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "yyutil/box.h"
#include "valyria/set.h"

#include "walker.internal.h"
#include "filesystem.h"
#include "fsedge.h"
#include "fsent.h"
#include "params.h"
#include "stats.h"
#include "config.internal.h"
#include "pattern.h"
#include "generate.h"
#include "match.h"
#include "reconcile.h"

#include "zbuffer.h"

#include "narrator.h"

static llist include_entry_list = LLIST_INITIALIZER(include_entry_list);         // active list
static llist include_entry_freelist = LLIST_INITIALIZER(include_entry_freelist); // freelist

/* exclude pattern list */
llist * walker_exclude_list;

typedef struct {
  fsent * n;
  char abspath[512];
  uint16_t abspathl;

  llist lln;
} entry;

static uint16_t walk_ids;
static set *generate_nodes;
static uint16_t reconciled;

//
// static
//

static void stathash(fsent * restrict n, char * restrict path, uint16_t pathl, uint32_t * restrict hash)
{
  struct stat stb;
  int r;
  char c;

  STATS_INC(g_stats.stathash);

  *hash += 0xbeef;

  c = path[pathl];
  path[pathl] = 0;
  r = uxfstatats(g_params.proj_dirfd, 0, &stb, path);
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

  path[pathl] = c;
}

static void contenthash(fsent * restrict n, char * restrict path, uint16_t pathl, uint32_t * restrict hash)
{
  int fd = -1;
  off_t end = 0;
  void *buf = MAP_FAILED;
  char c;

  STATS_INC(g_stats.contenthash);

  *hash += 0xface;

  c = path[pathl];
  path[pathl] = 0;
  fd = xopenats(O_RDONLY, g_params.proj_dirfd, path);
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
  path[pathl] = c;
}

static void refresh(walker_context * restrict ctx, fsent * restrict n, char * restrict path, uint16_t pathl)
{
  uint32_t hash;
  const filesystem *fs;
  moria_vertex *v;

  fsent_exists_set(n);

  fs = fsent_filesystem_get(n);
  if(fs->attrs == INVALIDATE_ALWAYS)
  {
    fsent_invalidate(n, ctx->invalidation);
    return;
  }
  else if(fs->attrs == INVALIDATE_NEVER)
  {
    return;
  }

  /* only hash/invalidate on primary files - the leaf nodes */
  v = &n->vertex;
  if(!rbtree_empty(&v->down)) {
    return;
  }

  hash = 0;
  if(fs->attrs == INVALIDATE_STAT)
  {
    stathash(n, path, pathl, &hash);
  }
  else if(fs->attrs == INVALIDATE_CONTENT)
  {
    contenthash(n, path, pathl, &hash);
  }

  if(hash != n->hash)
  {
    if(hash)
    {
      fsent_invalidate(n, ctx->invalidation);
    }

    n->hash = hash;
  }
}

static void include_entry_list_append(fsent * n)
{
  entry *e;

  if((e = llist_shift(&include_entry_freelist, typeof(*e), lln)) == 0) {
    xmalloc(&e, sizeof(*e));
    llist_init_node(&e->lln);
  }

  e->n = n;
  e->abspathl = fsent_absolute_path_znload(e->abspath, sizeof(e->abspath), n);

  llist_append(&include_entry_list, e, lln);
}

//
// internal
//

void walker_visit(int method, ftwinfo * info, void * arg, int * stop)
{
  walker_context * ctx = arg;
  const filesystem * fs = 0;
  moria_vertex * lv;
  fsent * n = 0;
  fsent * parent;
  llist edges;
  moria_vertex *v;
  moria_edge *e;
  vertex_filetype filetype;
  fsent * Bn;
  pattern *pat;
  bool matched;

  if(!info->parent)
  {
    /* this is the top-level directory */
    if(method == FTWAT_PRE && ctx->base)
    {
      n = ctx->base;
    }
    else if(method == FTWAT_PRE)
    {
      filetype = 0;
      if(info->type == FTWAT_D)
        filetype = VERTEX_FILETYPE_DIR;
      fsent_create(&ctx->base, filetype, VERTEX_OK, info->path + info->name_off, info->pathl - info->name_off);
      fsedge_connect(ctx->base_parent, ctx->base, ctx->invalidation);

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

    /* check whether this dirent matches any exclude pattern */
    if(info->type == FTWAT_D && walker_exclude_list)
    {
      llist_foreach(walker_exclude_list, pat, lln) {
        pattern_match(pat, parent, info->path + info->name_off, info->pathl - info->name_off, &matched);
        if(matched) {
          *stop = 1;
          return;
        }
      }
    }

    if((lv = moria_vertex_downs(&parent->vertex, info->path + info->name_off)))
    {
      n = containerof(lv, fsent, vertex);
    }
    else
    {
      filetype = 0;
      if(info->type == FTWAT_D) {
        filetype = VERTEX_FILETYPE_DIR;
      }
      fsent_create(&n, filetype, VERTEX_OK, info->path + info->name_off, info->pathl - info->name_off);
      fsedge_connect(parent, n, ctx->invalidation);
    }
  }

  info->udata = n;

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
    if(fs->attrs == INVALIDATE_NOTIFY)
    {
      if(n->wd == -1)
      {
        /* it's important that the watch happen in the PREORDER step */
        notify_thread_add_watch(n);
      }
      else if(n->descended)
      {
        *stop = 1;
      }

      n->descended = true;
    }
  }

  if(info->type == FTWAT_D && method == FTWAT_POST)
  {
    if(fs->attrs != INVALIDATE_NOTIFY)
    {
      // destroy edges connecting to vertices which were not visited by this walk
      llist_init_node(&edges);

      v = &n->vertex;
      rbtree_foreach(&v->down, e, rbn_down) {
        if((e->attrs & EDGE_TYPE_OPT) != EDGE_TYPE_FSTREE) {
          continue;
        }

        Bn = containerof(e->B, fsent, vertex);
        if(Bn->descend_walk_id == ctx->walk_id) {
          continue;
        }

        llist_prepend(&edges, e, lln);
      }

      while((e = llist_shift(&edges, typeof(*e), lln)))
      {
        fsedge_disintegrate(containerof(e, fsedge, edge), ctx->invalidation);
      }
    }
  }
  else if(info->type == FTWAT_F)
  {
    if(fs->invalidate != INVALIDATE_NOTIFY)
    {
      refresh(ctx, n, info->path, info->pathl);
    }
  }
}

//
// public
//

void walker_descend(
    fsent ** restrict basep
  , fsent * restrict base
  , fsent * restrict parent
  , const char * restrict abspath
  , uint16_t walk_id
  , graph_invalidation_context * restrict invalidation
)
{
  walker_context ctx;

  RUNTIME_ASSERT(!base ^ !parent);

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  ctx.walk_id = walk_id;
  ctx.invalidation = invalidation;
  ctx.base_parent = parent;
  ctx.base = base;

  // filesystem traversal from the root
  nftwat(AT_FDCWD, abspath, walker_visit, 64, &ctx);

  if(basep) {
    *basep = ctx.base;
  }
}

void walker_ascend(fsent * restrict base, uint16_t walk_id, graph_invalidation_context * restrict invalidation)
{
  char path[512];
  uint16_t pathl;
  fsent * dirn;
  moria_vertex * dirv;
  const filesystem *fs = 0;
  walker_context ctx = { };
  size_t z;
  fsent *n;
  moria_vertex *lv;

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  ctx.invalidation = invalidation;

  dirn = base;
  dirv = &dirn->vertex;

  pathl = fsent_absolute_path_znload(path, sizeof(path), dirn);

  // dont visit base itself, ascend at least once
  while(1)
  {
    pathl -= (dirv->label_len + 1);
    dirv = moria_vertex_up(dirv);
    dirn = containerof(dirv, fsent, vertex);
    if(!dirv || dirn->ascend_walk_id == walk_id) {
      break;
    }

    fs = fsent_filesystem_get(dirn);
    path[pathl] = 0;
    if(uxeuidaccesss(path, F_OK) == 0)
    {
      if(fs->attrs == INVALIDATE_NOTIFY)
      {
        if(dirn->wd == -1) {
          notify_thread_add_watch(dirn);
        }
      }

      refresh(&ctx, dirn, path, pathl);
    }
    else if(fs->attrs != INVALIDATE_NOTIFY)
    {
      /* disintegrate */
    }

    // visited
    dirn->ascend_walk_id = walk_id;

    /* check for var files in the directory */
    if((lv = moria_vertex_downw(dirv, MMS(fsent_name_var))))
    {
      n = containerof(lv, fsent, vertex);
      if(fs->attrs == INVALIDATE_NOTIFY)
      {
        z = 0;
        z += znloads(path + pathl + z, sizeof(path) - pathl - z, "/");
        z += znloadw(path + pathl + z, sizeof(path) - pathl - z, MMS(fsent_name_var));
        path[pathl + z] = 0;

        refresh(&ctx, n, path, pathl + z);
      }
      else if(fs->attrs != INVALIDATE_NOTIFY)
      {
        /* disintegrate */
      }
    }
    else
    {
      z = 0;
      z += znloads(path + pathl + z, sizeof(path) - pathl - z, "/");
      z += znloadw(path + pathl + z, sizeof(path) - pathl - z, MMS(fsent_name_var));
      path[pathl + z] = 0;

      if(uxeuidaccesss(path, F_OK) == 0)
      {
        fsent_create(&n, VERTEX_FILETYPE_REG, VERTEX_OK, MMS(fsent_name_var));
        fsedge_connect(dirn, n, invalidation);
      }
    }

    /* check for config files in the directory */
    if((lv = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      n = containerof(lv, fsent, vertex);
      if(fs->attrs == INVALIDATE_NOTIFY)
      {
        z = 0;
        z += znloads(path + pathl + z, sizeof(path) - pathl - z, "/");
        z += znloadw(path + pathl + z, sizeof(path) - pathl - z, MMS(fsent_name_config));
        path[pathl + z] = 0;

        refresh(&ctx, n, path, pathl + z);
      }
      else if(fs->attrs != INVALIDATE_NOTIFY)
      {
        /* disintegrate */
      }
    }
    else
    {
      z = 0;
      z += znloads(path + pathl + z, sizeof(path) - pathl - z, "/");
      z += znloadw(path + pathl + z, sizeof(path) - pathl - z, MMS(fsent_name_config));
      path[pathl + z] = 0;

      if(uxeuidaccesss(path, F_OK) == 0)
      {
        fsent_create(&n, VERTEX_FILETYPE_REG, VERTEX_OK, MMS(fsent_name_config));
        fsedge_connect(dirn, n, invalidation);
      }
    }

  }
}

void walker_system_reconcile(graph_invalidation_context * restrict invalidation)
{
  entry *e;
  fsent *n;
  uint16_t walk_id;

  /* already reconciled in this round */
  if(reconciled == reconciliation_id) {
    return;
  }

  walk_id = ++walk_ids;
  reconciled = reconciliation_id;

  /* up and down from the project node */
  walker_descend(0, g_project_root, 0, g_params.proj_dir, walk_id, invalidation);
  walker_ascend(g_project_root, walk_id, invalidation);

  /* global config nodes */
  walker_ascend(system_config_node, walk_id, invalidation);
  walker_ascend(user_config_node, walk_id, invalidation);

  /* other configured trees */
  llist_foreach(&include_entry_list, e, lln) {
    n = e->n;
    walker_descend(0, n, 0, e->abspath, walk_id, invalidation);
    walker_ascend(n, walk_id, invalidation);
  }
}

int walker_system_reconfigure(configblob * restrict cfg, char * restrict err, uint16_t err_sz)
{
  graph_invalidation_context invalidation = { };
  fsent *n;
  int x;
  pattern *pat;

  if(err == NULL) {
    return 0;
  }

  /* this flag controls whether the next reconcile actually does any work */
  if(cfg->walker.changed) {
    reconciled = 0;
  }

  /* re-load on any changes, because the exclude list from config is stored here */
  if(!cfg->changed) {
    return 0;
  }

  /* store the exclude list for later evaluation */
  walker_exclude_list = &cfg->walker.exclude.list;

  /* process the include list up-front */
  graph_invalidation_begin(&invalidation);

  llist_splice_head(&include_entry_freelist, &include_entry_list);

  /* these are all absolute paths */
  set_recycle(generate_nodes);
  llist_foreach(&cfg->walker.include.list, pat, lln) {
    pattern_generate(pat, 0, 0, 0, 0, &invalidation, 0, generate_nodes);
  }

  for(x = 0; x < generate_nodes->table_size; x++)
  {
    if(!(n = set_table_get(generate_nodes, x))) {
      continue;
    }

    include_entry_list_append(n);
  }

  graph_invalidation_end(&invalidation);

  return 0;
}

void walker_setup()
{
  set_create(&generate_nodes);
}

void walker_cleanup()
{
  set_xfree(generate_nodes);
}

uint16_t walker_begin()
{
  return ++walk_ids;
}
