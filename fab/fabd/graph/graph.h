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

/*

There is a singleton moria/graph which contains the project, areas of the filesystem referenced by
extern in config, and all the dependencies of various kinds among all these vertices.

*/

#include "types.h"
#include "fab/fsent.h"
#include "moria.h"
#include "moria/vertex.h"

struct attrs32;
struct moria_edge;
struct narrator;
struct moria_vertex;
struct moria_connect_context;

/* edge attrs ranges */
#define EDGE_TYPE_OPT           0x40000fff
#define EDGE_DEPENDENCY         0x00100000 /* invalidations propagate along these edges */
#define EDGE_MOD_RULE_FML       0x00200000 /* mod-rule + rule-fml edges */
#define EDGE_MODULES            0x01000000 /* module uses, requires, imports and directory edges */
#define EDGE_USES_MOD_DIR       0x02000000 /* module uses, directory edges */
#define EDGE_KIND_OPT           0x4fffffff

/* edge options */
#define EDGE_TYPE_TABLE                                                                                                       \
  DEF(EDGE_TYPE_FSTREE       , "fs"          , EDGE_TYPE_OPT, 0x40000000) /* directory : directory entry */                   \
  DEF(EDGE_TYPE_IMPORTS      , "imports"     , EDGE_TYPE_OPT, 0x00000002) /* module A imports directory B */                  \
  DEF(EDGE_TYPE_USES         , "uses"        , EDGE_TYPE_OPT, 0x00000004) /* module A uses model B */                         \
  DEF(EDGE_TYPE_REQUIRES     , "requires"    , EDGE_TYPE_OPT, 0x00000008) /* module A requires module B */                    \
  DEF(EDGE_TYPE_DEPENDS      , "depends"     , EDGE_TYPE_OPT, 0x00000010) /* foo.o : foo.c */                                 \
  DEF(EDGE_TYPE_CONDUIT      , "conduit"     , EDGE_TYPE_OPT, 0x00000020) /* invalidation propagates from A -> B */           \
  DEF(EDGE_TYPE_RULE_DIR     , "rule-dir"    , EDGE_TYPE_OPT, 0x00000040) /* rule A matches in directory B */                 \
  DEF(EDGE_TYPE_RULE_FML     , "rule-fml"    , EDGE_TYPE_OPT, 0x00000080) /* rule A has formula B */                          \
  DEF(EDGE_TYPE_MOD_RULE     , "mod-rule"    , EDGE_TYPE_OPT, 0x00000100) /* module A associated with rule B (rma exists) */  \
  DEF(EDGE_TYPE_MOD_DIR      , "mod-dir"     , EDGE_TYPE_OPT, 0x00000200) /* module A : module directory */                   \

typedef enum edge_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_TYPE_TABLE
} edge_type;

/* all of the different kinds of edges that can exist in the graph */
#define EDGE_KIND_TABLE                                                                                                                                             \
  DEF(EDGE_FSTREE       , "fs"          , EDGE_KIND_OPT , EDGE_TYPE_FSTREE)                                     /* filesystem  */                                   \
  DEF(EDGE_IMPORTS      , "imports"     , EDGE_KIND_OPT , EDGE_TYPE_IMPORTS | EDGE_MODULES)                     /* module A imports directory B into scope */       \
  DEF(EDGE_USES         , "uses"        , EDGE_KIND_OPT , EDGE_TYPE_USES | EDGE_MODULES | EDGE_USES_MOD_DIR)    /* module A uses model B */                         \
  DEF(EDGE_REQUIRES     , "requires"    , EDGE_KIND_OPT , EDGE_TYPE_REQUIRES | EDGE_MODULES)                    /* module A requires module B */                    \
  DEF(EDGE_DEPENDS      , "depends"     , EDGE_KIND_OPT , EDGE_TYPE_DEPENDS | EDGE_DEPENDENCY)                  /* A : B */                                         \
  DEF(EDGE_CONDUIT      , "conduit"     , EDGE_KIND_OPT , EDGE_TYPE_CONDUIT | EDGE_DEPENDENCY)                  /* invalidation propagates from A -> B */           \
  DEF(EDGE_RULE_DIR     , "rule-dir"    , EDGE_KIND_OPT , EDGE_TYPE_RULE_DIR)                                   /* rule A matches in directory B */                 \
  DEF(EDGE_RULE_FML     , "rule-fml"    , EDGE_KIND_OPT , EDGE_TYPE_RULE_FML | EDGE_MOD_RULE_FML)               /* rule A uses formula B */                         \
  DEF(EDGE_MOD_RULE     , "mod-rule"    , EDGE_KIND_OPT , EDGE_TYPE_MOD_RULE | EDGE_MOD_RULE_FML)               /* module A associated with rule B (rma exists) */  \
  DEF(EDGE_MOD_DIR      , "mod-dir"     , EDGE_KIND_OPT , EDGE_TYPE_MOD_DIR | EDGE_MODULES | EDGE_USES_MOD_DIR) /* module A : module directory fsent */             \

