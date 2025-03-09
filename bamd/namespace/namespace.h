/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "stats.h"
#include "descriptor.h"
#include "moria.h"
#include "vertex.h"

struct bacon_value;
struct bacon_writer;
struct channel;
struct config;
struct fsedge;
struct fsent;
struct fsname;
struct graph_invalidation_context;
struct lookup_context;
struct lookup_pattern;
struct pattern;
struct yyu_location;

extern llist namespace_list;

typedef union stats_namespace {
  struct {
      uint32_t count;   // number of indexed directories
      uint32_t put;     // counter ; directory added to the index
      uint32_t del;     // counter ; directory removed from the index
  };

  stats_record record;
} stats_namespace;

extern descriptor_type descriptor_stats_namespace;

/* VERTEX_NAMESPACE */
typedef struct namespace {
  moria_vertex vertex;

  stats_vertex stats;
  stats_namespace namespace_stats;
  char id[40];
  uint16_t id_len;
  uint16_t epoch;

  char name[255];
  uint16_t name_len;

  struct fsent *shadow;                //namespaces/id/
  struct fsent *shadow_fsnames;        //namespaces/id/fsnames/
  struct fsent *shadow_fsnames_by;     //namespaces/id/fsnames-by/
  struct fsent *shadow_fsnames_byname; //namespaces/id/fsnames-by/name/
} namespace;

/* default namespaces */
extern namespace *namespace_modules;
extern namespace *namespace_formulas;
extern namespace *namespace_plugins;

void namespace_setup(void);
void namespace_cleanup(void);

void namespace_reconfigure(struct config *cfg, bool dryrun, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void namespace_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* lookup a namespace by name, returns NULL if no such namespace exists */
namespace *namespace_lookup(const char * restrict name, uint16_t name_len)
  __attribute__((nonnull));

/* lookup a namespace by name, create it if it does not exist */
namespace *namespace_get(const char * restrict name, uint16_t name_len, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* index a non-shadow directory node for graph lookup by label */
void namespace_put(
    namespace * restrict ns
  , struct fsent * restrict n
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

/*
 * evaluate a lookup pattern against the namespace in cxt->ns
 *
 * returns 0, 1, or 2, indicating the number of nodes reached
 */
int namespace_resolve(struct lookup_context *ctx, struct lookup_pattern * restrict pattern)
  __attribute__((nonnull));

//
//
//

void namespace_show(const namespace *ns)
  __attribute__((nonnull));

void namespace_describe(const namespace *ns, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void namespace_identity_describe(const namespace * restrict ns, struct bacon_writer * restrict)
  __attribute__((nonnull));

void namespace_stats_build(const namespace *ns, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t namespace_path_cat(void * restrict dst, size_t sz, const namespace * restrict ns)
  __attribute__((nonnull));

void namespace_release(namespace * restrict ns)
  __attribute__((nonnull));

void namespace_id(const namespace * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void namespace_compute_id(const namespace * restrict ns, struct identity_ctx * restrict id)
  __attribute__((nonnull));
