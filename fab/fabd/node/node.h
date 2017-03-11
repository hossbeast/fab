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

struct path;
struct map;
struct reconfigure_context;
struct value;

// relation attributes
#define NODE_RELATION_TABLE	    																			        \
	NODE_RELATION(FS      , 1 , "filesystem")	/* directory : directory entry */	\
  NODE_RELATION(WEAK    , 2 , "weak")       /* A :^ B */                      \
  NODE_RELATION(STRONG  , 3 , "strong")     /* A : B */                       \

enum {
#define NODE_RELATION(a, b, c) NODE_RELATION_ ## a = UINT32_C(b),
NODE_RELATION_TABLE
#undef NODE_RELATION
};

#define NODE_FS_TYPE_TABLE                                      \
  NODE_FS_TYPE(DIR    , 1 , "dir")                              \
  NODE_FS_TYPE(FILE   , 2 , "file")                             \

enum {
#define NODE_FS_TYPE(a, b, c) NODE_FS_TYPE_ ## a = UINT8_C(b),
NODE_FS_TYPE_TABLE
#undef NODE_FS_TYPE
};

// the node corresponding to the project directory
extern struct graph * g_node_graph;
extern struct node * g_root;
extern struct map * g_nodes_by_wd;

// allocated as the value of a vertex
typedef struct node
{
  struct path * name;   // name of filesystem entry
  uint8_t       fstype;

  int invalid;
  const struct filesystem * fs;   // not null
  struct node * fsparent;

  union {
    struct {    // FS_TYPE_DIR and INVALIDATE_NOTIFY
      int wd;
    };
  };

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
  , /* 2 */ uint8_t fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ const char * restrict name
  , /* 5 */ size_t namel
)
  __attribute__((nonnull));

xapi node_creates(
    /* 1 */ node ** restrict n
  , /* 2 */ uint8_t fstype
  , /* 3 */ const struct filesystem * restrict fs
  , /* 4 */ const char * restrict name
)
  __attribute__((nonnull));

void node_destroy(node * n)
  __attribute__((nonnull));

xapi node_dump(void);

size_t node_get_path(node * restrict n, void * restrict dst, size_t dst_size)
  __attribute__((nonnull));

#endif
