/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "graph.h"
#include "moria/vertex.h"

struct graph_invalidation_context;
enum moria_traversal_mode;
enum bam_fsent_state;

extern uint64_t vertex_token_counter;

/* invalidate a vertex and its transitive consumers (upwards) */
void vertex_invalidate(moria_vertex * restrict n, struct graph_invalidation_context * restrict)
  __attribute__((nonnull));

void vertex_invalidate_visitor(moria_vertex * restrict v, void * arg, enum moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
  __attribute__((nonnull(1, 2)));

void vertex_invalidate_visitor_first(moria_vertex * restrict v, void * arg, enum moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
  __attribute__((nonnull(1, 2)));

void vertex_invalidate_direct(moria_vertex * restrict n, struct graph_invalidation_context * restrict)
  __attribute__((nonnull));

/* mark single vertex as ok / up-to-date / fresh */
bool vertex_mark_fresh(moria_vertex * restrict v)
  __attribute__((nonnull));

/* mark single vertex as invalid / out-of-date / stale */
bool vertex_mark_stale(moria_vertex * restrict v)
  __attribute__((nonnull));

/*
 *
 */

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

static inline enum vertex_shadowtype vertex_shadowtype_get(const moria_vertex * restrict v)
{
  uint32_t attrs = v->attrs;
  attrs &= VERTEX_SHADOWTYPE_OPT;
  return attrs;
}

static inline void vertex_shadowtype_set(moria_vertex * restrict v, enum vertex_shadowtype shadowtype)
{
  uint32_t attrs;

  RUNTIME_ASSERT(shadowtype & VERTEX_SHADOWTYPE_OPT);

  attrs = v->attrs;
  attrs &= ~VERTEX_SHADOWTYPE_OPT;
  attrs |= shadowtype;

  vertex_attrs_set(v, attrs);
}

void vertex_state_set(moria_vertex * restrict v, vertex_state state)
  __attribute__((nonnull));

enum bam_vertex_state vertex_bam_vertex_state_get(moria_vertex * restrict v)
  __attribute__((nonnull));

static inline vertex_state vertex_state_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_STATE_OPT;

  return attrs;
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

bool vertex_invalid_get(moria_vertex * restrict v)
  __attribute__((nonnull));

static inline bool vertex_protect_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_PROTECT_BIT;

  return !!attrs;
}

static inline void vertex_protect_set(moria_vertex * restrict v, bool b)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_PROTECT_BIT;
  if(b) {
    attrs |= VERTEX_PROTECT_BIT;
  }

  vertex_attrs_set(v, attrs);
}

static inline bool vertex_shield_get(const moria_vertex * restrict v)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= VERTEX_SHIELD_BIT;

  return !!attrs;
}

static inline void vertex_shield_set(moria_vertex * restrict v, bool b)
{
  uint32_t attrs;

  attrs = v->attrs;
  attrs &= ~VERTEX_SHIELD_BIT;
  if(b) {
    attrs |= VERTEX_SHIELD_BIT;
  }

  vertex_attrs_set(v, attrs);
}

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

size_t vertex_path_load(void * restrict dst, size_t dst_size, const moria_vertex * v)
  __attribute__((nonnull));

size_t vertex_id_load(void * restrict dst, size_t dst_size, const moria_vertex * v)
  __attribute__((nonnull));

size_t vertex_token_load(void * restrict dst, size_t dst_size, const moria_vertex * v)
  __attribute__((nonnull));

size_t vertex_stats_collate(void *dst, size_t sz, bool reset, moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_identity_say(const moria_vertex * restrict vertex, struct narrator * restrict N)
  __attribute__((nonnull));

size_t vertex_identity_cat(void * restrict dst, size_t dst_size, const moria_vertex * restrict vertex)
  __attribute__((nonnull));

void vertex_identity_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_identity_describe(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void vertex_describe(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void vertex_describe_edges(const moria_vertex * restrict v, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

size_t vertex_cat(void * restrict dst, size_t dst_size, const moria_vertex * restrict vertex)
  __attribute__((nonnull));

void vertex_say(const moria_vertex * restrict e, struct narrator * restrict N)
  __attribute__((nonnull));

void vertex_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

void vertex_edges_show(const moria_vertex * restrict v)
  __attribute__((nonnull));

uint64_t vertex_compute_hash(moria_vertex * restrict v, uint64_t u64)
  __attribute__((nonnull));
