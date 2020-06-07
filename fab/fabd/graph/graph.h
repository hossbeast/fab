/* Copyright (c) 2011-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_GRAPH_H
#define FABD_GRAPH_H

#include "types.h"
#include "xapi.h"
#include "valyria/llist.h"

struct attrs32;
struct dictionary;
struct edge;
struct narrator;
struct vertex_traversal_state;
struct edge_traversal_state;
struct rule_run_context;
struct vertex;
struct rule_module_association;

#define GRAPH_VERTEX_VALUE_SIZE 256
#define GRAPH_EDGE_VALUE_SIZE   64

/* edge attrs ranges */
#define EDGE_TYPE_OPT 0x00fff000

#define EDGE_INVALIDATIONS                               0x00100000 /* invalidations propagate along these edges */

/* edge options */
#define EDGE_TYPE_TABLE                                                                                          \
  DEF(EDGE_TYPE_FS        , "fs"        , EDGE_TYPE_OPT, 0x00400000) /* directory : directory entry */             \
  DEF(EDGE_TYPE_IMPORTS   , "imports"   , EDGE_TYPE_OPT, 0x00001000) /* module A imports directory B into scope */ \
  DEF(EDGE_TYPE_USES      , "uses"      , EDGE_TYPE_OPT, 0x00002000) /* module A uses model B */                   \
  DEF(EDGE_TYPE_REQUIRES  , "requires"  , EDGE_TYPE_OPT, 0x00004000) /* module A requires module B */              \
  DEF(EDGE_TYPE_STRONG    , "strong"    , EDGE_TYPE_OPT, 0x00108000) /* A : B */                                   \
  DEF(EDGE_TYPE_CONDUIT   , "conduit"   , EDGE_TYPE_OPT, 0x00110000) /* invalidation propagates from A -> B */     \
  DEF(EDGE_TYPE_RULE_DIR  , "rule-dir"  , EDGE_TYPE_OPT, 0x00020000) /* rule A matches in directory B */           \
  DEF(EDGE_TYPE_RULE_FML  , "rule-fml"  , EDGE_TYPE_OPT, 0x00240000) /* rule A uses formula B */                   \
  DEF(EDGE_TYPE_MOD_RULE  , "mod-rule"  , EDGE_TYPE_OPT, 0x00280000) /* module A uses rule B */                    \


typedef enum edge_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_TYPE_TABLE
} edge_type;

/* vertex attrs ranges */
#define VERTEX_STATE_OPT      0x0000000e  /* 00000000 00000000 00000000 00001110 */
#define VERTEX_TYPE_OPT       0x00000070  /* 00000000 00000000 00000000 01110000 */
#define VERTEX_FILETYPE_OPT   0x00000181  /* 00000000 00000000 00000001 10000001 */
#define VERTEX_NODETYPE_OPT   0x00001e00  /* 00000000 00000000 00011110 00000000 */
#define VERTEX_SHADOWTYPE_OPT 0x0000e000  /* 00000000 00000000 11100000 00000000 */
#define VERTEX_KIND_OPT       0x000ffff1  /* 00000000 00001111 11111111 11110001 */

/* vertex options */
#define VERTEX_TYPE_TABLE                                                                                \
  DEF(VERTEX_TYPE_NODE     , "node"     , VERTEX_TYPE_OPT      , 0x00000010) /* filesystem entry */      \
  DEF(VERTEX_TYPE_RULE     , "rule"     , VERTEX_TYPE_OPT      , 0x00000020) /* a rule */                \

typedef enum vertex_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_TYPE_TABLE
} vertex_type;

#define VERTEX_FILETYPE_TABLE                                                                     \
  DEF(VERTEX_FILETYPE_REG  , "regular"   , VERTEX_FILETYPE_OPT , 0x00000080) /* regular file */   \
  DEF(VERTEX_FILETYPE_LINK , "link"      , VERTEX_FILETYPE_OPT , 0x00000001) /* symbolic link */  \
  DEF(VERTEX_FILETYPE_DIR  , "dir"       , VERTEX_FILETYPE_OPT , 0x00000100) /* directory */      \

typedef enum vertex_filetype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_FILETYPE_TABLE
} vertex_filetype;

/* special types of nodes with specific logic */
#define VERTEX_NODETYPE_TABLE                                                 \
  DEF(VERTEX_NODETYPE_MODULE  , "module" , VERTEX_NODETYPE_OPT , 0x00000200)  \
  DEF(VERTEX_NODETYPE_MODEL   , "model"  , VERTEX_NODETYPE_OPT , 0x00000400)  \
  DEF(VERTEX_NODETYPE_FML     , "fml"    , VERTEX_NODETYPE_OPT , 0x00000600)  \
  DEF(VERTEX_NODETYPE_VAR     , "var"    , VERTEX_NODETYPE_OPT , 0x00000800)  \

typedef enum vertex_nodetype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_NODETYPE_TABLE
} vertex_nodetype;

#define VERTEX_SHADOWTYPE_TABLE                                                                                                               \
  DEF(VERTEX_SHADOWTYPE_FS              , "fs"              , VERTEX_SHADOWTYPE_OPT, 0x00002000) /* a node in the shadow fs */                \
  DEF(VERTEX_SHADOWTYPE_MODULE          , "module"          , VERTEX_SHADOWTYPE_OPT, 0x00008000) /* shadow directory //module */              \
  DEF(VERTEX_SHADOWTYPE_MODULES         , "modules"         , VERTEX_SHADOWTYPE_OPT, 0x0000a000) /* shadow directory //modules */             \


