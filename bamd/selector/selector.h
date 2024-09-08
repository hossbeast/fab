/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria/traverse.h"
#include "valyria/llist.h"

#include "graph.h"
#include "selection.h"
#include "entlist.h"

struct attrs32;
struct channel;
struct buildplan_entity;
struct module;
struct narrator;
struct pattern;
struct bacon_writer;
struct fsent;

#define SELECTOR_TRAVERSE_DEFAULT_MODE          MORIA_TRAVERSE_PRE
#define SELECTOR_TRAVERSE_DEFAULT_DIRECTION     MORIA_TRAVERSE_DOWN
#define SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH     1
#define SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH     UINT16_MAX
#define SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE  true
#define SELECTOR_TRAVERSE_DEFAULT_HIDDEN        false
#define SELECTOR_TRAVERSE_DEFAULT_NOFOLLOW      false

#define SELECTOR_TYPE_OPT 0xf
#define SELECTOR_TYPE_TABLE                                              \
  /* aggregates */                                                       \
  SELECTOR_DEF(SELECTOR_SEQUENCE  , "sequence" , SELECTOR_TYPE_OPT, 0x1) \
  SELECTOR_DEF(SELECTOR_UNION     , "union"    , SELECTOR_TYPE_OPT, 0x2) \
  /* operations */                                                       \
  SELECTOR_DEF(SELECTOR_NODESET   , "nodeset"  , SELECTOR_TYPE_OPT, 0x3) \
  SELECTOR_DEF(SELECTOR_PATTERN   , "pattern"  , SELECTOR_TYPE_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_LOOKUP    , "lookup"   , SELECTOR_TYPE_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_TRAVERSE  , "traverse" , SELECTOR_TYPE_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_MODULE_OF , "module-of", SELECTOR_TYPE_OPT, 0x7) \
  SELECTOR_DEF(SELECTOR_FSTREE_OF , "fstree-of", SELECTOR_TYPE_OPT, 0x8) \

#define SELECTOR_NODESET_OPT 0xf
#define SELECTOR_NODESET_TABLE                                                   \
  SELECTOR_DEF(SELECTOR_NODESET_TARGET  , "target"  , SELECTOR_NODESET_OPT, 0x1) \
  SELECTOR_DEF(SELECTOR_NODESET_TARGETS , "targets" , SELECTOR_NODESET_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCE  , "source"  , SELECTOR_NODESET_OPT, 0x3) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCES , "sources" , SELECTOR_NODESET_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_NODESET_MODULE  , "module"  , SELECTOR_NODESET_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_NODESET_ANCHOR  , "anchor"  , SELECTOR_NODESET_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_NODESET_ALL     , "all"     , SELECTOR_NODESET_OPT, 0x7) \
  SELECTOR_DEF(SELECTOR_NODESET_NONE    , "none"    , SELECTOR_NODESET_OPT, 0x8) \

typedef enum selector_type {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_TYPE_TABLE
#undef SELECTOR_DEF
} selector_type;

extern struct attrs32 * selector_type_attrs;

typedef enum selector_nodeset {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_NODESET_TABLE
#undef SELECTOR_DEF
} selector_nodeset;

extern struct attrs32 * selector_nodeset_attrs;

struct selector;
typedef struct selector {
  selector_type type;

  union {
    llist head;                                   // SELECTOR_AGGREGATE
    selector_nodeset nodeset;                     // SELECTOR_NODESET
    struct pattern *pattern;                      // SELECTOR_PATTERN
    struct {                                      // SELECTOR_LOOKUP
      char * path;
      uint16_t path_len;
      bool nofollow;              // dont follow symlinks
      bool hidden;                // allow !VERTEX_EXISTS_BIT
    } lookup;
    struct selector_traverse {                    // SELECTOR_TRAVERSE
      moria_traversal_direction direction;
      moria_traversal_mode mode;
      moria_traversal_criteria criteria;
      bool nofollow;             // MORIA_TRAVERSE_NOFOLLOW
      bool exhaustive;           // MORIA_TRAVERSE_EXHAUSTIVE
      bool cross_module;
      bool hidden;               // allow !VERTEX_EXISTS_BIT
      uint32_t visit_mask;       // limit visit by vertex attrs
      uint32_t visit_bits;
    } traverse;
  };

  llist lln;
} selector;

void selector_free(selector * restrict s);
void selector_ifree(selector ** restrict s)
  __attribute__((nonnull));

void selector_alloc(selector_type type, selector ** restrict s)
  __attribute__((nonnull));

typedef struct selector_context {
  /* inputs */
  struct buildplan_entity *bpe;      // set when run in the context of a build slot
  const struct module *mod;
  const struct fsent *cwd;
  selection_iteration_type iteration_type;

  // mutable
  selector * sel;

  // results
  entlist lookup_hits;
  struct selection * selection; // the active selection
  struct channel * chan;
} selector_context;

void selector_context_init(selector_context *ctx)
  __attribute__((nonnull));

void selector_context_destroy(selector_context *ctx)
  __attribute__((nonnull));

/*
 * execute a selector against the graph
 */
void selector_exec(selector * restrict s, selector_context * restrict ctx)
  __attribute__((nonnull));

void selector_say(selector * restrict s, struct narrator * const restrict N)
  __attribute__((nonnull));

void selector_writer_write(selector * const restrict sel, struct bacon_writer * const restrict writer)
  __attribute__((nonnull));

void selector_graph_fs(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_depends(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_special(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_special(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_uses(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_requires(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_module(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_ruleset(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_formula(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_varset(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_configset(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_special_plugin(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_module_dir(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_directory(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_rule(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_fs(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_rma(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_bpe(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));
