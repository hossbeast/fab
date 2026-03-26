/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

A module consists of a set of rules which generate the dependencies among files in a region of the
filesystem

*/

#include "types.h"

#include "valyria/llist.h"
#include "moria.h"
#include "descriptor.h"

#include "graph.h"
#include "module_system.h"
#include "vertex.h"
#include "descriptor.h"
#include "stats.h"
#include "identity.h"

struct map;
struct fsent;
struct bacon_value;
struct graph_invalidation_context;
struct fsedge;
struct rules_parser;
struct narrator;
struct rule_run_context;
struct exec;
struct anchor;
struct ruleset;
struct graph_disintegrate_context;
struct lookup_pattern;

extern llist module_list;           // active modules
extern struct moria_vertex *g_module;

extern descriptor_type descriptor_stats_module;

/* module vertex stats */
typedef union stats_module {
  struct {
    /* event counters */
    uint16_t reloaded;         /* module was reloaded */

    uint32_t unlink_no;        /* unlink not permitted by fstree */
    uint32_t unlink_ok;        /* unlink() returned zero */
    uint32_t unlink_enoent;    /* unlink(), ENOENT */
    uint32_t unlink_err;       /* unlink(), other nonzero error */
    uint32_t rmdir_no;         /* rmdir not permitted by fstree */
    uint32_t rmdir_ok;         /* rmdir() returned zero */
    uint32_t rmdir_enoent;     /* rmdir(), ENOENT */
    uint32_t rmdir_err;        /* rmdir(), other nonzero error */

    /* relation/entity counts */
    uint32_t rmas;
    uint32_t rulesets;
    uint32_t varsets;
    uint32_t fsnames;
  };

  stats_record record;
} stats_module;

/* VERTEX_MODULE */
typedef struct module {
  moria_vertex vertex;

  llist lln;                // adhoc
  llist invalid_lln;        // in rebuild lists

  /* runtime state */
  stats_vertex stats;
  stats_module module_stats;
  char id[40];                     // hash by which the module is accessed at //modules/id
  uint16_t id_len;
  char name[256];
  uint16_t name_len;
  uint16_t epoch;

  struct bacon_value *vars;        // effective vars (result of merges, and sysvars)
  bool disjoint;                   // true if the build dir does not descend from the anchor dir
  bool managed;                    // whether the build dir should be managed
  bool anchor_inherited;           // true when loaded from module_bootstrap_anchored_visitor

  /* module state exported to shadow-fs */
  struct fsent *shadow;                //modules/id/
  struct fsent *shadow_rulesets;       //modules/id/rulesets/
  struct fsent *shadow_varsets;        //modules/id/varsets/
  struct fsent *shadow_rules;          //modules/id/rules/
  struct fsent *shadow_rules_by;       //modules/id/rules-by/
  struct fsent *shadow_rules_byname;   //modules/id/rules-by/tag
  struct fsent *shadow_rmas;           //modules/id/rmas/
  struct fsent *shadow_rmas_by;        //modules/id/rmas-by/
  struct fsent *shadow_rmas_byname;    //modules/id/rmas-by/tag

  /* build dir */
  struct fsent *build_dir;             // build directory
  char build_dir_abspath[PATH_MAX];
  uint16_t build_dir_abspath_len;

  /* anchor dir */
  struct fsent *anchor_dir;            // anchor directory
  char anchor_dir_abspath[PATH_MAX];
  uint16_t anchor_dir_abspath_len;
} module;

void module_setup(void);
void module_cleanup(void);

void module_alloc(module ** restrict mod)
  __attribute__((nonnull));

void module_rebuild_enqueue(module * restrict mod)
  __attribute__((nonnull));

void module_release(module * restrict mod)//, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_reparse(module * restrict mod)
  __attribute__((nonnull));

void module_reload(module * restrict mod, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* create/refresh EDGE_RULES_MOD edges
 *
 * returns true if any such edges were newly created
 */
bool module_reimport(module * restrict mod, struct rule_run_context * restrict rule_run_ctx)
  __attribute__((nonnull));

void module_prune(module * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_rebuild(module * restrict mod, struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void module_remake(module * restrict mod, struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

struct module_bootstrap_context {
  module *mod;
  graph_invalidation_context *invalidation;
};

void module_bootstrap(
    struct fsent * build_dir
  , struct fsent * anchor_dir
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 3)));

void module_bootstrap_anchor_visitor(moria_vertex * mod_dir_v, void *ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull(1, 2)));

void module_bootstrap_visitor(moria_vertex * mod_dir_v, void *ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull(1, 2)));

void module_bootstrap_anchored_visitor(moria_vertex * mod_dir_v, void *ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull(1, 2)));

void module_initialize(struct module * restrict mod, struct fsent *build_dir)
  __attribute__((nonnull));

bool module_import_rules_file(
    struct fsent * restrict rules_file_n
  , module * restrict mod
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void module_reload_blocks(
    module * restrict mod
  , struct ruleset * restrict rules
  , llist * restrict blocks
  , graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 3, 4)));

void module_describe(const module * restrict mod, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void module_dir_describe(const struct fsent * restrict dir, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void module_file_describe(const struct fsent * restrict dir, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t module_path_cat(void * restrict dst, size_t sz, const struct module * restrict module)
  __attribute__((nonnull));

void module_identity_describe(const struct module * restrict module, struct bacon_writer * restrict)
  __attribute__((nonnull));

void module_stats_build(module * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void module_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void module_dir_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void modules_show(void);

void module_unlink(module * restrict mod, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void module_hash(const module * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void module_id(const module * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void module_compute_id(const struct module * restrict module, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void modules_invalidate_for_specials(struct fsent * restrict varfile, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

bool module_reap(struct module * restrict mod, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));
