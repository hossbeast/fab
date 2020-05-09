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

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "xapi.h"

#include "lorien/path_normalize.h"
#include "value.h"
#include "valyria/list.h"
#include "valyria/rbtree.h"
#include "valyria/map.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "narrator.h"

#include "filesystem.internal.h"
#include "config.internal.h"
#include "config_parser.h"
#include "CONFIG.errtab.h"
#include "box.h"
#include "stats.h"

#include "zbuffer.h"
#include "hash.h"
#include "attrs.h"

fstree fstree_root;
filesystem filesystem_root;

fstree fstree_shadow;
filesystem filesystem_shadow;

static llist fstree_freelist = LLIST_INITIALIZER(fstree_freelist);
static llist filesystem_freelist = LLIST_INITIALIZER(filesystem_freelist);

attrs16 * invalidate_attrs = (attrs16[]) {{
#define INVALIDATE_DEF(x, n, r, y) + 1
    num : 0 INVALIDATE_TYPE_TABLE
  , members : (member16[]) {
#undef INVALIDATE_DEF
#define INVALIDATE_DEF(x, n, r, y) { name : n, value : UINT16_C(y), range : UINT16_C(r) },
INVALIDATE_TYPE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(invalidate_attrs);
}

static void fstree_node_destroy(fstree * restrict fst)
{
  fstree *child;

  rbtree_foreach(&fst->down, child, rbn) {
    fstree_node_destroy(child);
  }

  llist_append(&fstree_freelist, fst, lln);
  if(fst->fs) {
    llist_append(&filesystem_freelist, fst->fs, lln);
  }
}

static void fstree_destroy()
{
  fstree *child;

  if(fstree_root.fs == 0)
    return;

  // fstree_root
  rbtree_foreach(&fstree_root.down, child, rbn) {
    fstree_node_destroy(child);
  }

  rbtree_init(&fstree_root.down);
  rbnode_init(&fstree_root.rbn);
  filesystem_root.attrs = 0;

  // fstree_shadow
  rbtree_foreach(&fstree_shadow.down, child, rbn) {
    fstree_node_destroy(child);
  }

  rbtree_init(&fstree_shadow.down);
  rbnode_init(&fstree_shadow.rbn);
}

static int children_compare(const void *Ap, const void *Bp)
{
  const fstree *A = *(typeof(A)*)Ap;
  const fstree *B = *(typeof(B)*)Bp;

  return memncmp(A->name, A->namel, B->name, B->namel);
}

static size_t fstree_node_znload(fstree * restrict fst, uint8_t lvl, void * restrict dst, size_t sz)
{
  fstree *child;
  size_t z;
  int x;
  fstree *children[128];
  uint16_t children_len;

  z = 0;
  for(x = 0; x < lvl; x++)
    z += znloadc(dst + z, sz - z, ' ');
  z += znloadw(dst + z, sz - z, fst->name, fst->namel);
  if(fst->fs)
  {
    z += znloads(dst + z, sz - z, " : ");
    z += znloads(dst + z, sz - z, attrs16_name_byvalue(invalidate_attrs, fst->fs->invalidate) ?: "(none)");
  }
  z += znloadc(dst + z, sz - z, '\n');

  children_len = 0;
  rbtree_foreach(&fst->down, child, rbn) {
    children[children_len++] = child;
  }

  qsort(children, children_len, sizeof(*children), children_compare);

  for(x = 0; x < children_len; x++) {
    z += fstree_node_znload(children[x], lvl + 1, dst + z, sz - z);
  }

  return z;
}

//
// internal
//

size_t fstree_znload(void * restrict dst, size_t sz)
{
  size_t z;

  z = znloadc(dst, sz, '/');
  z += fstree_node_znload(&fstree_root, 0, dst + z, sz - z);
  ((char*)dst)[z] = 0;

  return z;
}

//
// public
//

xapi filesystem_setup()
{
  enter;

  rbtree_init(&fstree_root.down);
  rbnode_init(&fstree_root.rbn);
  fstree_root.fs = &filesystem_root;
  filesystem_root.fst = &fstree_root;

  rbtree_init(&fstree_shadow.down);
  rbnode_init(&fstree_shadow.rbn);
  fstree_shadow.fs = &filesystem_shadow;
  filesystem_shadow.fst = &fstree_shadow;
  filesystem_shadow.invalidate = INVALIDATE_NEVER;

  llist_init_node(&fstree_freelist);
  llist_init_node(&filesystem_freelist);

  finally : coda;
}

xapi filesystem_cleanup()
{
  enter;

  fstree *fst;
  filesystem *fs;
  llist *lln;

  fstree_destroy();

  llist_foreach_safe(&fstree_freelist, fst, lln, lln) {
    wfree(fst);
  }

  llist_foreach_safe(&filesystem_freelist, fs, lln, lln) {
    wfree(fs);
  }

  finally : coda;
}

struct fs_rbn_key {
  const char *name;
  uint16_t namel;
};

static int rbn_key_cmp(void *keyp, const rbnode *np)
{
  const struct fs_rbn_key *key = keyp;
  const fstree *fst = containerof(np, typeof(*fst), rbn);

  return memncmp(key->name, key->namel, fst->name, fst->namel);
}

xapi filesystem_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  char path[512];

  fstree * fst = 0;
  int x;
  const char *key;
  struct config_filesystem_entry *fse;
  const char *base;
  const char *seg;
  const char *end;
  fstree * parent;
  rbtree_search_context search_ctx;
  rbnode *rbn;
  struct fs_rbn_key rbn_key;

  if(dry)
    goto XAPI_FINALIZE;

  if(!(cfg->filesystems.attrs & CONFIG_CHANGED))
    goto XAPI_FINALIZE;

  fstree_destroy();

  for(x = 0; x < cfg->filesystems.entries->table_size; x++)
  {
    if(!(key = map_table_key(cfg->filesystems.entries, x)))
      continue;

    fse = map_table_value(cfg->filesystems.entries, x);
    path_normalize(path, sizeof(path), key);

    parent = &fstree_root;
    base = path;
    seg = base + 1;
    while(*seg)
    {
      end = seg + 1;
      while(*end && *end != '/')
        end++;

      rbn_key.name = seg;
      rbn_key.namel = end - seg;

      if((rbn = rbtree_search(&parent->down, &search_ctx, &rbn_key, rbn_key_cmp)))
      {
        fst = containerof(rbn, typeof(*fst), rbn);
      }
      else
      {
        if((fst = llist_shift(&fstree_freelist, typeof(*fst), lln)) == 0)
        {
          fatal(xmalloc, &fst, sizeof(*fst));
        }

        memcpy(fst->name, seg, end - seg);
        fst->name[end - seg] = 0;
        fst->namel = end - seg;
        fst->fs = 0;
        rbtree_init(&fst->down);

        rbtree_insert_node(&parent->down, &search_ctx, &fst->rbn);
      }

      parent = fst;

      seg = end;
      if(*seg)
        seg++;
    }

    fst = parent;
    if(fst->fs == 0)
    {
      if((fst->fs = llist_shift(&filesystem_freelist, typeof(*fst->fs), lln)) == 0)
      {
        fatal(xmalloc, &fst->fs, sizeof(*fst->fs));
      }
    }

    fst->fs->attrs = fse->invalidate->v;
    fst->fs->fst = fst;
  }

  finally : coda;
}

const fstree * fstree_down(fstree * restrict fst, const char * restrict name, uint16_t namel)
{
  rbtree_search_context search_ctx;
  struct fs_rbn_key rbn_key;
  rbnode *rbn;

  rbn_key.name = name;
  rbn_key.namel = namel;

  if((rbn = rbtree_search(&fst->down, &search_ctx, &rbn_key, rbn_key_cmp)))
  {
    return containerof(rbn, typeof(*fst), rbn);
  }

  return 0;
}

size_t filesystem_get_absolute_path(const filesystem * restrict fs, void * restrict dst, size_t sz)
{
  size_t z;
  const fstree *bs[64];
  uint8_t bsl = 0;
  const fstree *fst;
  int x;

  fst = fs->fst;
  while(fst)
  {
    if(fst == &fstree_root)
      break;

    if(fst == &fstree_shadow)
      break;

    bs[bsl++] = fst;
    fst = fst->up;
  }

  z = 0;

  if(fst == &fstree_root)
    z += znloads(dst + z, sz - z, "/");

  if(fst == &fstree_shadow)
    z += znloads(dst + z, sz - z, "//");

  for(x = bsl - 1; x >= 0; x--)
  {
    if(z > 2)
      z += znloads(dst + z, sz - z, "/");
    z += znloadw(dst + z, sz - z, bs[x]->name, bs[x]->namel);
  }

  ((char*)dst)[z] = 0;

  return z;
}
