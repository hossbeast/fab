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

#ifndef FABD_NODE_H
#define FABD_NODE_H

#include <stdint.h>

#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "moria/vertex.h"

#include "types.h"
#include "xapi.h"

#include "graph.h"
#include "buildplan_entity.h"
#include "sweeper_thread.h"
#include "notify_thread.h"

struct edge;      // moria/edge.h
struct attrs32;
struct config;
struct ff_node;
struct pattern;
struct formula;
struct value;
struct map;
struct module;
struct narrator;
struct path;
struct vertex;
struct rule;
struct variant;
struct yyu_location;
enum vertex_filetype;
enum rule_direction;
struct graph_invalidation_context;
struct filesystem;
struct fstree;

// specially named nodes
#define NODE_MODEL_BAM      "model.bam"
#define NODE_MODULE_BAM     "module.bam"
#define NODE_VAR_BAM        "var.bam"

// globally scoped nodes
extern struct node * g_root;            // directory node at /
extern struct node * g_project_root;    // directory node at the project root
extern struct node * g_project_shadow;  // project modules shadow node

// node lookup datastructures
extern struct set * g_parse_nodes;
extern struct map * g_nodes_by_wd;

// VERTEX_TYPE_FSENT
typedef struct node {
  struct path * name;               // name (not full path) of filesystem entry

  struct {
    uint8_t not_loaded:1;
    uint8_t not_parsed:1;
  };

  /* variant the node is associated with, if any */
  const struct variant * var;

  /* a node has at most one associated bpe */
  buildplan_entity self_bpe;        // generate-only/match-only one-to-one rule
  buildplan_entity *bpe;            // any other type of rule, owned by some edge

  union {
    struct {                        // FILETYPE_DIR
      int wd;                       // watch descriptor
      bool descended;
      struct module * mod;          // module the node belongs to
      const struct filesystem * fs; // not null
      const struct fstree *fst;     // not null if the fs is rooted at this node
      uint8_t fs_epoch;

      /* notify-thread */
      rbtree pending_child;         // sweeper_child_event
    };

    struct {                        // FILETYPE_REG
      union {
        struct formula * self_fml;    // VERTEX_NODETYPE_FML  e.g. bam-cc
        struct value * self_var;      // VERTEX_NODETYPE_VAR  var.bam
        struct module * self_mod;     // VERTEX_NODETYPE_MOD  module.bam
      };

      uint16_t bp_plan_id;
      uint32_t hash;              // for INVALIDATE_STAT | INVALIDATE_CONTENT
    };
  };

  int walk_id;                    // walker
  sweeper_event pending;          // sweeper-thread
  enum notify_state notify_state;
  uint32_t notify_epoch;
} node;

STATIC_ASSERT(sizeof(node) <= GRAPH_VERTEX_VALUE_SIZE);

#define EDGE_DIRECTION_TABLE                                    \
  DEF(EDGE_TGT_SRC   , 0x0001)  /* ltr : targets -> sources */  \
  DEF(EDGE_SRC_TGT   , 0x0002)  /* rtl : sources <- targets */  \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum edge_direction {
EDGE_DIRECTION_TABLE
} edge_direction;

/* dependency edge connecting two nodes */
typedef struct node_edge_dependency {          // EDGE_TYPE_STRONG | EDGE_TYPE_CONDUIT
  buildplan_entity bpe;
  edge_direction dir;
  uint32_t refresh_id;
  struct rule_module_association *rma;
  llist lln;                                   // rma->edges
} node_edge_dependency;
STATIC_ASSERT(sizeof(node_edge_dependency) <= GRAPH_EDGE_VALUE_SIZE);

/* imports edge connecting two module directory nodes */
typedef struct node_edge_imports {          // EDGE_TYPE_IMPORTS
  uint8_t shadow_epoch;
  struct edge * imports_edge;
  struct edge * scope_edge;
} node_edge_imports;
STATIC_ASSERT(sizeof(node_edge_imports) <= GRAPH_EDGE_VALUE_SIZE);

