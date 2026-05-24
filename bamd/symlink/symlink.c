/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "lorien/path_normalize.h"

#include "fsent.h"
#include "fstree.h"
#include "filesystem.h"
#include "events/events.h"
#include "ref_edge.h"
#include "system.h"
#include "symlink.h"
#include "edge.h"
#include "edges.h"
#include "vertices.h"
#include "disintegrate.h"

static llist symlink_reconcile_list = LLIST_INITIALIZER(symlink_reconcile_list);

static fsent *symlink_resolve(fstree * restrict fst, fsent * restrict fse)
{
  char path[PATH_MAX];
  uint16_t path_len;
  ssize_t r;
  fsent *this;
  fsent *next;
  moria_vertex *v;
  const char *seg;
  const char *end;
  path_normalize_context ctx;

  if(fst->readlink == READLINK_NO) {
    fse->symlink_target_len = 0;
    filesystem_stats.readlink_no++;
    fst->fstree_stats.readlink_no++;
    fse->symlink_stats.readlink_no++;
    return 0;
  }

  /* writes the absolute path to fse->symlink_target */
  fsent_absolute_path_cat(fse->symlink_target, fse->symlink_target_sz, fse);

  /* writes the link target to path */
  r = readlinkat(INT32_MAX, fse->symlink_target, path, sizeof(path));
  if(r < 1) {
    fse->symlink_target_len = 0;
    filesystem_stats.readlink_err++;
    fst->fstree_stats.readlink_err++;
    fse->symlink_stats.readlink_err++;
    return 0;
  }
  path_len = r;
  path[path_len] = 0;

  /* writes the normalized path to fse->symlink_target
   * because ctx.base is . the output can be relative */
  memset(&ctx, 0, sizeof(ctx));

  r = path_normalize(&ctx, fse->symlink_target, fse->symlink_target_sz, path, path_len);
  fse->symlink_target_len = r;

  if(fse->symlink_target_len > 0 && fse->symlink_target[0] == '/') {
    /* absolute, relative to root */
    this = g_root;
    seg = fse->symlink_target + 1;
  } else {
    /* relative to ref */
    this = fsent_parent(fse);
    seg = fse->symlink_target;
  }

  /* resolve the link target */
  next = 0;
  while(*seg)
  {
    end = seg + 1;
    while(*end && *end != '/') {
      end++;
    }

    if(memncmp(MMS("."), seg, end - seg) == 0) {
      next = this;
    } else if(memncmp(MMS(".."), seg, end - seg) == 0) {
      next = fsent_parent(this);
    } else if((v = moria_vertex_downw(&this->vertex, seg, end - seg))) {
      next = containerof(v, fsent, vertex);
    } else {
      break;
    }
    this = next;

    seg = end;
    if(*seg) {
      seg++;
    }
  }

  /* unresolved link */
  if(*seg) {
    filesystem_stats.readlink_unresolved++;
    fst->fstree_stats.readlink_unresolved++;
    fse->symlink_stats.readlink_unresolved++;
    return 0;
  }

  filesystem_stats.readlink_ok++;
  fst->fstree_stats.readlink_ok++;
  fse->symlink_stats.readlink_ok++;

  return next;
}

static void symlink_error(fsent * restrict fse)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;
  char path[PATH_MAX];

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  /* linkpath */
  fsent_absolute_path_cat(path, sizeof(path), fse);

  { MAPPING
    STRING("symlink resolution failure")
    { SET
      { MAPPING STRING("path")
        STRING(path)
      }
      { MAPPING STRING("target")
        BYTES(fse->symlink_target, fse->symlink_target_len)
      }
      { MAPPING STRING("error")
        STRING(strerror(errno))
      }
    }
  }

  bacon_writer_close(&writer);
  msg->code = EPERM;
  msg->len = fixed.l;
  events_publish(&ectx);
}

/* try to destroy a vertex ; it may not complete, if the node has edges. But at least
 * affirmatively delete its upwards identity edge */
static void edge_destroy(struct moria_edge * restrict edge)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  graph_invalidation_context invalidation = { };

  vertices_init(&unlinking);
  vertices_init(&releasing);
  edges_init(&edges);

  edges_push(&edges, edge);

  graph_invalidation_begin(&invalidation);
  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
  graph_invalidation_end(&invalidation);
}

ref_edge *fsent_symlink_connect(fsent * restrict ref, fsent * restrict link)
{
  struct ref_edge *edge;

  link->vertex.attrs |= MORIA_VERTEX_LINK;
  link->vertex.ref = &ref->vertex;
  edge = ref_connect(&ref->vertex, &link->vertex, EDGE_REF_SYMLINK);

  return edge;
}

static void fsent_symlink_resolve(fsent * restrict link)
{
  fsent *ref;
  fstree *fst;
  struct moria_edge *prev, *next;
  struct ref_edge *edge;

  /* previous edge if any */
  prev = edges_check_down(&link->vertex, EDGE_REF_SYMLINK);

  /* resolve */
  fst = vertex_fstree_of(&link->vertex);
  ref = symlink_resolve(fst, link);

  if(ref) {
    edge = fsent_symlink_connect(ref, link);
    next = &edge->edge;
  } else {
    link->vertex.attrs &= ~MORIA_VERTEX_LINK;
    next = 0;
  }

  /* clean up the previous edge */
  if(prev && prev != next) {
    edge_destroy(prev);
  }

  if(fst->readlink == READLINK_REQUIRED && next == 0) {
    filesystem_stats.readlink_failed++;
    fst->fstree_stats.readlink_failed++;
    link->symlink_stats.readlink_failed++;

    symlink_error(link);
  }
}

void symlink_system_reconcile()
{
  fsent *fse;

  while((fse = llist_shift(&symlink_reconcile_list, typeof(*fse), fs_readlink_lln))) {
    switch(vertex_type_get(&fse->vertex)) {
      case VERTEX_SYMLINK:
        fsent_symlink_resolve(fse);
        break;
      default:
        RTA;
    }

    if(system_has_error) {
      llist_unshift(&symlink_reconcile_list, &fse->fs_readlink_lln);
      break;
    }

    vertex_mark_fresh(&fse->vertex);
  }
}

void symlink_readlink_push(struct fsent * restrict fse)
{
  switch(vertex_type_get(&fse->vertex)) {
    case VERTEX_SYMLINK:
      break;
    default:
      RTA;
  }

  llist_delete(&fse->fs_readlink_lln);
  llist_push(&symlink_reconcile_list, &fse->fs_readlink_lln);
}

void symlink_readlink_cancel(struct fsent * restrict fse)
{
  llist_delete(&fse->fs_readlink_lln);
}
