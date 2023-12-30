/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "lorien/path_normalize.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstat.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "narrator/fixed.h"
#include "valyria/llist.h"
#include "narrator.h"

#include "filesystem.internal.h"
#include "configuration.h"
#include "args.h"
#include "fsent.h"
#include "stats.h"
#include "events.h"
#include "vertex.h"
#include "pattern_parser.h"
#include "pattern_parser.internal.h"
#include "pattern/lookup/lookup.h"
#include "channel.h"
#include "system_state.h"

#include "zcat.h"
#include "common/attrs.h"

fstree fstree_root;
filesystem filesystem_root;

fstree fstree_shadow;
filesystem filesystem_shadow;

llist filesystem_reconcile_list = LLIST_INITIALIZER(filesystem_reconcile_list);

static llist fstree_freelist = LLIST_INITIALIZER(fstree_freelist);
static llist filesystem_freelist = LLIST_INITIALIZER(filesystem_freelist);
static pattern_parser *parser;

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

  llist_append(&fstree_freelist, &fst->lln);
  if(fst->fs) {
    llist_append(&filesystem_freelist, &fst->fs->lln);
  }
}

static void fstree_root_destroy()
{
  fstree *child;

  if(fstree_root.fs == 0) {
    return;
  }

  // fstree root
  rbtree_foreach(&fstree_root.down, child, rbn) {
    fstree_node_destroy(child);
  }

  rbtree_init(&fstree_root.down);
  rbnode_init(&fstree_root.rbn);
  filesystem_root.invalidate = g_args.default_filesystem_invalidate;

  // shadow root
  rbtree_foreach(&fstree_shadow.down, child, rbn) {
    fstree_node_destroy(child);
  }

  rbtree_init(&fstree_shadow.down);
  rbnode_init(&fstree_shadow.rbn);
}

static void fstree_shadow_destroy()
{
  fstree *child;

  if(fstree_shadow.fs == 0) {
    return;
  }

  // shadow root
  rbtree_foreach(&fstree_shadow.down, child, rbn) {
    fstree_node_destroy(child);
  }

  rbtree_init(&fstree_shadow.down);
  rbnode_init(&fstree_shadow.rbn);
}

struct fs_rbn_key {
  const char *label;
  uint16_t label_len;
};

static int fs_rbn_key_cmp(void *keyp, const rbnode *np)
{
  const struct fs_rbn_key *key = keyp;
  const fstree *fst = containerof(np, typeof(*fst), rbn);

  return memncmp(key->label, key->label_len, fst->label, fst->label_len);
}

//
// trace
//

static int children_compare(const void *Ap, const void *Bp)
{
  const fstree *A = *(typeof(A)*)Ap;
  const fstree *B = *(typeof(B)*)Bp;

  return memncmp(A->label, A->label_len, B->label, B->label_len);
}

static size_t fstree_node_cat(fstree * restrict fst, uint8_t lvl, void * restrict dst, size_t sz)
{
  fstree *child;
  size_t z;
  int x;
  fstree *children[128];
  uint16_t children_len;

  z = 0;
  for(x = 0; x < lvl; x++) {
    z += zcatc(dst + z, sz - z, ' ');
  }
  z += zcatw(dst + z, sz - z, fst->label, fst->label_len);
  if(fst->fs)
  {
    z += zcats(dst + z, sz - z, " : ");
    z += zcats(dst + z, sz - z, attrs16_name_byvalue(invalidate_attrs, fst->fs->invalidate) ?: "(none)");
  }
  z += zcatc(dst + z, sz - z, '\n');

  children_len = 0;
  rbtree_foreach(&fst->down, child, rbn) {
    RUNTIME_ASSERT(children_len < (sizeof(children) / sizeof(*children)));
    children[children_len++] = child;
  }

  qsort(children, children_len, sizeof(*children), children_compare);

  for(x = 0; x < children_len; x++) {
    z += fstree_node_cat(children[x], lvl + 1, dst + z, sz - z);
  }

  return z;
}