typedef enum vertex_shadowtype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_SHADOWTYPE_TABLE
} vertex_shadowtype;

/* all of the different kinds of nodes that can exist in the graph */
#define VERTEX_KIND_TABLE                                                                                                                                              \
  DEF(VERTEX_DIR                    , "dir"             , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_DIR)                             /* directory */        \
  DEF(VERTEX_MODULE_DIR             , "module-dir"      , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_DIR | VERTEX_NODETYPE_MODULE)    /* module directory */ \
  DEF(VERTEX_FILE                   , "file"            , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG)                             /* regular file */     \
  DEF(VERTEX_MODULE_BAM             , "module.bam"      , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG | VERTEX_NODETYPE_MODULE)    /* module.bam */       \
  DEF(VERTEX_MODEL_BAM              , "model.bam"       , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG | VERTEX_NODETYPE_MODEL)     /* model.bam */        \
  DEF(VERTEX_FML                    , "fml"             , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG | VERTEX_NODETYPE_FML)       /* formula file */     \
  DEF(VERTEX_VAR_BAM                , "var.bam"         , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG | VERTEX_NODETYPE_VAR)       /* var.bam */          \
  DEF(VERTEX_RULE                   , "rule"            , VERTEX_KIND_OPT , VERTEX_TYPE_RULE)                                                   /* a rule */           \
  DEF(VERTEX_SHADOW_DIR             , "shadow-dir"      , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_DIR | VERTEX_SHADOWTYPE_FS)      /* shadow fs directory */    \
  DEF(VERTEX_SHADOW_FILE            , "shadow-file"     , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_REG | VERTEX_SHADOWTYPE_FS)      /* shadow fs regular file */ \
  DEF(VERTEX_SHADOW_LINK            , "shadow-link"     , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_LINK | VERTEX_SHADOWTYPE_FS)     /* shadow fs symlink */      \
  DEF(VERTEX_SHADOW_MODULE          , "shadow-module"   , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_DIR | VERTEX_SHADOWTYPE_MODULE)  /* shadow fs //module */     \
  DEF(VERTEX_SHADOW_MODULES         , "shadow-modules"  , VERTEX_KIND_OPT , VERTEX_TYPE_NODE | VERTEX_FILETYPE_DIR | VERTEX_SHADOWTYPE_MODULES) /* shadow fs //modules */    \


typedef enum vertex_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_KIND_TABLE
} vertex_kind;

#define VERTEX_EXISTS_BIT  0x00000002
#define VERTEX_INVALID_BIT 0x00000004

#define VERTEX_STATE_TABLE                                                               \
  DEF(VERTEX_OK         , ""  , VERTEX_STATE_OPT, 0x00000002) /* up-to-date */           \
  DEF(VERTEX_INVALID    , "I" , VERTEX_STATE_OPT, 0x00000006) /* needs to be updated */  \
  DEF(VERTEX_UNCREATED  , "U" , VERTEX_STATE_OPT, 0x00000004) /* not yet created */      \
  DEF(VERTEX_UNLINKED   , "X" , VERTEX_STATE_OPT, 0x0000000c) /* deleted from fs */      \

typedef enum vertex_state {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_STATE_TABLE
} vertex_state;

extern struct attrs32 * graph_edge_attrs;
extern struct attrs32 * graph_vertex_attrs;
extern struct attrs32 * graph_kind_attrs;
extern struct attrs32 * graph_state_attrs;
extern struct attrs32 * graph_node_attrs;
extern struct attrs32 * graph_shadow_attrs;
extern struct graph * g_graph;

/* refresh state */
extern uint32_t graph_refresh_id;

/// graph_setup
//
// SUMMARY
//  setup
//
xapi graph_setup(void);

/// graph_cleanup
//
// SUMMARY
//  teardown
//
xapi graph_cleanup(void);

/// graph_node_create
//
// SUMMARY
//  create a node in g_graph
//
// PARAMETERS
//  n      - (returns) newly created node
//  name   - filename
//  namel  -
//
xapi graph_node_createw(void ** restrict n, const char * restrict name, uint16_t namel)
  __attribute__((nonnull));

xapi graph_delete_vertex(struct vertex *restrict v)
  __attribute__((nonnull));

xapi graph_edge_say(const struct edge * restrict ne, struct narrator * restrict N)
  __attribute__((nonnull));

typedef struct graph_invalidation_context {
  struct vertex_traversal_state * vertex_traversal;
  struct edge_traversal_state * edge_traversal;
} graph_invalidation_context;

xapi graph_invalidation_begin(graph_invalidation_context * restrict context)
  __attribute__((nonnull));

void graph_invalidation_end(graph_invalidation_context * restrict context)
  __attribute__((nonnull));

xapi graph_full_refresh(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

/*
 * enqueue this rma to be executed during the next refresh
 */
void graph_rma_enqueue(struct rule_module_association * restrict rma)
  __attribute__((nonnull));

#endif
