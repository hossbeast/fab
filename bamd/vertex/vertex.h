/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria/vertex.h"

#include "types.h"
#include "graph.h"
#include "descriptor.h"
#include "stats.h"

struct graph_invalidation_context;
struct graph_disintegrate_context;
struct bacon_writer;
enum moria_traversal_mode;
struct channel;
struct module;
struct identity_ctx;

typedef union stats_vertex {
  struct {
    /* event counters */
    uint32_t stale;         /* node marked as stale */
    uint32_t fresh;         /* node marked as fresh */

    uint8_t indexed_notify; /* whether this node is indexed for lookup by notify descriptor */
  };

  stats_record record;
} stats_vertex;

extern descriptor_type descriptor_stats_vertex;

/* invalidate a vertex and its transitive consumers (upwards) */
void vertex_invalidate(moria_vertex * restrict n, struct graph_invalidation_context * restrict)
  __attribute__((nonnull));

void vertex_invalidate_visitor(
    moria_vertex * restrict v
  , void * arg
  , enum moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
  __attribute__((nonnull(1, 2)));

void vertex_invalidate_visitor_first(
    moria_vertex * restrict v
  , void * arg
  , enum moria_traversal_mode mode
  , int distance, moria_traverse_result * restrict result
)
  __attribute__((nonnull(1, 2)));

void vertex_invalidate_direct(moria_vertex * restrict n, struct graph_invalidation_context * restrict)
  __attribute__((nonnull));

/* mark single vertex as ok / up-to-date / fresh */
bool vertex_mark_fresh(moria_vertex * restrict v)
  __attribute__((nonnull));

/* mark single vertex as invalid / out-of-date / stale */
bool vertex_mark_stale(moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_attrs_set(moria_vertex * restrict v, uint32_t attrs)
  __attribute__((nonnull));

static inline enum vertex_kind vertex_kind_get(const moria_vertex * restrict v)
{
  uint32_t attrs = v->attrs;
  attrs &= VERTEX_KIND_OPT;
  return attrs;
}

static inline void vertex_kind_set(moria_vertex * restrict v, enum vertex_kind kind)
{
  uint32_t attrs;

  RUNTIME_ASSERT(kind & VERTEX_KIND_OPT);

  attrs = v->attrs;
  attrs &= ~VERTEX_KIND_OPT;
  attrs |= kind;

  vertex_attrs_set(v, attrs);
}

static inline enum vertex_filetype vertex_filetype_get(const moria_vertex * restrict v)
{
  uint32_t attrs = v->attrs;
  attrs &= VERTEX_FILETYPE_OPT;
  return attrs;
}

static inline void vertex_filetype_set(moria_vertex * restrict v, enum vertex_filetype filetype)
{
  uint32_t attrs;

  RUNTIME_ASSERT(filetype & VERTEX_FILETYPE_OPT);

  attrs = v->attrs;
  attrs &= ~VERTEX_FILETYPE_OPT;
  attrs |= filetype;

  vertex_attrs_set(v, attrs);
}

static inline bool vertex_shadow_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_SHADOW_BIT;

  return !!attrs;
}

static inline void vertex_shadow_set(moria_vertex * restrict v, bool b)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_SHADOW_BIT;
  if(b) {
    attrs |= VERTEX_SHADOW_BIT;
  }

  vertex_attrs_set(v, attrs);
}

void vertex_state_set(moria_vertex * restrict v, vertex_state state)
  __attribute__((nonnull));

enum bam_vertex_state vertex_bam_vertex_state_get(const moria_vertex * restrict v)
  __attribute__((nonnull));

static inline vertex_state vertex_state_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_STATE_OPT;

  return attrs;
}

static inline bool vertex_is_unlinked(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_UNLINKED_BIT;

  return !!attrs;
}

static inline bool vertex_exists_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_EXISTS_BIT;

  return !!attrs;
}

static inline void vertex_exists_set(moria_vertex * restrict v, bool b)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_EXISTS_BIT;
  if(b) {
    attrs |= VERTEX_EXISTS_BIT;
  }

  vertex_attrs_set(v, attrs);
}

