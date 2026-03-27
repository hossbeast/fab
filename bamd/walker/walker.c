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
#include "moria/edge.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "bacon/value.h"

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
#include "vertices.h"
#include "system.h"
#include "channel.h"
#include "events/events.h"
#include "edges.h"
#include "vertices.h"
#include "config_system.h"
#include "notify.h"
#include "symlink.h"
#include "disintegrate.h"

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
rbtree *walker_exclude_list;           // match_pattern
static rbtree *walker_special_list;    // config_specials_entry

static uint16_t walk_ids;

//
// static
//

static void stathash(fsent * restrict n, char * restrict path, uint16_t path_len)
{
  struct stat stb;
  int r;
  char c;
  fstree * fst;
  uint32_t hash;

  fst = vertex_fstree_of(&n->vertex);
  filesystem_stats.stathash++;
  fst->fstree_stats.stathash++;

  hash = n->hash;

  c = path[path_len];
  path[path_len] = 0;
  if((r = uxfstatats(g_basedirfd, AT_SYMLINK_NOFOLLOW, &stb, path)) == 0) {
    hash = hash32(hash, (uint32_t[]) { 0x82802206 }, 4);

    /* accumulate specific fields we care about - surely this can be optimized alot */
    hash = hash32(hash, &stb.st_dev, sizeof(stb.st_dev));
    hash = hash32(hash, &stb.st_ino, sizeof(stb.st_ino));
    hash = hash32(hash, &stb.st_mode, sizeof(stb.st_mode));     // type
    hash = hash32(hash, &stb.st_uid, sizeof(stb.st_uid));
    hash = hash32(hash, &stb.st_gid, sizeof(stb.st_gid));
    hash = hash32(hash, &stb.st_size, sizeof(stb.st_size));
    hash = hash32(hash, &stb.st_mtim, sizeof(stb.st_mtim));     // last modification
    hash = hash32(hash, &stb.st_ctim, sizeof(stb.st_ctim));     // last status change
  } else {
    hash = hash32(hash, (uint32_t[]) { 0x5d221ba5 }, 4);
  }

  n->hash = hash;
  path[path_len] = c;
}

