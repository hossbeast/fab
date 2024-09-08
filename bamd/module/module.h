/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

A module consists of a set of rules which generate the dependencies among files in a region of the
filesystem

*/

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"
#include "descriptor.h"

#include "graph.h"
#include "module_system.h"
#include "vertex.h"
#include "descriptor.h"
#include "stats.h"

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

extern llist module_list;           // active modules
extern struct moria_vertex *g_module;

extern descriptor_type descriptor_stats_module;

/* module vertex stats */
typedef union stats_module {
  struct {
    /* event counters */
    uint16_t parse;          /* parse attempted */
    uint16_t parse_ok;       /* parsed successfully */
    uint16_t reloaded;       /* module was reloaded */

    /* relation/entity counts */
    uint32_t rules;          /* number of rules in this file */
    uint32_t uses_down;      /* rulesets in effect */
    uint32_t requires_up;
    uint32_t requires_down;
    uint32_t modules_up;
    uint32_t modules_down;
  };

  stats_record record;
} stats_module;

/* VERTEX_MODULE */
typedef struct module {
  moria_vertex vertex;

  llist lln;                // adhoc
  rules_container invalid;  // in rebuild lists

  /* runtime state */
  stats_vertex stats;
  stats_module module_stats;
  char id[40];                     // hash by which the module is accessed at //modules/id
  uint16_t id_len;

  bool in_tree;                    // whether dir_node is a descendant of projdir
  bool anchor_disjoint;
  llist blocks;                    // struct block
  struct anchor *anchor;
  struct bacon_value *vars;        // effective vars (result of merges)
  struct exec *exec;               // pre-rendered into envs strings for exec
  llist varfiles;                  // struct pattern
  llist rulesfiles;                // struct pattern

  /* module state exported to shadow-fs */
  struct fsent *shadow;                //modules/id/
  struct fsent *shadow_requires;       //modules/id/requires/
  struct fsent *shadow_rulesets;       //modules/id/rulesets/
  struct fsent *shadow_varsets;        //modules/id/varsets/
  struct fsent *shadow_rules;          //modules/id/rules/
  struct fsent *shadow_rules_by;       //modules/id/rules-by/
  struct fsent *shadow_rules_byname;   //modules/id/rules-by/tag
  struct fsent *shadow_rmas;           //modules/id/rmas/
  struct fsent *shadow_rmas_by;        //modules/id/rmas-by/
  struct fsent *shadow_rmas_byname;    //modules/id/rmas-by/tag

  /* runtime state */
  struct fsent *shadow_vars;           /* linked @ //modules/id/vars */
  struct fsent *shadow_envs;           /* linked @ //modules/id/envs */

  /* self_node */
  struct fsent *self_node;             // VERTEX_MODULE_FILE module.bam
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;

  /* dir_node */
  struct fsent *dir_node;              // module base directory node
  char dir_node_abspath[PATH_MAX];
  uint16_t dir_node_abspath_len;

  /* anchor_node */
  struct fsent *anchor_node;           // module anchor directory node
  char anchor_node_abspath[PATH_MAX];
  uint16_t anchor_node_abspath_len;
  int anchornode_fd;                   // readonly O_PATH | O_DIRECTORY file descriptor
} module;

void module_setup(void);
void module_cleanup(void);

void module_alloc(module ** restrict mod)
  __attribute__((nonnull));

void module_rebuild_enqueue(module * restrict mod)
  __attribute__((nonnull));

void module_dispose(module * restrict mod)
  __attribute__((nonnull));

void module_release(module * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_reparse(module * restrict mod)
  __attribute__((nonnull));

void module_reload(module * restrict mod)
  __attribute__((nonnull));

void module_excise(module * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_disconnect_rules(module * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* create/refresh EDGE_RULES_MOD edges
 *
 * returns true if any such edges were newly created
 */
bool module_reimport(module * restrict mod, struct rule_run_context * restrict rule_run_ctx)
  __attribute__((nonnull));

void module_prune(module * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_rebuild(module * restrict mod, struct rule_run_context * restrict rule_run_ctx)// struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_bootstrap(
    struct fsent * mod_dir_n
  , struct fsent * mod_anchor_n
  , struct fsent * restrict mod_file_n
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 2, 4)));

void module_bootstrap_visitor(moria_vertex * mod_dir_v, void * _ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull(1)));

void module_bootstrap_anchored_visitor(moria_vertex * mod_dir_v, void * _ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull(1)));

void module_initialize(struct fsent * mod_dir_n, struct fsent * mod_anchor_n, struct fsent * restrict mod_file_n)
  __attribute__((nonnull(1, 2)));

bool module_import_rules_file(
    module * restrict A
  , struct fsent * restrict rules_file_n
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

void module_identity_say(const struct module * restrict module, struct narrator * restrict N)
  __attribute__((nonnull));

void module_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void module_dir_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void module_stats_build(module * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void module_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void module_dir_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));
