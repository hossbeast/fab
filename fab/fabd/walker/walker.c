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

#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "lorien/nftwat.h"
#include "xlinux/xstring.h"
#include "xlinux/xinotify.h"
#include "valyria/list.h"
#include "valyria/map.h"

#include "walker.internal.h"
#include "node.h"
#include "node_operations.h"
#include "filesystem.h"
#include "notify_thread.h"
#include "module.h"
#include "path.h"

#include "macros.h"
#include "zbuffer.h"

static int walk_ids = 1;

//
// static
//

static uint8_t fstype_ftwinfo(ftwinfo * info)
{
  if(info->type == FTWAT_D)
    return NODE_FSTYPE_DIR;

  if(info->type == FTWAT_F)
    return NODE_FSTYPE_FILE;

  return 0;
}

xapi walker_visit(int method, ftwinfo * info, void * arg, int * stop)
{
  enter;

  walker_context * ctx = arg;
  const filesystem * fs = 0;
  module * mod = 0;
  vertex * v;
  node * n;

  if(!info->parent)
  {
    if(method == FTWAT_PRE && ctx->root)
    {
      n = ctx->root;
    }
    else if(method == FTWAT_PRE)
    {
      if(ctx->ancestor && ctx->ancestor->mod->leaf)
        mod = ctx->ancestor->mod;
      if(!mod)
        mod = ctx->modlookup(ctx, info->path, info->pathl);
      if(mod)
        fs = mod->base->fs;
      else if(ctx->ancestor && ctx->ancestor->fs->leaf)
        fs = ctx->ancestor->fs;
      if(!fs)
        fs = ctx->fslookup(ctx, info->path, info->pathl);
      fatal(ctx->create, ctx, &n, fstype_ftwinfo(info), fs, mod, info->path + info->name_off);
      ctx->root = n;

      if(ctx->ancestor)
        fatal(ctx->connect, ctx, ctx->ancestor, n);
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
    node * parent = info->parent->udata;
    vertex * v = vertex_downs(
        vertex_containerof(parent)
      , info->path + info->name_off
    );
    if(v)
    {
      n = vertex_value(v);
    }
    else
    {
      if(parent->mod && parent->mod->leaf)
        mod = parent->mod;
      if(!mod)
        mod = ctx->modlookup(ctx, info->path, info->pathl);
      if(mod)
        fs = mod->base->fs;
      else if(info->type == FTWAT_D && parent->fs->leaf)
        fs = parent->fs;
      if(!fs)
        fs = ctx->fslookup(ctx, info->path, info->pathl);
      
      fatal(ctx->create, ctx, &n, fstype_ftwinfo(info), fs, mod, info->path + info->name_off);
      fatal(ctx->connect, ctx, parent, n);
    }
  }

  info->udata = n;
  v = vertex_containerof(n);

  // this node was visited
  if(method != FTWAT_POST)
    n->walk_id = ctx->walk_id;

  if(method == FTWAT_PRE)
  {
    // stop the traversal on a leaf inotify dir which has already been explored
    if(n->fs->attrs == FILESYSTEM_INVALIDATE_NOTIFY)
    {
      if(n->fs->leaf && n->wd != -1)
        *stop = 1;
    }
  }

  if(method == FTWAT_POST)
  {
    if(n->fs->attrs == FILESYSTEM_INVALIDATE_NOTIFY)
    {
      if(n->wd == -1)
        fatal(ctx->watch, ctx, n);
    }
    else
    {
      int x;
      for(x = v->down->l - 1; x >= 0; x--)
      {
        edge * e = list_get(v->down, x);
        vertex * B = e->B;
        node * Bn = vertex_value(B);
        if(Bn->walk_id != ctx->walk_id)
          fatal(ctx->disintegrate, ctx, e);
      }
    }
  }

  if(info->type == FTWAT_F)
    fatal(ctx->refresh, ctx, n);

  finally : coda;
}

static xapi create(walker_context * ctx, node ** restrict n, uint8_t fstype, const filesystem * restrict fs, module * restrict mod, const char * restrict name)
{
  xproxy(node_creates, n, fstype, fs, mod, name);
}

static const filesystem * fslookup(walker_context * ctx, const char * const restrict path, size_t pathl)
{
  return filesystem_lookup(path, pathl);
}

static module * modlookup(walker_context * ctx, const char * const restrict path, size_t pathl)
{
  return module_lookup(path, pathl);
}

static xapi refresh(walker_context * ctx, node * n)
{
  xproxy(node_refresh, n, 0);
}

static xapi watch(walker_context * ctx, node * n)
{
  xproxy(notify_thread_watch, n);
}

static xapi connect(walker_context * restrict ctx, node * restrict parent, node * restrict n)
{
  xproxy(node_connect, parent, n, RELATION_TYPE_FS);
}

static xapi disintegrate(walker_context * restrict ctx, edge * restrict e)
{
  xproxy(node_disintegrate_fs, e, 0);
}

//
// public
//

xapi walker_walk(node ** restrict root, node * restrict base, node * restrict ancestor, const char * restrict abspath, int walk_id)
{
  enter;

  walker_context ctx;

  if(walk_id != walk_ids)
    walk_id = ++walk_ids;

  ctx.walk_id = walk_id;
  ctx.ancestor = ancestor;
  ctx.root = 0;
  if(base)
    ctx.root = base;

  // ops
  ctx.create = create;
  ctx.fslookup = fslookup;
  ctx.modlookup = modlookup;
  ctx.refresh = refresh;
  ctx.watch = watch;
  ctx.connect = connect;
  ctx.disintegrate = disintegrate;

  // filesystem traversal from the root
  fatal(nftwat, AT_FDCWD, abspath, walker_visit, 64, &ctx);

  if(root)
    *root = ctx.root;

  finally : coda;
}

int walker_walk_begin()
{
  return ++walk_ids;
}