typedef enum edge_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_KIND_TABLE
} edge_kind;

/* vertex attrs ranges */
#define VERTEX_STATE_OPT       0x0000000e  /* 00000000 00000000 00000000 00001110 */
#define VERTEX_TYPE_OPT        0x00000070  /* 00000000 00000000 00000000 01110000 */
#define VERTEX_FILETYPE_OPT    0x80000181  /* 10000000 00000000 00000001 10000001 */
#define VERTEX_FSENTTYPE_OPT   0x00001e00  /* 00000000 00000000 00011110 00000000 */
#define VERTEX_SHADOWTYPE_OPT  0x0000e000  /* 00000000 00000000 11100000 00000000 */
#define VERTEX_KIND_OPT        0x800ffff1  /* 10000000 00001111 11111111 11110001 */
#define VERTEX_PROTECT_BIT     0x00100000  /* 00000000 00010000 00000000 00000000 */

/* vertex options */
#define VERTEX_TYPE_TABLE                                                                                \
  DEF(VERTEX_TYPE_FSENT    , "fsent"    , VERTEX_TYPE_OPT      , 0x00000010) /* filesystem entry */      \
  DEF(VERTEX_TYPE_RULE     , "rule"     , VERTEX_TYPE_OPT      , 0x00000020) /* a rule */                \
  DEF(VERTEX_TYPE_MODULE   , "module"   , VERTEX_TYPE_OPT      , 0x00000030) /* a module */              \
  DEF(VERTEX_TYPE_FML      , "fml"      , VERTEX_TYPE_OPT      , 0x00000040) /* a formula */             \
  DEF(VERTEX_TYPE_VAR      , "var"      , VERTEX_TYPE_OPT      , 0x00000050) /* a var */                 \
  DEF(VERTEX_TYPE_CONFIG   , "config"   , VERTEX_TYPE_OPT      , 0x00000060) /* a config blob */         \

typedef enum vertex_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_TYPE_TABLE
} vertex_type;

#define VERTEX_FILETYPE_TABLE                                                                     \
  DEF(VERTEX_FILETYPE_REG  , "regular"   , VERTEX_FILETYPE_OPT , 0x00000080) /* regular file */   \
  DEF(VERTEX_FILETYPE_LINK , "link"      , VERTEX_FILETYPE_OPT , 0x80000000) /* symbolic link */  \
  DEF(VERTEX_FILETYPE_DIR  , "dir"       , VERTEX_FILETYPE_OPT , 0x00000100) /* directory */      \

typedef enum vertex_filetype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_FILETYPE_TABLE
} vertex_filetype;

