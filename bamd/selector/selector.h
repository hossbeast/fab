/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "moria/traverse.h"
#include "valyria/llist.h"

#include "graph.h"
#include "selection.h"
#include "edges.h"
#include "vertices.h"
#include "lookup.h"
#include "resolve.h"

struct attrs32;
struct channel;
struct buildplan_entity;
struct module;
struct narrator;
struct pattern;
struct bacon_writer;
struct fsent;
struct lookup_pattern;
struct path_pattern;
struct search_pattern;
struct fstree;

#define SELECTOR_TRAVERSE_DEFAULT_MODE          MORIA_TRAVERSE_PRE
#define SELECTOR_TRAVERSE_DEFAULT_DIRECTION     MORIA_TRAVERSE_DOWN
#define SELECTOR_TRAVERSE_DEFAULT_MIN_DEPTH     1
#define SELECTOR_TRAVERSE_DEFAULT_MAX_DEPTH     UINT16_MAX
#define SELECTOR_TRAVERSE_DEFAULT_CROSS_MODULE  true
#define SELECTOR_TRAVERSE_DEFAULT_CROSS_FSTREE  true
#define SELECTOR_TRAVERSE_DEFAULT_HIDDEN        false
#define SELECTOR_TRAVERSE_DEFAULT_NOFOLLOW      false

#define SELECTOR_TYPE_OPT 0xf
#define SELECTOR_TYPE_TABLE                                              \
  /* aggregates */                                                       \
  SELECTOR_DEF(SELECTOR_SEQUENCE  , "sequence" , SELECTOR_TYPE_OPT, 0x1) \
  SELECTOR_DEF(SELECTOR_UNION     , "union"    , SELECTOR_TYPE_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_SUBSET    , "subset"   , SELECTOR_TYPE_OPT, 0x3) \
  /* origin operations */                                                \
  SELECTOR_DEF(SELECTOR_NODESET   , "nodeset"  , SELECTOR_TYPE_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_LOOKUP    , "lookup"   , SELECTOR_TYPE_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_PATH      , "path"     , SELECTOR_TYPE_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_SEARCH    , "search"   , SELECTOR_TYPE_OPT, 0x7) \
  /* motion operations */                                                \
  SELECTOR_DEF(SELECTOR_TRAVERSE  , "traverse" , SELECTOR_TYPE_OPT, 0x8) \
  SELECTOR_DEF(SELECTOR_MODULE_OF , "module-of", SELECTOR_TYPE_OPT, 0x9) \
  SELECTOR_DEF(SELECTOR_FSTREE_OF , "fstree-of", SELECTOR_TYPE_OPT, 0xa) \

typedef enum selector_type {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_TYPE_TABLE
#undef SELECTOR_DEF
} selector_type;

extern struct attrs32 *selector_type_attrs;

#define SELECTOR_NODESET_OPT 0xf
#define SELECTOR_NODESET_TABLE                                                   \
  SELECTOR_DEF(SELECTOR_NODESET_TARGET  , "target"  , SELECTOR_NODESET_OPT, 0x1) \
  SELECTOR_DEF(SELECTOR_NODESET_TARGETS , "targets" , SELECTOR_NODESET_OPT, 0x2) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCE  , "source"  , SELECTOR_NODESET_OPT, 0x3) \
  SELECTOR_DEF(SELECTOR_NODESET_SOURCES , "sources" , SELECTOR_NODESET_OPT, 0x4) \
  SELECTOR_DEF(SELECTOR_NODESET_MODULE  , "module"  , SELECTOR_NODESET_OPT, 0x5) \
  SELECTOR_DEF(SELECTOR_NODESET_MODULES , "modules" , SELECTOR_NODESET_OPT, 0x6) \
  SELECTOR_DEF(SELECTOR_NODESET_ANCHOR  , "anchor"  , SELECTOR_NODESET_OPT, 0x7) \

typedef enum selector_nodeset {
#define SELECTOR_DEF(x, s, r, y) x = UINT32_C(y),
SELECTOR_NODESET_TABLE
#undef SELECTOR_DEF
} selector_nodeset;

extern struct attrs32 *selector_nodeset_attrs;

#define TRAVERSE_TYPE_OPT 0x3
#define TRAVERSE_TYPE_TABLE                                                                                             \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_UP    , "up"    , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_UP)    /* traverse upwards */    \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_DOWN  , "down"  , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_DOWN)  /* traverse downwards */  \

