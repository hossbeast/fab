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

#ifndef _FABD_NODE_H
#define _FABD_NODE_H

#include <stdint.h>
#include "xapi.h"

#include "moria/vertex.h"

struct map;
struct module;
struct narrator;
struct path;
struct reconfigure_context;
struct value;

// relation attributes
#define NODE_RELATION_TABLE	    																			                        \
	NODE_RELATION(NODE_RELATION_FS      , 0x1 , "filesystem")	/* directory : directory entry */	\
  NODE_RELATION(NODE_RELATION_WEAK    , 0x2 , "weak")       /* A :^ B */                      \
  NODE_RELATION(NODE_RELATION_STRONG  , 0x6 , "strong")     /* A : B */                       \

enum {
#define NODE_RELATION(a, b, c) a = UINT32_C(b),
NODE_RELATION_TABLE
#undef NODE_RELATION
};

#define NODE_FSTYPE_TABLE             \
  NODE_FSTYPE_DEF(NODE_FSTYPE_DIR)    \
  NODE_FSTYPE_DEF(NODE_FSTYPE_FILE)

typedef enum node_fstype {
  NODE_FSTYPE_RANGE_BEFORE = 0,

#define NODE_FSTYPE_DEF(t) t,
NODE_FSTYPE_TABLE
#undef NODE_FSTYPE_DEF

  NODE_FSTYPE_RANGE_AFTER
} node_fstype;

/// node_fstype_name
//
// SUMMARY
//  get the string name for a node_fstype
//
const char *node_fstype_name(node_fstype type);

// the node corresponding to the project directory
extern struct graph * g_node_graph;
extern struct map * g_nodes_by_wd;

// allocated as the value of a vertex
typedef struct node
{
  struct path * name;             // name of filesystem entry
  node_fstype   fstype;           // one of NODE_FSTYPE_*

  int invalid;
  const struct filesystem * fs;   // not null

  struct module * mod;            // module rooted at this dir, if any

  union {
    struct {    // FSTYPE_DIR and INVALIDATE_NOTIFY
      int wd;
    };
  };

/*

reference counts for deletion

*/

  int walk_id;
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
//
//
xapi node_createw(
    /* 1 */ node ** restrict n
  , /* 2 */ node_fstype fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ const char * restrict name
  , /* 5 */ size_t namel
)
  __attribute__((nonnull));

xapi node_creates(
    /* 1 */ node ** restrict n
  , /* 2 */ node_fstype fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ const char * restrict name
)
  __attribute__((nonnull));

void node_destroy(node * n)
  __attribute__((nonnull));

xapi node_dump(node * restrict)
  __attribute__((nonnull));

size_t node_get_relative_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

size_t node_get_absolute_path(const node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

xapi node_path_say(const node * restrict n, struct narrator * restrict N)
  __attribute__((nonnull));

static inline node * node_fsparent(const node * restrict n)
{
  vertex * fsparent = vertex_travel_vertex(
      vertex_containerof(n)
    , 0
    , NODE_RELATION_FS
    , MORIA_TRAVERSE_UP
  );

  return vertex_value(fsparent);
}

#endif