size_t fstree_cat(void * restrict dst, size_t sz)
{
  size_t z;

  z = zcatc(dst, sz, '/');
  z += fstree_node_cat(&fstree_root, 0, dst + z, sz - z);
  ((char*)dst)[z] = 0;

  return z;
}

//
// public
//

void filesystem_setup()
{
  rbtree_init(&fstree_root.down);
  rbnode_init(&fstree_root.rbn);
  fstree_root.fs = &filesystem_root;
  filesystem_root.fst = &fstree_root;
  filesystem_root.invalidate = g_args.default_filesystem_invalidate;
  filesystem_root.mkdirs = g_args.default_filesystem_mkdirs;
  filesystem_root.readlinks = g_args.default_filesystem_readlinks;
  filesystem_root.indexed = g_args.default_filesystem_indexed;

  rbtree_init(&fstree_shadow.down);
  rbnode_init(&fstree_shadow.rbn);
  fstree_shadow.fs = &filesystem_shadow;
  filesystem_shadow.fst = &fstree_shadow;
  filesystem_shadow.invalidate = INVALIDATE_NEVER;
  filesystem_shadow.mkdirs = false;
  filesystem_shadow.readlinks = false;
  filesystem_shadow.indexed = false;

  pattern_parser_create(&parser);
}

void filesystem_cleanup()
{
  fstree *fst;
  filesystem *fs;
  llist *lln;

  fstree_root_destroy();
  fstree_shadow_destroy();

  llist_foreach_safe(&fstree_freelist, fst, lln, lln) {
    free(fst);
  }

  llist_foreach_safe(&filesystem_freelist, fs, lln, lln) {
    free(fs);
  }

  pattern_parser_free(parser);
}

void filesystem_reconfigure(configuration * restrict cfg, bool dryrun)
{
  fstree * fst = 0;
  configuration_filesystems_entry *fse;
  configuration_filesystems_mapping *mapping;
  const char *base;
  const char *seg;
  const char *end;
  fstree * parent;
  rbtree_search_context search_ctx;
  rbnode *rbn;
  struct fs_rbn_key rbn_key;

  /* no error cases to check during dry run */
  if(dryrun) {
    return;
  }
  //if(!cfg->filesystems.changed) {
  //  return 0;
  //}

  fstree_root_destroy();

  /* rebuild the root fstree from configuration */
  rbtree_foreach(&cfg->filesystems.set, mapping, rbn) {
    fse = &mapping->val;
    parent = &fstree_root;
    base = mapping->key.path;
    seg = base + 1;
    while(*seg)
    {
      end = seg + 1;
      while(*end && *end != '/') {
        end++;
      }

      rbn_key.label = seg;
      rbn_key.label_len = end - seg;

      if((rbn = rbtree_search(&parent->down, &search_ctx, &rbn_key, fs_rbn_key_cmp)))
      {
        fst = containerof(rbn, typeof(*fst), rbn);
      }
      else
      {
        if((fst = llist_shift(&fstree_freelist, typeof(*fst), lln)) == 0) {
          xmalloc(&fst, sizeof(*fst));
        }

        memcpy(fst->label, seg, end - seg);
        fst->label[end - seg] = 0;
        fst->label_len = end - seg;
        fst->fs = 0;
        rbtree_init(&fst->down);

        rbtree_insert(&parent->down, &search_ctx, &fst->rbn);
      }

      parent = fst;

      seg = end;
      if(*seg)
        seg++;
    }

    fst = parent;
    if(fst->fs == 0) {
      if((fst->fs = llist_shift(&filesystem_freelist, typeof(*fst->fs), lln)) == 0) {
        xmalloc(&fst->fs, sizeof(*fst->fs));
      }
    }

    fst->fs->mkdirs = g_args.default_filesystem_mkdirs;
    if(fse->mkdirs.v) {
      fst->fs->mkdirs = fse->mkdirs.b;
    }
    fst->fs->readlinks = g_args.default_filesystem_readlinks;
    if(fse->readlinks.v) {
      fst->fs->readlinks = fse->readlinks.b;
    }
    fst->fs->indexed = g_args.default_filesystem_indexed;
    if(fse->indexed.v) {
      fst->fs->indexed = fse->indexed.b;
    }
    fst->fs->invalidate = g_args.default_filesystem_invalidate;
    if(fse->invalidate.v) {
      fst->fs->invalidate = fse->invalidate.e;
    }
    fst->fs->fst = fst;
  }

  return;
}