typedef enum traverse_type {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_TYPE_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_type;

#define TRAVERSE_ORDER_OPT 0x7
#define TRAVERSE_ORDER_TABLE                                                                                          \
  TRAVERSE_ATTR_DEF(TRAVERSE_ORDER_UP    , "preorder"   , TRAVERSE_ORDER_OPT , MORIA_TRAVERSE_PRE)   /* preorder */   \
  TRAVERSE_ATTR_DEF(TRAVERSE_ORDER_DOWN  , "postorder"  , TRAVERSE_ORDER_OPT , MORIA_TRAVERSE_POST)  /* postorder */  \

typedef enum traverse_order {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_ORDER_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_order;

#define SELECTOR_EXPECT_OPT 0xf
#define SELECTOR_EXPECT_TABLE                                               \
  DEF(SELECTOR_EXPECT_ANY     , "any"       , SELECTOR_EXPECT_OPT, 0x1)     \
  DEF(SELECTOR_EXPECT_ZERO    , "zero"      , SELECTOR_EXPECT_OPT, 0x2)     \
  DEF(SELECTOR_EXPECT_ONE     , "one"       , SELECTOR_EXPECT_OPT, 0x3)     \
  DEF(SELECTOR_EXPECT_NONZERO , "nonzero"   , SELECTOR_EXPECT_OPT, 0x4)     \

typedef enum selector_expect {
#undef DEF
#define DEF(x, s, r, y) x = y,
SELECTOR_EXPECT_TABLE
} selector_expect;

extern struct attrs32 *selector_expect_attrs;

#define SELECTOR_ORDER_OPT 0xf
#define SELECTOR_ORDER_TABLE                             \
  DEF(SELECTOR_ORDER_ASCENDING   , "ascending"    , 1)   \
  DEF(SELECTOR_ORDER_DESCENDING  , "descending"   , 2)   \

typedef enum selector_order {
#undef DEF
#define DEF(x, s, y) x = y,
SELECTOR_ORDER_TABLE
} selector_order;

extern struct attrs32 *selector_order_attrs;

#define SELECTOR_SORTKEY_OPT 0xf
#define SELECTOR_SORTKEY_TABLE                            \
  DEF(SELECTOR_SORTKEY_RUNTIME   , "runtime"    , 1)      \
  DEF(SELECTOR_SORTKEY_FORKTIME  , "forktime"   , 2)      \

typedef enum selector_sortkey {
#undef DEF
#define DEF(x, s, y) x = y,
SELECTOR_SORTKEY_TABLE
} selector_sortkey;

extern struct attrs32 *selector_sortkey_attrs;

struct selector;
typedef struct selector {
  selector_type type;

  bool nofollow;              // dont follow symlinks
  bool hidden;                // allow !VERTEX_EXISTS_BIT
  selector_expect expect;

  union {
    llist head;                                   // SELECTOR_AGGREGATE
    selector_nodeset nodeset;                     // SELECTOR_NODESET
    struct {                                      // SELECTOR_LOOKUP
      struct lookup_pattern *pattern;
      char *namespace;
      uint16_t namespace_len;
    } lookup;
    struct {                                      // SELECTOR_PATH
      struct path_pattern *pattern;
    } path;
    struct {                                      // SELECTOR_SEARCH
      struct search_pattern *pattern;
    } search;
    struct selector_traverse {                    // SELECTOR_TRAVERSE
      moria_traversal_direction direction;
      moria_traversal_mode mode;
      moria_traversal_criteria criteria;
      bool exhaustive;           // MORIA_TRAVERSE_EXHAUSTIVE
      bool cross_module;
      bool cross_fstree;
      bool primary;
      bool invalid;
      uint32_t visit_mask;       // limit visit by vertex attrs
      uint32_t visit_bits;
    } traverse;
    struct {
      uint16_t limit;
      struct {
        struct {
          selector_order order;
          selector_sortkey key;
        } ranks[16];
        uint16_t ranks_len;
      } sort;
    } subset;
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
  const struct module *build_mod;
  const struct module *anchor_mod;
  const struct fsent *cwd;
  selection_iteration_type iteration_type;

  // mutable
  selector *sel;
  struct lookup_context lctx;
  const struct fstree *fst;

  // results
  struct selection *selection; // the active selection
  struct request_handler *handler;
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

void selector_describe(selector * const restrict sel, struct bacon_writer * const restrict builder_writer)
  __attribute__((nonnull));

void selector_graph_fs(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_depends(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_records(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_special(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_graph_uses(struct selector_traverse * const restrict traverse)
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

void selector_visit_primary(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_invalid(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_rule(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_fstree(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_record(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_rma(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_client(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_visit_bpe(struct selector_traverse * const restrict traverse)
  __attribute__((nonnull));

void selector_show(selector * const restrict sel)
  __attribute__((nonnull));
