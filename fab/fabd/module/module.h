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

#ifndef FABD_MODULE_H
#define FABD_MODULE_H

/*

SUMMARY
 A module is the smallest component which fab can build.

*/

#include "types.h"
#include "xapi.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"

struct array;
struct map;
struct node;
struct pattern;
struct set;
struct value;
struct graph_invalidation_context;
struct rule;

/* edge connecting a rule to a module */
typedef struct rule_module_association {
  llist lln_owner;           // in module->rmas_owner, or freelist
  rbnode rbn_rmas;           // in module->rmas
  rbnode nohits_rbn;         // in rule_run_context->nohits
  struct edge * associating_edge; //
  struct module * mod_owner; // module which defines this rule

  struct rule * rule;        // rule to run
  struct module * mod;       // module to run in context of
  struct set * variants;     // variants for rule execution

  /* considered during a refresh period */
  uint32_t refresh_id;
  union {
    llist refresh_lln;
    llist lln;
  };

  llist edges;              // struct node_edge_depends - created by this rma

  struct {
    llist lln;              // in module_rma_list
    uint32_t refresh_round_id;
  } changed[2];
} rule_module_association;

typedef struct statement_block {
  llist lln;
  struct set * variants;     // struct variant *; variants in the block
  llist rules;               // struct rule
} statement_block;

typedef struct module {
  llist lln_modules;      // in g_modules
  llist lln_invalidated;  // in modules_invalidated

  /* runtime state */
  char id[16];                    // hash by which the module is indexed in the shadow fs
  struct node * self_node;        // file node for module.bam
  char * self_node_relpath;
  struct node * dir_node;         // directory node for the module
  char * dir_node_abspath;
  struct node * shadow;           // shadow dirnode //modules/id/
  struct node * shadow_imports;   // shadow dirnode //modules/id/imports/
  struct node * shadow_targets;   // shadow dirnode //modules/id/targets
  struct node * shadow_scope;     // shadow dirnode //modules/id/scope/
  bool leaf;                      // true if there do not exist more specific modules under this path
  int dirnode_fd;                 // open file descriptor to the module directory node

  llist rmas_owner;               // rmas for rules owned/defined by this module
//  rbtree rmas;                    // rmas for rules associated to this module

  /*
   * values in this module are owned by the value_parser : variant_var, novariant_var
   */
  struct value_parser *value_parser;

  /* from here down - rebuilt each time the module is refreshed */
  struct map * variant_var;       // map<struct variant, value*>
  struct value * novariant_var;
  struct map * variant_envs;      // map<struct variant*, exec*>
  struct exec * novariant_envs;

  /* parse state */
  statement_block * unscoped_block;
  llist scoped_blocks;
  struct value * var_value;     // var : { } block in module.bam
  bool var_merge_overwrite;

  llist lln;                    // adhoc
} module;

// loaded modules
extern llist g_modules;

xapi module_setup(void);
xapi module_cleanup(void);

/// module_load_project
//
// SUMMARY
//  load the module in the project directory, and nested modules, recursively
//
// PARAMETERS
//  project_root - base node for the starting module
//  project_dir  - absolute path for the
//  rule_ctx     - tracks which rules have hit
//
xapi module_load_project(struct node * restrict root, const char * restrict absdir, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

xapi module_full_refresh(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_invalidated(module * mod)
  __attribute__((nonnull));

/// module_lookup
//
// SUMMARY
//  get the module for the specified path, if any
//
// PARAMETERS
//  path - normalized absolute path
//  mod  - (returns) owning module, if any
//
module * module_lookup(const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

xapi module_rule_associate(
    module * mod
  , module * mod_owner
  , struct rule * restrict rule
  , struct set * restrict variants
  , rule_module_association ** restrict rma
)
  __attribute__((nonnull(1, 2, 3, 5)));

#endif