const fstree *fstree_down(fstree * restrict fst, const char * restrict label, uint16_t label_len)
{
  rbtree_search_context search_ctx;
  struct fs_rbn_key rbn_key;
  rbnode *rbn;

  rbn_key.label = label;
  rbn_key.label_len = label_len;

  if((rbn = rbtree_search(&fst->down, &search_ctx, &rbn_key, fs_rbn_key_cmp))) {
    return containerof(rbn, typeof(*fst), rbn);
  }

  return 0;
}

size_t filesystem_absolute_path_cat(void * restrict dst, size_t sz, const filesystem * restrict fs)
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
    z += zcats(dst + z, sz - z, "/");

  if(fst == &fstree_shadow)
    z += zcats(dst + z, sz - z, "//");

  for(x = bsl - 1; x >= 0; x--)
  {
    if(z > 2)
      z += zcats(dst + z, sz - z, "/");
    z += zcatw(dst + z, sz - z, bs[x]->label, bs[x]->label_len);
  }

  ((char*)dst)[z] = 0;

  return z;
}

static void vertex_dir_mkdir(fsent *fse)
{
  const filesystem *fs;
  char path[PATH_MAX];
  channel *chan;
  bamipc_message *msg;
  const char *errstr;

  if(vertex_state_get(&fse->vertex) == VERTEX_OK) {
    return;
  }

  fs = fsent_filesystem_get(fse);
  if(!fs->mkdirs) {
    vertex_state_set(&fse->vertex, VERTEX_OK);
    return;
  }

  STATS_INC(g_stats.mkdir);
  fsent_absolute_path_cat(path, sizeof(path), fse);
  if(mkdir(path, 0755) != 0 && errno != EEXIST) {
    errstr = strerror(errno);
    goto err;
  }
  vertex_state_set(&fse->vertex, VERTEX_OK);
  return;

err:
  msg = system_error_event(&chan);
  msg->code = EINVAL;
  msg->size += zcats(msg->text + msg->size, sizeof(msg->text) - msg->size, errstr);
  msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, " with path %s", path);
  events_publish(chan, msg);
}

void fsent_symlink_resolve(fsent *fse)
{
/*

NOTE : this is not being used yet

 */

  const filesystem *fs;
  char path[PATH_MAX];
  char target[PATH_MAX];
  int r;
  channel *chan;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  int error;
  pattern *pattern;
  const moria_vertex *ref[2];
  module *mod;
  bacon_value *vars;

  if(vertex_type_get(&fse->vertex) != VERTEX_SYMLINK) {
    return;
  } else if(vertex_state_get(&fse->vertex) == VERTEX_OK) {
    return;
  }

  ref[0] = 0;

  fs = fsent_filesystem_get(fse);
  if(!fs->readlinks) {
    goto end;
  }

  STATS_INC(g_stats.readlink);

  fsent_absolute_path_cat(path, sizeof(path), fse);
  if((r = readlink(path, target, sizeof(target))) == -1) {
    error = errno;
    msg = system_error_event(&chan);
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("readlink failure")
      { SET
        { MAPPING
          STRING("symlink")
          STRING(path)
        }
        { MAPPING
          STRING("error")
          STRING(strerror(error))
        }
      }
    }

    bacon_writer_close(builder_writer);
    msg->code = error;
    msg->size = fixed.l;
    events_publish(chan, msg);
    goto end;
  }

  /* parse the target string */
  if(lookup_pattern_parse(parser, target, sizeof(target), path, &pattern) != 0) {
    error = EINVAL;
    msg = system_error_event(&chan);

    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("symlink target parse failure")
      { SET
        { MAPPING
          STRING("symlink")
          STRING(path)
        }
        { MAPPING
          STRING("error")
          BYTES(parser->lookup_yyu.error_str, parser->lookup_yyu.error_len)
        }
      }
    }

    bacon_writer_close(builder_writer);
    msg->code = error;
    msg->size = fixed.l;
    events_publish(chan, msg);
    goto end;
  }

  /* resolve */
  vars = 0;
  if((mod = fsent_module_get(fse))) {
    vars = mod->vars;
  }
  pattern_lookup(pattern, vars, mod->dir_node, 0);
  if(0) {
    msg = system_error_event(&chan);

    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("symlink resolution failure")
      { SET
        { MAPPING
          STRING("symlink")
          STRING(path)
        }
        { MAPPING
          STRING("target")
          STRING(target)
        }
      }
    }

    bacon_writer_close(builder_writer);
    msg->code = ENOENT;
    msg->size = fixed.l;
    events_publish(chan, msg);
    goto end;
  }

  vertex_state_set(&fse->vertex, VERTEX_OK);