/* special types of fsents with specific logic */
#define VERTEX_FSENTTYPE_TABLE                                                                          \
  DEF(VERTEX_FSENTTYPE_MODULE  , "module" , VERTEX_FSENTTYPE_OPT , 0x00000200)  /* 00000010 00000000 */ \
  DEF(VERTEX_FSENTTYPE_MODEL   , "model"  , VERTEX_FSENTTYPE_OPT , 0x00000400)  /* 00000100 00000000 */ \
  DEF(VERTEX_FSENTTYPE_FML     , "fml"    , VERTEX_FSENTTYPE_OPT , 0x00000600)  /* 00000110 00000000 */ \
  DEF(VERTEX_FSENTTYPE_VAR     , "var"    , VERTEX_FSENTTYPE_OPT , 0x00000800)  /* 00001000 00000000 */ \
  DEF(VERTEX_FSENTTYPE_CONFIG  , "config" , VERTEX_FSENTTYPE_OPT , 0x00000a00)  /* 00001010 00000000 */ \

typedef enum vertex_fsenttype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_FSENTTYPE_TABLE
} vertex_fsenttype;

#define VERTEX_SHADOWTYPE_TABLE                                                                                              \
  DEF(VERTEX_SHADOWTYPE_FS       , "shadow-fs"       , VERTEX_SHADOWTYPE_OPT, 0x00002000) /* an fsent in the shadow fs */    \
  DEF(VERTEX_SHADOWTYPE_MODULE   , "shadow-module"   , VERTEX_SHADOWTYPE_OPT, 0x00008000) /* shadow directory //module */    \
  DEF(VERTEX_SHADOWTYPE_MODULES  , "shadow-modules"  , VERTEX_SHADOWTYPE_OPT, 0x0000a000) /* shadow directory //modules */   \

typedef enum vertex_shadowtype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_SHADOWTYPE_TABLE
} vertex_shadowtype;

/* all of the different kinds of vertices that can exist in the graph */
#define VERTEX_KIND_TABLE                                                                                                                                                       \
  DEF(VERTEX_DIR                    , "dir"             , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_DIR)                              /* directory */               \
  DEF(VERTEX_MODULE_DIR             , "module-dir"      , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_FSENTTYPE_MODULE)   /* module directory */        \
  DEF(VERTEX_FILE                   , "file"            , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG)                              /* regular file */            \
  DEF(VERTEX_MODULE_FILE            , "module-file"     , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_FSENTTYPE_MODULE)   /* module.bam file */         \
  DEF(VERTEX_MODEL_FILE             , "model-file"      , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_FSENTTYPE_MODEL)    /* model.bam file */          \
  DEF(VERTEX_FORMULA_FILE           , "formula-file"    , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_FSENTTYPE_FML)      /* formula file */            \
  DEF(VERTEX_VAR_FILE               , "var-file"        , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_FSENTTYPE_VAR)      /* var.bam file */            \
  DEF(VERTEX_CONFIG_FILE            , "config-file"     , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_FSENTTYPE_CONFIG)   /* config file */             \
  DEF(VERTEX_RULE                   , "rule"            , VERTEX_KIND_OPT , VERTEX_TYPE_RULE)                                                     /* a rule */                  \
  DEF(VERTEX_MODULE                 , "module"          , VERTEX_KIND_OPT , VERTEX_TYPE_MODULE)                                                   /* a module */                \
  DEF(VERTEX_FML                    , "formula"         , VERTEX_KIND_OPT , VERTEX_TYPE_FML)                                                      /* a formula */               \
  DEF(VERTEX_VAR                    , "var"             , VERTEX_KIND_OPT , VERTEX_TYPE_VAR)                                                      /* a var */                   \
  DEF(VERTEX_CONFIG                 , "config"          , VERTEX_KIND_OPT , VERTEX_TYPE_CONFIG)                                                   /* a config */                \
  DEF(VERTEX_SHADOW_DIR             , "shadow-dir"      , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_FS)      /* shadow fs directory */     \
  DEF(VERTEX_SHADOW_FILE            , "shadow-file"     , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SHADOWTYPE_FS)      /* shadow fs regular file */  \
  DEF(VERTEX_SHADOW_LINK            , "shadow-link"     , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_LINK | VERTEX_SHADOWTYPE_FS)      /* shadow fs symlink */       \
  DEF(VERTEX_SHADOW_MODULE          , "shadow-module"   , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_MODULE)  /* shadow fs //module */      \
  DEF(VERTEX_SHADOW_MODULES         , "shadow-modules"  , VERTEX_KIND_OPT , VERTEX_TYPE_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_MODULES) /* shadow fs //modules */     \

