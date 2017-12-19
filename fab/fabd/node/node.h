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

#include "moria/vertex.h" // vertex_value

#include "types.h"
#include "xapi.h"

struct formula;
struct map;
struct module;
struct narrator;
struct path;
struct reconfigure_context;
struct value;
struct vertex;
struct ff_node;
struct ff_node_pattern;

/*
 * relation/edge attributes
 */
#define RELATION_TYPE_TABLE                                                                 \
  RELATION_ATTR_DEF(RELATION_TYPE_FS      , 0x00000001)   /* directory : directory entry */ \
  RELATION_ATTR_DEF(RELATION_TYPE_WEAK    , 0x00000002)   /* A :^ B */                      \
  RELATION_ATTR_DEF(RELATION_TYPE_STRONG  , 0x00000003)   /* A : B */                       \

typedef enum relation_type {
#define RELATION_ATTR_DEF(x, y) x = UINT32_C(y),
RELATION_TYPE_TABLE
#undef RELATION_ATTR_DEF
} relation_type;

#define RELATION_TYPE_OPT 0x0000000F

/// node_relation_name
//
// SUMMARY
//  get the string name for a node_relation
//
const char *relation_type_name(uint32_t attrs);

/*
 * node/vertex attributes
 */
#define NODE_FSTYPE_TABLE                          \
  NODE_ATTR_DEF(NODE_FSTYPE_DIR   , 0x00000010)    \
  NODE_ATTR_DEF(NODE_FSTYPE_FILE  , 0x00000020)    \

typedef enum node_fstype {
#define NODE_ATTR_DEF(x, y) x = UINT32_C(y),
NODE_FSTYPE_TABLE
#undef NODE_ATTR_DEF
} node_fstype;

#define NODE_FSTYPE_OPT 0x000000F0

/// node_fstype_name
//
// SUMMARY
//  get the string name for a node_fstype
//
const char *node_fstype_name(uint32_t attrs);

// globally scoped nodes
extern struct graph * g_node_graph;
extern struct map * g_nodes_by_wd;
extern struct node * g_root;            // directory node at /
extern struct node * g_project_root;    // directory node at the project root

// allocated as the value of a vertex
typedef struct node
{
  struct path * name;             // name of filesystem entry
  bool invalid;

  struct module * mod;            // module
  const struct filesystem * fs;   // not null
  struct ff_node * ffn;

  union {           // FSTYPE_DIR
    struct {
      int wd;                       // for INVALIDATE_NOTIFY
    };

    struct {        // FSTYPE_FILE
      struct formula * fml;
    };
  };

  int walk_id;    // reference counts for deletion
  int build_depth;
} node;

/// node_setup
//
// SUMMARY
//  setup
//
xapi node_setup(void);

/// node_cleanup
//
// SUMMARY
//  teardown
//
xapi node_cleanup(void);

/// node_root_init
//
// SUMMARY
//  create the root node (g_root)
//
xapi node_root_init(void);

/// node_project_init
//
// SUMMARY
//  create the project node (g_project_root) and attach it to the root
//
xapi node_project_init(void);

/// node_report
//
// SUMMARY
//  log all of the nodes in the graph under L_GRAPH
//
xapi node_report(void);

/// node_reconfigure
//
// SUMMARY
//  apply configuration changes
//
xapi node_reconfigure(struct reconfigure_context * ctx, const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

/// node_create
//
// SUMMARY
//  create a node in g_node_graph
//
// PARAMETERS
//  n      - (returns) newly created node
//  fstype - filesystem type of the node. Certain node operations can change this later.
//  [fs]   - filesystem the node belongs to
//  [mod]  - module the node belongs to
//  name   - filename
//  namel  - 
//
xapi node_createw(
    /* 1 */ node ** restrict n
  , /* 2 */ node_fstype fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ struct module * restrict mod
  , /* 5 */ const char * restrict name
  , /* 6 */ uint16_t namel
)
  __attribute__((nonnull(1, 5)));

xapi node_creates(
    /* 1 */ node ** restrict n
  , /* 2 */ node_fstype fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ struct module * restrict mod
  , /* 5 */ const char * restrict name
)
  __attribute__((nonnull(1, 5)));

void node_destroy(node * n)
  __attribute__((nonnull));

size_t node_get_relative_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

size_t node_get_absolute_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

xapi node_relative_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

xapi node_absolute_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

xapi node_lookup_path(const struct ff_node * restrict ffn, const char * restrict path, node ** restrict rn)
  __attribute__((nonnull(2, 3)));

xapi node_lookup_pattern(const struct ff_node_pattern * restrict ref, node ** restrict rn)
  __attribute__((nonnull(2)));

static inline node * node_fsparent(const node * restrict n)
{
  const vertex * v = vertex_containerof(n);
  if((v = vertex_up(v)))
    return vertex_value(v);

  return 0;
}

static inline node_fstype node_fstype_get(const node * restrict n)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= NODE_FSTYPE_OPT;
  return attrs;
}

static inline void node_fstype_set(const node * restrict n, node_fstype fstype)
{
  uint32_t attrs = vertex_containerof(n)->attrs;
  attrs &= ~NODE_FSTYPE_OPT;
  attrs |= fstype;
  vertex_containerof(n)->attrs = attrs;
}

/// node_graft
//
// SUMMARY
//  get or create nodes starting at the root to some base path
//
// PARAMETERS
//  base - absolute path from the root to the desired node
//  rn   - (returns) node rooted at path base
//
xapi node_graft(const char * restrict base, node ** restrict rn)
  __attribute__((nonnull));

xapi node_get_ffn(node * restrict n, struct ff_node ** ffn)
  __attribute__((nonnull(1)));

#endif