end:
  if((fse->vertex.ref = (void*)ref[0])) {
    fse->vertex.attrs |= MORIA_VERTEX_LINK;
  } else {
    fse->vertex.attrs &= ~MORIA_VERTEX_LINK;
  }
}

void filesystem_reconcile()
{
  fsent *fse;

  /* this algorithm for mkdirs relies on the fact that, because pattern-generate is left-to-right, directories
   * that need to be created will have been added to this list in the proper order */

  while((fse = llist_shift(&filesystem_reconcile_list, typeof(*fse), fs_reconcile_lln))) {
    switch(vertex_type_get(&fse->vertex)) {
      case VERTEX_MODULE_DIR:
      case VERTEX_DIR:
        vertex_dir_mkdir(fse);
        break;
/*
      case VERTEX_SYMLINK:
        fsent_symlink_resolve(fse);
        break;
*/
      default:
        vertex_show(&fse->vertex);
        RTA;
    }

    if(system_error) {
      llist_unshift(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
      break;
    }
  }
}

void filesystem_mkdir_push(struct fsent * restrict fse)
{
  llist_delete(&fse->fs_reconcile_lln);
  llist_push(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
}

void filesystem_mkdir_unshift(struct fsent * restrict fse)
{
  llist_delete(&fse->fs_reconcile_lln);
  llist_unshift(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
}

static void fstree_describe_fstree(
    bacon_writer * restrict builder_writer
  , const struct fstree *fst
  , char * restrict dst
  , size_t sz
  , uint16_t z
)
{
  fstree *child;
  const filesystem *fs;

  if(fst != &fstree_root) {
    z += zcats(dst + z, sz - z, "/");
    z += zcatw(dst + z, sz - z, fst->label, fst->label_len);
  }

  if(!(fs = fst->fs)) {
    rbtree_foreach(&fst->down, child, rbn) {
      fstree_describe_fstree(builder_writer, child, dst, sz, z);
    }
    return;
  }

  { MAPPING
    if(fst == &fstree_root) {
      LOADF("fst %p fs %p /", fst, fs)
    } else {
      LOADF("fst %p fs %p %.*s", fst, fs, z, dst)
    }
    { SET
      if(fs->invalidate) {
        MAPPING
        STRING("invalidate")
        STRING(attrs16_name_byvalue(invalidate_attrs, fs->invalidate));
      }
      { MAPPING
        STRING("mkdirs")
        BOOL(fs->mkdirs)
      }
      { MAPPING
        STRING("readlinks")
        BOOL(fs->readlinks)
      }
      { MAPPING
        STRING("indexed")
        BOOL(fs->indexed)
      }
      if(!rbtree_empty(&fst->down)) {
        LIST
        rbtree_foreach(&fst->down, child, rbn) {
          fstree_describe_fstree(builder_writer, child, dst, sz, z);
        }
      }
    }
  }
}

void fstree_describe(bacon_writer * restrict writer)
{
  char path[1024];
  fstree_describe_fstree(writer, &fstree_root, path, sizeof(path), 0);
}

void fstree_show()
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  fstree_describe(builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}