void vertex_invalid_set(moria_vertex * restrict v)
  __attribute__((nonnull));

bool vertex_invalid_get(const moria_vertex * restrict v)
  __attribute__((nonnull));

static inline enum vertex_type vertex_type_get(const moria_vertex * restrict v)
{
  uint32_t attrs = v->attrs;
  attrs &= VERTEX_TYPE_OPT;
  return attrs;
}

static inline void vertex_type_set(moria_vertex * restrict v, vertex_type type)
{
  uint32_t attrs;

  RUNTIME_ASSERT(type & VERTEX_TYPE_OPT);

  attrs = v->attrs;
  attrs &= ~VERTEX_TYPE_OPT;
  attrs |= type;

  vertex_attrs_set(v, attrs);
}

static inline enum vertex_special vertex_special_get(const moria_vertex * restrict v)
{
  uint32_t attrs = v->attrs;
  attrs &= VERTEX_SPECIAL_OPT;
  return attrs;
}

static inline void vertex_special_set(moria_vertex * restrict v, enum vertex_special special)
{
  uint32_t attrs;

  RUNTIME_ASSERT(special & VERTEX_SPECIAL_OPT);

  attrs = v->attrs;
  attrs &= ~VERTEX_SPECIAL_OPT;
  attrs |= special;

  vertex_attrs_set(v, attrs);
}

//
// getters
//

/* get the module this fsent vertex is associated with if any */
struct module *vertex_module_of(const moria_vertex * restrict v)
  __attribute__((nonnull));

/* get the fstree this fsent vertex resides within */
struct fstree *vertex_fstree_of(const moria_vertex * restrict v)
  __attribute__((nonnull));

//
// path
//

size_t vertex_path_cat(void * restrict dst, size_t dst_size, const moria_vertex * v)
  __attribute__((nonnull));

void vertex_path_say(const moria_vertex * restrict v, struct narrator * restrict N)
  __attribute__((nonnull));

void vertex_path_describe(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

//
// id
//

size_t vertex_id_cat(void * restrict dst, size_t dst_size, const moria_vertex * v)
  __attribute__((nonnull));

void vertex_id_describe(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void vertex_id(const moria_vertex * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void vertex_compute_id(const moria_vertex * restrict v, struct identity_ctx * restrict id)
  __attribute__((nonnull));

//
// identity
//

size_t vertex_identity_cat(void * restrict dst, size_t dst_size, const moria_vertex * restrict vertex)
  __attribute__((nonnull));

void vertex_identity_say(const moria_vertex * restrict vertex, struct narrator * restrict N)
  __attribute__((nonnull));

void vertex_identity_describe(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void vertex_identity_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_describe_edges(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void vertex_edges_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_identity_edges_list(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void vertex_identity_edges_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_describe_identity_edges(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

uint64_t vertex_compute_hash(moria_vertex * restrict v, uint64_t u64)
  __attribute__((nonnull));

void vertex_tell(const moria_vertex * restrict v, struct request_handler * restrict handler)
  __attribute__((nonnull));

//
// describe
//

void vertex_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void vertex_identity_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void vertex_describe_edges(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void vertex_identity_describe_edges(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void vertex_stats_describe(moria_vertex * restrict v, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

/* vertex describe helper ; list of edges by path, excluding fstree and shadow-ref edges */
void vertex_relations_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

/* vertex describe helper ; standard list of mappings for vertices of all types */
void vertex_mappings(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

//
// ops
//

void vertex_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_release(moria_vertex * restrict v)
  __attribute__((nonnull));

moria_vertex *vertex_follow(const moria_vertex * restrict v);

  __attribute__((nonnull));

void vertex_unlink(moria_vertex * restrict v, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

bool vertex_reap(moria_vertex * restrict v, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void vertex_release(moria_vertex * restrict v)
  __attribute__((nonnull));

//
// attrs
//

void attrs32_say(uint32_t attrs, const struct attrs32 * restrict list, struct narrator * restrict N)
  __attribute__((nonnull));

void attrs32_describe(uint32_t attrs, const struct attrs32 * restrict list, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