typedef enum vertex_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_KIND_TABLE
} vertex_kind;

#define VERTEX_EXISTS_BIT   0x00000002
#define VERTEX_INVALID_BIT  0x00000004
#define VERTEX_UNLINKED_BIT 0x00000008

/* each vertex is in one of the following states */
#define VERTEX_STATE_TABLE                                                                                                                 \
  DEF(VERTEX_OK         , ""  , VERTEX_STATE_OPT,                                            VERTEX_EXISTS_BIT) /* up-to-date */           \
  DEF(VERTEX_INVALID    , "I" , VERTEX_STATE_OPT,                       VERTEX_INVALID_BIT | VERTEX_EXISTS_BIT) /* needs to be updated */  \
  DEF(VERTEX_UNCREATED  , "U" , VERTEX_STATE_OPT,                       VERTEX_INVALID_BIT                    ) /* not yet created */      \
  DEF(VERTEX_UNLINKED   , "X" , VERTEX_STATE_OPT, VERTEX_UNLINKED_BIT | VERTEX_INVALID_BIT                    ) /* unlinked from fs */     \

typedef enum vertex_state {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_STATE_TABLE
} vertex_state;

extern struct attrs32 * graph_vertex_attrs;   /* vertex definitions */
extern struct attrs32 * graph_edge_attrs;     /* edge definitions */

/* sub-attrs */
extern struct attrs32 * graph_edge_type_attrs;
extern struct attrs32 * graph_vertex_kind_attrs;
extern struct attrs32 * graph_vertex_state_attrs;

extern struct moria_graph g_graph;      /* the graph */
extern struct hashtable * g_graph_ht;   /* supports directory node lookup by label */

void graph_setup(void);
void graph_cleanup(void);

void graph_edge_alloc(struct moria_edge ** ep)
  __attribute__((nonnull));

void graph_edge_release(struct moria_edge *)
  __attribute__((nonnull));

void graph_edge_say(const struct moria_edge * restrict ne, struct narrator * restrict N)
  __attribute__((nonnull));

/* An invalidation can visit each vertex and edge at most once */
typedef struct graph_invalidation_context {
  struct moria_vertex_traversal_state * vertex_traversal;
  struct moria_edge_traversal_state * edge_traversal;
  bool any;
} graph_invalidation_context;

void graph_invalidation_begin(graph_invalidation_context * restrict context)
  __attribute__((nonnull));

void graph_invalidation_end(graph_invalidation_context * restrict context)
  __attribute__((nonnull));

/* map an internal vertex_kind to an external fab_fsent_type */
enum fab_fsent_type vertex_kind_remap(vertex_kind kind);
enum fab_fsent_state vertex_state_remap(vertex_state state);

/* create an edge in the graph */
void graph_hyperconnect(
    /* 1 */ struct moria_connect_context * restrict ctx
  , /* 2 */ struct moria_vertex ** restrict Alist
  , /* 3 */ uint16_t Alen
  , /* 4 */ struct moria_vertex ** restrict Blist
  , /* 5 */ uint16_t Blen
  , /* 6 */ struct moria_edge * restrict e
  , /* 7 */ uint32_t attrs
)
  __attribute__((nonnull(1, 6)));

/* create an edge in the graph */
void graph_connect(
    struct moria_connect_context * restrict ctx
  , struct moria_vertex * restrict A
  , struct moria_vertex * restrict B
  , struct moria_edge * restrict e
  , uint32_t attrs
)
  __attribute__((nonnull));

/* disconnect an edge in the graph */
void graph_disconnect(struct moria_edge * restrict e)
  __attribute__((nonnull));

/* disconnect an fsedge or dependency edge and cascade */
void graph_disintegrate(struct moria_edge * restrict ne, struct graph_invalidation_context * restrict context)
  __attribute__((nonnull(1)));

#endif