#define NODE_PROPERTY_OPT 0xf000
#define NODE_PROPERTY_TABLE                                                    \
  /* direct properties of the node name */                                     \
  DEF(NODE_PROPERTY_NAME         , "name"         , NODE_PROPERTY_OPT, 0x1000) \
  DEF(NODE_PROPERTY_EXT          , "ext"          , NODE_PROPERTY_OPT, 0x2000) \
  DEF(NODE_PROPERTY_SUFFIX       , "suffix"       , NODE_PROPERTY_OPT, 0x3000) \
  DEF(NODE_PROPERTY_BASE         , "base"         , NODE_PROPERTY_OPT, 0x4000) \
  /* derived properties of the node path */                                    \
  DEF(NODE_PROPERTY_ABSPATH      , "abspath"      , NODE_PROPERTY_OPT, 0x5000) \
  DEF(NODE_PROPERTY_ABSDIR       , "absdir"       , NODE_PROPERTY_OPT, 0x6000) \
  DEF(NODE_PROPERTY_RELPATH      , "relpath"      , NODE_PROPERTY_OPT, 0x7000) \
  DEF(NODE_PROPERTY_RELDIR       , "reldir"       , NODE_PROPERTY_OPT, 0x8000) \
  /* other derived properties */                                               \
  DEF(NODE_PROPERTY_FSROOT       , "fsroot"       , NODE_PROPERTY_OPT, 0x9000) \
  DEF(NODE_PROPERTY_INVALIDATION , "invalidation" , NODE_PROPERTY_OPT, 0xa000) \
  DEF(NODE_PROPERTY_VARIANT      , "variant"      , NODE_PROPERTY_OPT, 0xb000) \
  DEF(NODE_PROPERTY_STATE        , "state"        , NODE_PROPERTY_OPT, 0xc000) \
  DEF(NODE_PROPERTY_TYPE         , "type"         , NODE_PROPERTY_OPT, 0xd000) \

typedef enum node_property {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
NODE_PROPERTY_TABLE
} node_property;

struct attrs32 * node_property_attrs;

typedef struct node_property_context {
  const struct module * mod;
} node_property_context;

/// node_setup
//
// SUMMARY
//  setup
//
xapi node_setup(void);

/// node_setup_minimal
//
// SUMMARY
//  setup
//
xapi node_setup_minimal(void);

/// node_cleanup
//
// SUMMARY
//  teardown
//
xapi node_cleanup(void);

/// node_reconfigure
//
// SUMMARY
//  apply configuration changes
//
xapi node_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

/// node_create
//
// SUMMARY
//  create a node in g_graph
//
// PARAMETERS
//  n      - (returns) newly created node
//  filetype - filesystem type of the node. Certain node operations can change this later.
//  [fs]   - filesystem the node belongs to
//  [rule] - rule by which the node was generated
//  [mod]  - module the node belongs to
//  name   - filename
//
xapi node_createw(
    /* 1 */ node ** restrict n
  , /* 2 */ enum vertex_filetype filetype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ struct module * restrict mod
  , /* 5 */ const char * restrict name
  , /* 6 */ uint16_t namel
)
  __attribute__((nonnull(1, 5)));

xapi node_creates(
    /* 1 */ node ** restrict n
  , /* 2 */ enum vertex_filetype filetype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ struct module * restrict mod
  , /* 5 */ const char * restrict name
)
  __attribute__((nonnull(1, 5)));

xapi node_xdestroy(node * n)
  __attribute__((nonnull));

static inline node * node_fsparent(const node * restrict n)
{
  const vertex * v = vertex_containerof(n);
  if((v = vertex_up(v)))
    return vertex_value(v);

  return 0;
}

static inline enum vertex_kind node_kind_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_KIND_OPT;
  return attrs;
}

static inline void node_kind_set(node * restrict n, enum vertex_kind kind)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= ~VERTEX_KIND_OPT;
  attrs |= kind;

RUNTIME_ASSERT(attrs);
  vertex_containerof(n)->attrs = attrs;
}

static inline enum vertex_filetype node_filetype_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_FILETYPE_OPT;
  return attrs;
}

