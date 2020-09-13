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

#include <string.h>

#include "xapi.h"
#include "types.h"

#include "xlinux/xfcntl.h"
#include "xlinux/xinotify.h"
#include "xlinux/xmman.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "valyria/set.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "lorien/nftwat.h"
#include "valyria/list.h"
#include "valyria/map.h"

#include "walker.internal.h"
#include "filesystem.h"
#include "module.internal.h"
#include "node.h"
#include "node_operations.h"
#include "notify_thread.h"
#include "path.h"
#include "params.h"
#include "stats.h"

#include "macros.h"
#include "zbuffer.h"
#include "common/hash.h"
#include "common/attrs.h"

static int walk_ids = 1;

//
// static
//

static vertex_filetype fstype_ftwinfo(ftwinfo * info)
{
  if(info->type == FTWAT_D)
    return VERTEX_FILETYPE_DIR;

  if(info->type == FTWAT_F)
    return VERTEX_FILETYPE_REG;

  return 0;
}

static xapi stathash(node * restrict n, char * restrict path, uint16_t pathl, uint32_t * restrict hash)
{
  enter;

  struct stat stb;
  int r;
  char c;

  STATS_INC(stathash);

  *hash += 0xbeef;

  c = path[pathl];
  path[pathl] = 0;
  fatal(uxfstatats, &r, g_params.proj_dirfd, 0, &stb, path);
  if(r == 0)
  {
    /* accumulate specific fields we care about */
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

finally:
  path[pathl] = c;
coda;
}

static xapi contenthash(node * restrict n, char * restrict path, uint16_t pathl, uint32_t * restrict hash)
{
  enter;

  int fd = -1;
  off_t end;
  void *buf = MAP_FAILED;
  char c;

  STATS_INC(contenthash);

  *hash += 0xface;

  c = path[pathl];
  path[pathl] = 0;
  fatal(xopenats, &fd, O_RDONLY, g_params.proj_dirfd, path);
  if(fd != -1)
  {
    fatal(xlseek, fd, 0, SEEK_END, &end);
    if(end > 0)
    {
      fatal(xmmap, 0, end, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0, &buf);
      *hash = hash32(*hash, buf, end);
    }
  }

finally:
  fatal(ixmunmap, &buf, end);
  fatal(xclose, fd);
  path[pathl] = c;
coda;
}


static xapi refresh(walker_context * restrict ctx, node * restrict n, char * restrict path, uint16_t pathl)
{
  enter;

  uint32_t hash;
  const filesystem *fs;
  vertex *v;

  fs = node_filesystem_get(n);
  if(fs->attrs == INVALIDATE_ALWAYS)
  {
    fatal(node_invalidate, n, ctx->invalidation);
    goto XAPI_FINALIZE;
  }

  /* only hash/invalidate on primary files - those with no dependencies */
  v = vertex_containerof(n);
  if(!rbtree_empty(&v->down)) {
    goto XAPI_FINALIZE;
  }

  hash = 0;
  if(fs->attrs == INVALIDATE_STAT)
  {
    fatal(stathash, n, path, pathl, &hash);
  }
  else if(fs->attrs == INVALIDATE_CONTENT)
  {
    fatal(contenthash, n, path, pathl, &hash);
  }

  if(hash != n->hash)
  {
    if(hash)
    {
      fatal(node_invalidate, n, ctx->invalidation);
    }

    n->hash = hash;
  }

  finally : coda;
}

//
// internal
//

xapi walker_visit(int method, ftwinfo * info, void * arg, int * stop)
{
  enter;

  walker_context * ctx = arg;
  const filesystem * fs = 0;
  module * mod = 0;
  vertex * lv;
  node * n = 0;
  node * parent;
  llist edges;
  vertex *v;
  edge *e;

/* put this into config */
if((info->pathl - info->name_off) == 4 && memcmp(info->path + info->name_off, ".git", 4) == 0) {
  *stop = 1;
  goto XAPI_FINALIZE;
}

  /* if !info->parent, this is the top-level directory */
  if(!info->parent)
  {
    if(method == FTWAT_PRE && ctx->base)
    {
      n = ctx->base;
    }
    else if(method == FTWAT_PRE)
    {
      mod = node_module_get(ctx->base_parent);
      fs = node_filesystem_get(ctx->base_parent);

      fatal(node_creates, &ctx->base, fstype_ftwinfo(info) | VERTEX_OK, fs, mod, info->path + info->name_off);
      fatal(node_connect_fs, ctx->base_parent, ctx->base, EDGE_TYPE_FS, ctx->invalidation, 0, 0);

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
    lv = vertex_downs(
        vertex_containerof(parent)
      , info->path + info->name_off
    );
    if(lv)
    {
      n = vertex_value(lv);
    }
    else
    {
      if(info->type == FTWAT_D)
      {
        mod = node_module_get(parent);
        fs = node_filesystem_get(parent);
      }

      fatal(node_creates, &n, fstype_ftwinfo(info) | VERTEX_OK, fs, mod, info->path + info->name_off);
      fatal(node_connect_fs, parent, n, EDGE_TYPE_FS, ctx->invalidation, 0, 0);
    }
  }

  info->udata = n;

  // this node was visited
  if(method != FTWAT_POST) {
    n->walk_id = ctx->walk_id;
  }

  fs = node_filesystem_get(n);
  if(info->type == FTWAT_D && method == FTWAT_PRE)
  {
    // stop the traversal on a leaf inotify dir which has already been explored
    if(fs->attrs == INVALIDATE_NOTIFY)
    {
      if(n->wd == -1)
      {
        /* it's important that the watch happen in the PREORDER step */
        fatal(notify_thread_watch, n);
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

      v = vertex_containerof(n);
      rbtree_foreach(&v->down, e, rbn_down) {
        if(e->attrs != (MORIA_EDGE_IDENTITY | EDGE_TYPE_FS)) {
          continue;
        }

        vertex * B = e->B;
        node * Bn = vertex_value(B);
        if(Bn->walk_id == ctx->walk_id) {
          continue;
        }

        llist_prepend(&edges, e, lln);
      }

      while((e = llist_shift(&edges, typeof(*e), lln)))
      {
        fatal(node_disintegrate_fs, e, ctx->invalidation);
      }
    }
  }

  else if(info->type == FTWAT_F)
  {
    if(fs->invalidate != INVALIDATE_NOTIFY && fs->invalidate != INVALIDATE_NEVER)
    {
      fatal(refresh, ctx, n, info->path, info->pathl);
    }
  }

  finally : coda;
}

//
// public
//

xapi walker_descend(node ** restrict basep, node * restrict base, node * restrict parent, const char * restrict abspath, int walk_id, graph_invalidation_context * restrict invalidation)
{
  enter;

  walker_context ctx;

  RUNTIME_ASSERT(!base ^ !parent);

  if(walk_id != walk_ids)
    walk_id = ++walk_ids;

  ctx.walk_id = walk_id;
  ctx.invalidation = invalidation;
  ctx.base_parent = parent;
  ctx.base = base;

  // filesystem traversal from the root
  fatal(nftwat, AT_FDCWD, abspath, walker_visit, 64, &ctx);

  if(basep) {
    *basep = ctx.base;
  }

  finally : coda;
}

xapi walker_ascend(node * restrict basedir, int walk_id, graph_invalidation_context * restrict invalidation)
{
  enter;

  char path[512];
  uint16_t pathl;
  node * dirn;
  vertex * dirv;
  int r;
  const filesystem *fs = 0;
  module *mod = 0;
  walker_context ctx = { 0 };

  if(walk_id != walk_ids)
    walk_id = ++walk_ids;

  ctx.invalidation = invalidation;
  dirn = basedir;
  dirv = vertex_containerof(dirn);

  pathl = node_absolute_path_znload(path, sizeof(path), dirn);

  // ascend at least once
  while(1)
  {
    pathl -= (dirv->label_len + 1);
    dirv = vertex_up(dirv);
    dirn = vertex_value(dirv);
    if(!dirv || dirn->walk_id == walk_id)
      break;

    vertex * filev = vertex_downs(dirv, NODE_VAR_BAM);
    node * filen = vertex_value(filev);

    snprintf(path + pathl, sizeof(path) - pathl, "/" NODE_VAR_BAM);
    fatal(uxeuidaccesss, &r, F_OK, path);
    if(r == 0)
    {
      if(filev)
      {
        fatal(refresh, &ctx, filen, path, pathl + strlen("/" NODE_VAR_BAM));
      }
      else
      {
        fatal(node_creates, &filen, VERTEX_FILETYPE_REG | VERTEX_OK, fs, mod, NODE_VAR_BAM);
        fatal(node_connect_fs, dirn, filen, EDGE_TYPE_FS, invalidation, 0, 0);
      }
    }
    else if(filev)
    {
      fatal(node_disconnect, dirn, filen, invalidation);
    }

    if(dirn->fs->attrs == INVALIDATE_NOTIFY)
    {
      if(dirn->wd == -1)
        fatal(notify_thread_watch, dirn);
    }

    // visited
    dirn->walk_id = walk_id;
  }

  finally : coda;
}

int walker_descend_begin()
{
  return ++walk_ids;
}
