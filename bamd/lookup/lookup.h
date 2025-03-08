/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "resolve.h"

struct fsname;
struct lookup_pattern;
struct bacon_value;
struct bacon_writer;
struct namespace;
struct bacon_location;
struct moria_vertex;
struct fsent;
struct graph_invalidation_context;
struct yyu_location;

typedef struct lookup_context {
  resolve_context rctx;

  /* input */
  const char *ns_name;
  uint16_t ns_name_len;
  const struct yyu_location *loc;

  /* state */
  struct namespace *ns;

  /* returns */
  struct fsname *hit;
  struct fsname *miss;
} lookup_context;

typedef struct lookup_entry {
  rbnode rbn;
  llist lln;
  void *key;
  struct moria_vertex *v;
} lookup_entry;

typedef struct lookup_store {
  rbtree set;
} lookup_store;

void lookup_store_init(lookup_store * restrict)
  __attribute__((nonnull));

void lookup_store_destroy(lookup_store * restrict store)
  __attribute__((nonnull));

void lookup_store_reset(lookup_store * restrict store)
  __attribute__((nonnull));

void lookup_store_put(lookup_store * restrict results, void *key, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

struct moria_vertex *lookup_store_get(lookup_store * restrict results, void *key)
  __attribute__((nonnull));

int lookup_resolve(lookup_context * restrict ctx, struct lookup_pattern * restrict pattern)
  __attribute__((nonnull));

void lookup_namespace_error_build(struct bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
  __attribute__((nonnull));

void lookup_bound_error_build(struct bacon_writer * restrict builder_writer, const struct lookup_pattern * restrict ref)
  __attribute__((nonnull));

void lookup_error_build(struct bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
  __attribute__((nonnull));

void lookup_error_mappings(struct bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
  __attribute__((nonnull));

void lookup_cleanup(void);