static void contenthash(fsent * restrict n, char * restrict path, uint16_t path_len)
{
  int fd = -1;
  off_t end = 0;
  void *p = MAP_FAILED;
  char c;
  fstree *fst;
  char buf[PATH_MAX];
  ssize_t r;
  uint32_t hash;

  fst = vertex_fstree_of(&n->vertex);
  filesystem_stats.contenthash++;
  fst->fstree_stats.contenthash++;

  hash = n->hash;

  /* save */
  c = path[path_len];
  path[path_len] = 0;

  if(vertex_type_get(&n->vertex) == VERTEX_SYMLINK) {
    if((r = readlinkat(g_basedirfd, path, buf, sizeof(buf))) >= 0) {
      hash = hash32(hash, (uint32_t[]) { 0x48368027 }, 4);
      hash = hash32(hash, buf, r);
    } else {
      hash = hash32(hash, (uint32_t[]) { 0x9439344a }, 4);
    }
  } else if((fd = xopenats(O_RDONLY, g_basedirfd, path)) != -1) {
    end = xlseek(fd, 0, SEEK_END);
    if(end > 0) {
      p = xmmap(0, end, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
      hash = hash32(hash, (uint32_t[]) { 0x5eb6ee69 }, 4);
      hash = hash32(hash, p, end);
      ixmunmap(&p, end);
    } else {
      hash = hash32(hash, (uint32_t[]) { 0x24e41d36 }, 4);
    }
    xclose(fd);
  }

  n->hash = hash;

  /* restore */
  path[path_len] = c;
}

void walker_poke(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  uint32_t prev;
  const fstree *fst;
  char stor[PATH_MAX];

  fst = vertex_fstree_of(&n->vertex);
  vertex_exists_set(&n->vertex, true);

  if(vertex_invalid_get(&n->vertex) && n->hash != 0) {
    return;   /* already invalid */
  }

  prev = n->hash;

  /* honor filesystem setting */
  switch(fst->invalidation) {
    case INVALIDATION_NEVER:
      n->hash = 0;
      break;
    case INVALIDATION_ALWAYS:
      n->hash = prev + 1;
      break;
    case INVALIDATION_NOTIFY:
      n->hash = 1;
      break;
    case INVALIDATION_STAT:
    case INVALIDATION_CONTENT:
      if(!path) {
        path_len = fsent_absolute_path_cat(stor, sizeof(stor), n);
        path = stor;
      }

      n->hash = 0xc084f5ae;
      if(fst->invalidation == INVALIDATION_STAT) {
        stathash(n, path, path_len);
      } else if(fst->invalidation == INVALIDATION_CONTENT) {
        contenthash(n, path, path_len);
      }
      break;
  }

  if(vertex_type_get(&n->vertex) == VERTEX_SYMLINK) {
    if(prev == 0) {
      symlink_readlink_push(n);   /* resolve the symlink initially, it becomes fresh when its resolved */
    } else if(n->hash != prev) {
      vertex_invalidate(&n->vertex, invalidation);
      symlink_readlink_push(n);   /* on change, resolve the symlink again */
    }
  } else if(prev == 0) {
    vertex_mark_fresh(&n->vertex);
  } else if(n->hash != prev) {
    if(fst->invalidation != INVALIDATION_NOTIFY) {
      vertex_invalidate(&n->vertex, invalidation);
    }
  }
}

static int uxfaccessat(int dirfd, const char *path, int mode, int flags)
{
  /* path is always an absolute path */
  if(faccessat(INT32_MAX, path, F_OK, AT_EACCESS | AT_SYMLINK_NOFOLLOW) == 0) {
    return 0;
  }

  switch(errno) {
    case EACCES:
    case ENOENT:
      break;
    RTA;
  }

  return -1;
}

/*
 * call walker_poke to reconcile an fsent against the file system
 * do nothing if the fsent belongs to a mounted fstree, because in thaat case, this is already handled by
 * walker_descend
 * but needed for fsents which might not belong to any such fstree
 */
static void walker_query(fsent * restrict n, char * restrict path, uint16_t path_len, graph_invalidation_context * restrict invalidation)
{
  const fstree *fst;
  const fsent *parent;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

  if(!(parent = fsent_parent(n))) {
    RTA;
    return;
  }

  fst = parent->fst;
  if(fst->mount) {
    return; /* already being handled via traversal */
  }

  /* path is always an absolute path */
  if(uxfaccessat(INT32_MAX, path, F_OK, AT_EACCESS | AT_SYMLINK_NOFOLLOW) != 0) {
    edges_init(&edges);
    vertices_init(&unlinking);
    vertices_init(&releasing);
    vertices_push(&unlinking, &n->vertex);
    graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
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

bool walker_exclude_list_evaluate(fsent * restrict n)
{
  const match_pattern *pat;

  if(!walker_exclude_list) {
    return false;
  }

  rbtree_foreach(walker_exclude_list, pat, pattern.rbn) {
    if((pattern_match(pat, 0, n))) {
      fsent_unmount_candidate(n);
      return true;
    }
  }

  return false;
}

/* try to destroy a vertex ; it may not complete, if the node has edges. But at least
 * affirmatively delete its upwards identity edge */
static void vertex_excise(moria_vertex *v, graph_invalidation_context * restrict invalidation)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

  vertices_init(&unlinking);
  vertices_init(&releasing);
  edges_init(&edges);

  vertices_push(&unlinking, v);
  edges_push(&edges, v->up_identity);
  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
}

static void walker_error(int method, ftwinfo *info, void *arg, int error)
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
    STRING("walker error")
    { SET
      { MAPPING STRING("path")
        BYTES(info->path, info->pathl)
      }
      { MAPPING STRING("error")
        STRING(strerror(error))
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void walker_visit(int method, ftwinfo *info, void *arg, int *stop)
{
  walker_context * ctx = arg;
  const fstree *fst = 0;
  moria_vertex *v;
  fsent *n;
  fsent *parent;
  moria_edge *e;
  vertex_filetype filetype;
  fsent *Bn;
  moria_foreach_state st;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  uint32_t attrs;

  n = 0;
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

    if(info->type == FTWAT_D) {
      filetype = VERTEX_FILETYPE_DIR;
    } else if(info->type == FTWAT_LNK) {
      filetype = VERTEX_FILETYPE_SYMLINK;
    } else {
      filetype = VERTEX_FILETYPE_REG;
    }

    if((v = moria_vertex_downs(&parent->vertex, info->path + info->name_off))) {
      n = containerof(v, fsent, vertex);
      if(vertex_filetype_get(&n->vertex) != filetype) {
        /* if the filetype has changed, destroy and re-create the node. Note that it may not actually
         * be destroyed by this operation, but deleting its identity edge is sufficient to allow a
         * new node with the same name to be attached under the parent */
        vertex_excise(v, ctx->invalidation);
        n = 0;
      }
    }

    if(n == 0) {
      attrs = VERTEX_UNCREATED;
      if(filetype == VERTEX_FILETYPE_DIR) {
        /* Issue with UNCREATED here is that this directory is *known* to exist -- uncreated would mean
         * that, fs permitting, mkdir will be called on this directory and return EEXIST */
        attrs = VERTEX_OK;
      }

      fsent_create(&n, filetype, attrs, info->path + info->name_off, info->pathl - info->name_off);
      fsedge_connect(n, parent, ctx->invalidation);
      vertex_alloc_probe(&n->vertex);
    }
  }
  info->udata = n;

  if(info->type == FTWAT_D && method == FTWAT_PRE) {
    fsent_dir_mkdir_cancel(n);
    if(walker_exclude_list_evaluate(n)) {
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

  fst = vertex_fstree_of(&n->vertex);
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
      /* disintegrate nodes not visited on this walk */
      vertices_init(&unlinking);
      vertices_init(&releasing);
      edges_init(&edges);
      moria_foreach_edge_down(&st, &n->vertex, e) {
        if(edge_type_get(e) != EDGE_FS) {
          continue;
        }
        Bn = containerof(e->B, fsent, vertex);
        if(Bn->descend_walk_id == ctx->walk_id) {
          continue; /* still exists */
        }

        vertices_push(&unlinking, &Bn->vertex);
      }
      graph_disintegrate(&edges, &unlinking, &releasing, ctx->invalidation);
    }
  }
  else if(info->type == FTWAT_F || info->type == FTWAT_LNK)
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

  nftwat(AT_FDCWD, abspath, walker_visit, walker_error, 64, FTWAT_SYMLINKS, &ctx);
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
  moria_vertex *v;
  fsent *fse;
  struct stat stb;
  size_t z;
  vertex_filetype filetype;

  z = path_len;
  z += zcats(path + z, path_size - 1 - z, "/");
  z += zcatw(path + z, path_size - 1 - z, name, name_len);
  path[z] = 0;

  filetype = 0;
  if(uxfstatats(INT32_MAX, AT_SYMLINK_NOFOLLOW, &stb, path) == 0) {
    if((stb.st_mode & S_IFMT) == S_IFDIR) {
      filetype = VERTEX_FILETYPE_DIR;
    } else if((stb.st_mode & S_IFMT) == S_IFLNK) {
      filetype = VERTEX_FILETYPE_SYMLINK;
    } else {
      filetype = VERTEX_FILETYPE_REG;
    }
  }

  dirv = &dirn->vertex;
  if((v = moria_vertex_downw(dirv, name, name_len))) {
    if(vertex_filetype_get(v) != filetype) {
      /* destroy / re-create on filetype changes */
      vertex_excise(v, invalidation);
      v = 0;
    }
  }

  if(!v && filetype) {
    fsent_create(&fse, filetype, VERTEX_UNCREATED, name, name_len);
    fsedge_connect(fse, dirn, invalidation);
    vertex_alloc_probe(&fse->vertex);
    v = &fse->vertex;
  }

  if(v) {
    fse = containerof(v, typeof(*fse), vertex);
    walker_query(fse, path, z, invalidation);
  }
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
  const config_specials_entry *cse;

  if(walk_id != walk_ids) {
    walk_id = ++walk_ids;
  }

  abspath_len = fsent_absolute_path_cat(abspath, sizeof(abspath), dirn);
  dirv = &dirn->vertex;

  while(dirv)
  {
    dirn = containerof(dirv, fsent, vertex);
    fsent_dir_confirm(dirn);
    if(dirn->ascend_walk_id == walk_id) {
      break;
    }

    /* look for special files */
    if(walker_special_list) {
      rbtree_foreach(walker_special_list, cse, rbn) {
        special_query(dirn, cse->name->s, cse->name->len, invalidation, abspath, sizeof(abspath), abspath_len);
      }
    }

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

static void include_hit(void *ctx, const moria_vertex *v)
{
  vertices *hits = ctx;
  vertices_push(hits, v);
}

void walker_system_reconfigure(config * restrict cfg, bool dryrun)
{
  graph_invalidation_context invalidation = { };
  moria_vertex *v;
  fsent *n;
  config_fstree_mapping *fse;
  walker_entry *e;
  vertices *eln;
  vertices hits;

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
    fsent_unmount_candidate(e->n);
  }

  llist_splice_head(&include_entry_freelist, &walker_include_entry_list);

  /* these patterns are absolute paths to directory nodes
   * the dir nodes are in the uncreated state
   * directories are normally moved to OK in vertex_dir_mkdir
   * pattern_generate will leave the final node as an fsent instead of a dir */
  vertices_init(&hits);
  rbtree_foreach(&cfg->filesystem.fstrees.set, fse, rbn) {
    pattern_include_generate(fse->key.v, 0, 0, 0, 0, &invalidation, 0, 0, include_hit, &hits);
  }

  vertices_foreach(&hits, v, eln) {
    n = containerof(v, typeof(*n), vertex);
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
    if(walker_exclude_list_evaluate(n)) {
      continue;
    }

    fsent_dir_confirm(n);
    include_entry_list_append(n);
  }
  vertices_destroy(&hits);

  graph_invalidation_end(&invalidation);
}