static inline void node_filetype_set(node * restrict n, enum vertex_filetype filetype)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;

  attrs &= ~VERTEX_FILETYPE_OPT;
  attrs |= filetype;
  vertex_containerof(n)->attrs = attrs;
}

static inline enum vertex_shadowtype node_shadowtype_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_SHADOWTYPE_OPT;
  return attrs;
}

static inline void node_shadowtype_set(const node * restrict n, enum vertex_shadowtype shadowtype)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= ~VERTEX_SHADOWTYPE_OPT;
  attrs |= shadowtype;

  vertex_containerof(n)->attrs = attrs;
}

static inline enum vertex_nodetype node_nodetype_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_NODETYPE_OPT;
  return attrs;
}

static inline enum vertex_type node_type_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_TYPE_OPT;
  return attrs;
}

static inline void node_nodetype_set(node * restrict n, enum vertex_nodetype nodetype)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= ~VERTEX_NODETYPE_OPT;
  attrs |= nodetype;

  vertex_containerof(n)->attrs = attrs;

  if(nodetype == VERTEX_NODETYPE_FML)
    n->self_fml = 0;
  else if(nodetype == VERTEX_NODETYPE_VAR)
    n->self_var = 0;
}

static inline void node_state_set(node * restrict n, vertex_state state)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;
  attrs &= ~VERTEX_STATE_OPT;
  attrs |= state;

  RUNTIME_ASSERT(attrs);
  vertex_containerof(n)->attrs = attrs;
}

static inline vertex_state node_state_get(node * restrict n)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_STATE_OPT;

  return attrs;
}

static inline bool node_exists_get(node * restrict n)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_EXISTS_BIT;

  return !!attrs;
}

static inline bool node_invalid_get(node * restrict n)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;
  attrs &= VERTEX_INVALID_BIT;

  return !!attrs;
}

static inline void node_invalid_set(node * restrict n, bool invalid)
{
  uint32_t attrs;

  attrs = vertex_containerof(n)->attrs;
  attrs &= ~VERTEX_INVALID_BIT;

  if(invalid)
    attrs |= VERTEX_INVALID_BIT;

  RUNTIME_ASSERT(attrs);
  vertex_containerof(n)->attrs = attrs;
}

static inline struct module * node_module_get(const node * restrict n)
{
  enum vertex_filetype filetype;

  filetype = node_filetype_get(n);
  if(filetype == VERTEX_FILETYPE_REG)
    n = node_fsparent(n);

  return n->mod;
}

const struct filesystem * node_filesystem_get(node * restrict n)
  __attribute__((nonnull));

/// node_graft
//
// SUMMARY
//  get or create nodes starting at the root to some base path
//
// PARAMETERS
//  base - absolute path from the root to the desired node
//  rn   - (returns) node rooted at path base
//
xapi node_graft(const char * restrict base, node ** restrict rn, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/// node_var_loadp
//
// SUMMARY
//  mark this node as NODETYPE_VAR and load the env associated with this node (idempotent)
//
xapi node_var_loadp(node * restrict n)
  __attribute__((nonnull));

/// node_fml_loadp
//
// SUMMARY
//  mark this node as an NODETYPE_FML
//
xapi node_fml_loadp(node * restrict n)
  __attribute__((nonnull));

xapi node_full_refresh(void);

xapi node_property_say(const node * restrict n, node_property property, const node_property_context * restrict ctx, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the project-relative path to the node to a buffer
 */
size_t node_get_project_relative_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

/*
 * write the module-relative path to the node to a buffer
 */
size_t node_get_module_relative_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

/*
 * write the absolute path to the node to a buffer
 */
size_t node_get_absolute_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

size_t node_get_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

/*
 * write the project-relative path to a node to a narrator
 */
xapi node_project_relative_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the module-relative path to a node to a narrator
 */
xapi node_module_relative_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write the absolute path to a node to a narrator
 */
xapi node_absolute_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

/*
 * write one of the paths to a node to a narrator
 */
xapi node_path_say(node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
