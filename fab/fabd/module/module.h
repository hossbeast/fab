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

A module consists of a set of rules which generate the dependencies among files in a region of the
filesystem

*/

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"
#include "moria.h"
#include "fab/stats.h"

#include "graph.h"

struct map;
struct fsent;
struct set;
struct value;
struct graph_invalidation_context;
struct fsedge;
struct module_parser;
struct narrator;

extern uint16_t module_system_reconcile_epoch;
extern llist module_list;    // loaded/active modules

typedef struct statement_block {
  llist lln;
  struct set * variants;     // struct variant *; variants in the block
  llist rules;               // struct rule
} statement_block;

/* VERTEX_TYPE_MODULE */
typedef struct module {
  moria_vertex vertex;

  llist lln_reconcile;    // in module_system_reconcile
  llist rmas_owner;       // rmas for rules in this modules statement blocks

  /* runtime state */
  fab_module_stats stats;

  struct fsent * self_node;        // module.bam
  struct fsent * dir_node;         // module directory node
  int dirnode_fd;                 // open file descriptor to the module directory node

  /* rules */
  statement_block * unscoped_block;
  llist scoped_blocks;

  /* var : { } block in module.bam */
  struct value * var_value;
  bool var_merge_overwrite;
  struct value * novariant_var;
  struct exec * novariant_envs;

  /* adhoc */
  llist lln;

  struct fsent * shadow;               // shadow dirnode //modules/id/
  struct fsent * shadow_targets;       // shadow dirnode //modules/id/targets

  /* these exist for rules to match against */
  struct fsedge * shadow_fs;           // shadow link    //modules/id/fs
  struct fsent * shadow_imports;       // shadow dirnode //modules/id/imports/
  struct fsent * shadow_requires;      // shadow dirnode //modules/id/requires/
  struct fsent * shadow_uses;          // shadow dirnode //modules/id/uses/

  char id[16];                         // hash by which the module is accessed at //modules/id
  char self_node_relpath[512];
  uint16_t self_node_relpath_len;

  char dir_node_abspath[512];
  uint16_t dir_node_abspath_len;

  /*
   * values in this module are owned by the value_parser : variant_var, novariant_var
   */
  struct value_parser *value_parser;
  struct map * variant_var;       // map<struct variant*, value*>
  struct map * variant_envs;      // map<struct variant*, exec*>
} module;

/* EDGE_TYPE_MODULES - edge connecting two module vertices */
typedef struct module_edge {
  struct moria_edge edge;
  uint16_t module_system_reconcile_epoch;

  /* related edges, e.g. //module/requires, //module/uses */
  struct fsedge *edges[2];
} module_edge;

xapi module_setup(void);
xapi module_cleanup(void);

xapi module_edge_alloc(module_edge ** restrict mep)
  __attribute__((nonnull));

/*
 * load the module in the project directory, and nested modules, recursively
 *
 * project_root - base node for the starting module
 * project_dir  - absolute path for the starting module directory
 */
xapi module_system_bootstrap(void);//struct fsent * restrict root, const char * restrict absdir)

/*
 * reload invalidated modules
 */
xapi module_system_reconcile(void);

/*
 * get the module for the specified path, if any
 *
 * path - normalized absolute path
 * mod  - (returns) owning module, if any
 */
module * module_lookup(const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

/*
 * Write a modules stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
xapi module_collate_stats(void *dst, size_t sz, module *mod, bool reset, size_t *zp)
  __attribute__((nonnull));

xapi module_file_collate_stats(void *dst, size_t sz, struct fsent *modfile, bool reset, size_t *zp)
  __attribute__((nonnull));

xapi module_alloc(module ** restrict mod)
  __attribute__((nonnull));

xapi module_xrelease(module * restrict mod, struct module_parser * restrict parser)
  __attribute__((nonnull));

xapi module_resolve_require(module * restrict A, module * restrict B, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

xapi module_resolve_use(module * restrict A, module * restrict B, const char * restrict refname, uint16_t refname_len, bool scoped, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

xapi module_resolve_import(module * restrict A, struct fsent * restrict B, const char * restrict refname, uint16_t refname_len, bool scoped, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

xapi module_vars_say(module * restrict mod, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
