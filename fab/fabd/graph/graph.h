/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is a singleton moria/graph which contains the project, areas of the filesystem referenced by
extern in config, and all the dependencies of various kinds among all these vertices.

*/

#include "types.h"
#include "moria.h"
#include "moria/vertex.h"
#include "moria/traverse.h"
#include "valyria/hashtable.h"

struct attrs32;
struct moria_edge;
struct narrator;
struct moria_vertex;
struct bacon_writer;
struct moria_traverse_result;
struct llist;
struct entlist;
enum bam_vertex_type;
enum bam_vertex_state;

/* edge attrs ranges */
#define EDGE_KIND_OPT           0x40000ff0 /* 01000000 00000000 00001111 11110000 */
#define EDGE_GROUPS_OPT         0x000f0000 /* 00000000 00001111 00000000 00000000 */
#define EDGE_TYPE_OPT           0x400ffff0 /* 01000000 00001111 11111111 11110000 */

/* edge groups */
#define EDGE_INVALIDATIONS      0x00010000 /* invalidations propagate along these edges */
#define EDGE_DEPENDS            0x00020000 /* edges between fsents and bpes, created by running rules */
#define EDGE_MODULES            0x00040000 /* module A uses, requires, imports, var B edges */

/* edge options */
#define EDGE_KIND_TABLE                                                                                                          \
  DEF(EDGE_KIND_FSTREE        , "fs"            , EDGE_KIND_OPT, 0x40000010) /* directory : directory entry */                   \
  DEF(EDGE_KIND_USES          , "uses"          , EDGE_KIND_OPT, 0x00000020) /* module A uses rules.bam B */                     \
  DEF(EDGE_KIND_REQUIRES      , "requires"      , EDGE_KIND_OPT, 0x00000030) /* module A requires module B */                    \
  DEF(EDGE_KIND_VAR           , "var"           , EDGE_KIND_OPT, 0x00000040) /* module A anchored to directory B */              \
  DEF(EDGE_KIND_BPE_TGT       , "bpe-tgt"       , EDGE_KIND_OPT, 0x00000050) /* bpe B -> foo.o A */                              \
  DEF(EDGE_KIND_SRC_BPE       , "src-bpe"       , EDGE_KIND_OPT, 0x00000060) /* foo.c B -> bpe A */                              \
  DEF(EDGE_KIND_DIR_RMA       , "dir-rma"       , EDGE_KIND_OPT, 0x00000070) /* dirent B -> rma A */                             \
  DEF(EDGE_KIND_INDEX_RMA     , "index-rma"     , EDGE_KIND_OPT, 0x00000080) /* index-dir B -> rma A */                          \
  DEF(EDGE_KIND_INDEX_MOD     , "index-mod"     , EDGE_KIND_OPT, 0x00000090) /* index-dir B -> rma A */                          \
  DEF(EDGE_KIND_FML_RMA       , "fml-bpe"       , EDGE_KIND_OPT, 0x000000a0) /* fml B attached to bpe A */                       \
  DEF(EDGE_KIND_RULE_RMA      , "rule-rma"      , EDGE_KIND_OPT, 0x000000b0) /* rule B -> rma A */                               \
  DEF(EDGE_KIND_RMA_MOD       , "rma-mod"       , EDGE_KIND_OPT, 0x000000c0) /* rma B -> module A */                             \
  DEF(EDGE_KIND_RMA_TGT       , "rma-tgt"       , EDGE_KIND_OPT, 0x000000d0) /* rma B -> bind target A */                        \
  DEF(EDGE_KIND_RMA_BPE       , "rma-bpe"       , EDGE_KIND_OPT, 0x000000e0) /* rma B -> mod A */                                \
  DEF(EDGE_KIND_RMA_RULES     , "rma-rules"     , EDGE_KIND_OPT, 0x000000f0) /* rma B -> rules A */                              \
  DEF(EDGE_KIND_RULES_SPECIAL , "rules-special" , EDGE_KIND_OPT, 0x00000100) /* fsent B -> special A */                          \
  DEF(EDGE_KIND_MODULE_SPECIAL, "module-special", EDGE_KIND_OPT, 0x00000200) /* fsent B -> special A */                          \
  DEF(EDGE_KIND_VAR_SPECIAL   , "var-special"   , EDGE_KIND_OPT, 0x00000300) /* fsent B -> special A */                          \
  DEF(EDGE_KIND_CONFIG_SPECIAL, "config-special", EDGE_KIND_OPT, 0x00000400) /* fsent B -> special A */                          \
  DEF(EDGE_KIND_FML_SPECIAL   , "fml-special"   , EDGE_KIND_OPT, 0x00000500) /* fsent B -> special A */                          \
  DEF(EDGE_KIND_INDEX_DIR     , "index"         , EDGE_KIND_OPT, 0x40000600) /* index-dir -> index-dir */                        \
  DEF(EDGE_KIND_INDEX         , "index"         , EDGE_KIND_OPT, 0x00000700) /* fsent -> index-dir */                            \

typedef enum edge_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_KIND_TABLE
} edge_kind;

/* all of the different kinds of edges that can exist in the graph */
#define EDGE_TYPE_TABLE                                                                                                                                                         \
  DEF(EDGE_FSTREE        , "fs"            , EDGE_TYPE_OPT, EDGE_KIND_FSTREE)                                               /* filesystem  */                                   \
  DEF(EDGE_RULES_MOD     , "rules-mod"     , EDGE_TYPE_OPT, EDGE_KIND_USES | EDGE_MODULES | EDGE_INVALIDATIONS)             /* module A uses rules.bam B */                     \
  DEF(EDGE_REQUIRES      , "requires"      , EDGE_TYPE_OPT, EDGE_KIND_REQUIRES | EDGE_MODULES | EDGE_INVALIDATIONS)         /* module A requires module B */                    \
  DEF(EDGE_VAR_MOD       , "var-mod"       , EDGE_TYPE_OPT, EDGE_KIND_VAR | EDGE_MODULES | EDGE_INVALIDATIONS)              /* module A loaded var.bam B */                     \
  DEF(EDGE_BPE_TGT       , "bpe-tgt"       , EDGE_TYPE_OPT, EDGE_KIND_BPE_TGT | EDGE_DEPENDS | EDGE_INVALIDATIONS)          /* dependencies */                                  \
  DEF(EDGE_SRC_BPE       , "src-bpe"       , EDGE_TYPE_OPT, EDGE_KIND_SRC_BPE | EDGE_DEPENDS | EDGE_INVALIDATIONS)          /* dependencies */                                  \
  DEF(EDGE_DIR_RMA       , "dir-rma"       , EDGE_TYPE_OPT, EDGE_KIND_DIR_RMA | EDGE_INVALIDATIONS)                         /* rule A matches in directory B */                 \
  DEF(EDGE_INDEX_RMA     , "index-rma"     , EDGE_TYPE_OPT, EDGE_KIND_INDEX_RMA | EDGE_INVALIDATIONS)                       /* bind A looked-up in index-dir B */               \
  DEF(EDGE_INDEX_MOD     , "index-mod"     , EDGE_TYPE_OPT, EDGE_KIND_INDEX_MOD | EDGE_INVALIDATIONS)                       /* module A import looked-up in index-dir B */      \
  DEF(EDGE_FML_RMA       , "fml-bpe"       , EDGE_TYPE_OPT, EDGE_KIND_FML_RMA | EDGE_INVALIDATIONS)                         /* rule A generates formula B */                    \
  DEF(EDGE_RULE_RMA      , "rule-rma"      , EDGE_TYPE_OPT, EDGE_KIND_RULE_RMA)                                                                                                 \
  DEF(EDGE_RMA_MOD       , "rma-mod"       , EDGE_TYPE_OPT, EDGE_KIND_RMA_MOD)                                                                                                  \
  DEF(EDGE_RMA_RULES     , "rma-rules"     , EDGE_TYPE_OPT, EDGE_KIND_RMA_RULES)                                                                                                \
  DEF(EDGE_RMA_BPE       , "rma-bpe"       , EDGE_TYPE_OPT, EDGE_KIND_RMA_BPE | EDGE_INVALIDATIONS)                                                                             \
  DEF(EDGE_RMA_TGT       , "rma-tgt"       , EDGE_TYPE_OPT, EDGE_KIND_RMA_TGT | EDGE_INVALIDATIONS)                                                                             \
  DEF(EDGE_RULES_SPECIAL , "rules-special" , EDGE_TYPE_OPT, EDGE_KIND_RULES_SPECIAL | EDGE_INVALIDATIONS)                   /* fsent B associated to rules special */           \
  DEF(EDGE_MODULE_SPECIAL, "module-special", EDGE_TYPE_OPT, EDGE_KIND_MODULE_SPECIAL | EDGE_INVALIDATIONS)                  /* fsent B associated to module special */          \
  DEF(EDGE_VAR_SPECIAL   , "var-special"   , EDGE_TYPE_OPT, EDGE_KIND_VAR_SPECIAL | EDGE_INVALIDATIONS)                     /* fsent B associated to var special */             \
  DEF(EDGE_CONFIG_SPECIAL, "config-special", EDGE_TYPE_OPT, EDGE_KIND_CONFIG_SPECIAL | EDGE_INVALIDATIONS)                  /* fsent B associated to config special */          \
  DEF(EDGE_FML_SPECIAL   , "fml-special"   , EDGE_TYPE_OPT, EDGE_KIND_FML_SPECIAL | EDGE_INVALIDATIONS)                     /* fsent B associated to fml special */             \
  DEF(EDGE_INDEX_DIR     , "index-dir"     , EDGE_TYPE_OPT, EDGE_KIND_INDEX_DIR)                                            /* index-dir -> index-root edge */                  \
  DEF(EDGE_INDEX         , "index"         , EDGE_TYPE_OPT, EDGE_KIND_INDEX | EDGE_INVALIDATIONS)                           /* dirent -> index-dir */                           \


typedef enum edge_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_TYPE_TABLE
} edge_type;

/* vertex attrs ranges */
#define VERTEX_STATE_OPT       0x0000000f  /* 00000000 00000000 00000000 00001111 */
#define VERTEX_KIND_OPT        0x000000f0  /* 00000000 00000000 00000000 11110000 */
#define VERTEX_SPECIAL_OPT     0x00000f00  /* 00000000 00000000 00001111 00000000 */
#define VERTEX_SHADOWTYPE_OPT  0x0000f000  /* 00000000 00000000 11110000 00000000 */
#define VERTEX_FILETYPE_OPT    0x000f0000  /* 00000000 00001111 00000000 00000000 */
#define VERTEX_TYPE_OPT        0x00fffff0  /* 00000000 11111111 11111111 11110000 */
#define VERTEX_PROTECT_BIT     0x01000000  /* 00000001 00000000 00000000 00000000 */
#define VERTEX_SHIELD_BIT      0x02000000  /* 00000010 00000000 00000000 00000000 */

/* vertex groups */
#define VERTEX_FSE_BPE         0x00800000  /* 00000000 10000000 00000000 00000000 */

/* vertex options */
#define VERTEX_KIND_TABLE                                                                                             \
  DEF(VERTEX_KIND_FSENT    , "fsent"    , VERTEX_KIND_OPT      , 0x00000010) /* filesystem entity */                  \
  DEF(VERTEX_KIND_SPECIAL  , "special"  , VERTEX_KIND_OPT      , 0x00000020) /* corresponds to a filesystem entity */ \
  DEF(VERTEX_KIND_RULE     , "rule"     , VERTEX_KIND_OPT      , 0x00000030) /* search rule */                        \
  DEF(VERTEX_KIND_RMA      , "rma"      , VERTEX_KIND_OPT      , 0x00000040) /* rule/module association */            \
  DEF(VERTEX_KIND_BPE      , "bpe"      , VERTEX_KIND_OPT      , 0x00000050) /* buildable fsent association */        \
  DEF(VERTEX_KIND_INDEX    , "index"    , VERTEX_KIND_OPT      , 0x00000080) /* vertex-index node */                  \

typedef enum vertex_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_KIND_TABLE
} vertex_kind;

#define VERTEX_FILETYPE_TABLE                                                                     \
  DEF(VERTEX_FILETYPE_REG  , "regular"   , VERTEX_FILETYPE_OPT , 0x00010000) /* regular file */   \
  DEF(VERTEX_FILETYPE_LINK , "link"      , VERTEX_FILETYPE_OPT , 0x00020000) /* symbolic link */  \
  DEF(VERTEX_FILETYPE_DIR  , "dir"       , VERTEX_FILETYPE_OPT , 0x00030000) /* directory */      \

typedef enum vertex_filetype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_FILETYPE_TABLE
} vertex_filetype;

/* special types of fsents with specific logic */
#define VERTEX_SPECIAL_TABLE                                                \
  DEF(VERTEX_SPECIAL_MODULE  , "module" , VERTEX_SPECIAL_OPT , 0x00000100)  \
  DEF(VERTEX_SPECIAL_RULES   , "rules"  , VERTEX_SPECIAL_OPT , 0x00000500)  \
  DEF(VERTEX_SPECIAL_FORMULA , "fml"    , VERTEX_SPECIAL_OPT , 0x00000600)  \
  DEF(VERTEX_SPECIAL_VAR     , "var"    , VERTEX_SPECIAL_OPT , 0x00000700)  \
  DEF(VERTEX_SPECIAL_CONFIG  , "config" , VERTEX_SPECIAL_OPT , 0x00000800)  \


typedef enum vertex_special {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_SPECIAL_TABLE
} vertex_special;

#define VERTEX_SHADOWTYPE_TABLE                                                                                                                       \
  DEF(VERTEX_SHADOWTYPE_FS              , "shadow-fs"              , VERTEX_SHADOWTYPE_OPT, 0x00001000) /* an fsent in the shadow fs */               \
  DEF(VERTEX_SHADOWTYPE_MODULE          , "shadow-module"          , VERTEX_SHADOWTYPE_OPT, 0x00002000) /* shadow directory //modules/id */           \
  DEF(VERTEX_SHADOWTYPE_RULE            , "shadow-rules"           , VERTEX_SHADOWTYPE_OPT, 0x00003000) /* shadow rma //module/id/rules/id */         \
  DEF(VERTEX_SHADOWTYPE_CONTEXT_MODULE  , "shadow-context-module"  , VERTEX_SHADOWTYPE_OPT, 0x00005000) /* the //module node */

typedef enum vertex_shadowtype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_SHADOWTYPE_TABLE
} vertex_shadowtype;

/* all of the different kinds of vertices that can exist in the graph */
/* VERTEX_TYPE_OPT */
#define VERTEX_TYPE_TABLE                                                                                                                                                                      \
  DEF(VERTEX_DIR            , "dir"            , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR)                              /* directory */                       \
  DEF(VERTEX_MODULE_DIR     , "module-dir"     , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SPECIAL_MODULE)     /* module base directory */           \
  DEF(VERTEX_FILE           , "file"           , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG)                              /* regular file */                    \
  DEF(VERTEX_SYMLINK        , "symlink"        , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_LINK)                             /* symlink */                         \
  DEF(VERTEX_MODULE_FILE    , "module-file"    , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_MODULE)     /* module.bam file */                 \
  DEF(VERTEX_RULES_FILE     , "rules-file"     , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_RULES)      /* rules.bam file */                  \
  DEF(VERTEX_FORMULA_FILE   , "formula-file"   , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_FORMULA)    /* formula file */                    \
  DEF(VERTEX_VAR_FILE       , "var-file"       , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_VAR)        /* var.bam file */                    \
  DEF(VERTEX_CONFIG_FILE    , "config-file"    , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_CONFIG)     /* config file */                     \
  DEF(VERTEX_RULE           , "rule"           , VERTEX_TYPE_OPT, VERTEX_KIND_RULE)                                                                      /* search rule */                     \
  DEF(VERTEX_MODULE         , "module"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_MODULE)                      /* a module */                        \
  DEF(VERTEX_FORMULA        , "formula"        , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_FORMULA)                     /* a formula */                       \
  DEF(VERTEX_VAR            , "var"            , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_VAR)                         /* a var */                           \
  DEF(VERTEX_CONFIG         , "config"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_CONFIG)                      /* a config */                        \
  DEF(VERTEX_RULES          , "rules"          , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_RULES)                       /* a set of rules */                  \
  DEF(VERTEX_RMA            , "rma"            , VERTEX_TYPE_OPT, VERTEX_KIND_RMA)                                                                       /* rule/module association */         \
  DEF(VERTEX_BPE            , "bpe"            , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_BPE)                                                      /* buildlan entity */                 \
  DEF(VERTEX_SHADOW_DIR     , "shadow-dir"     , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_FS)      /* shadow fs directory */             \
  DEF(VERTEX_SHADOW_FILE    , "shadow-file"    , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SHADOWTYPE_FS)      /* shadow fs regular file */          \
  DEF(VERTEX_SHADOW_LINK    , "shadow-link"    , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_LINK | VERTEX_SHADOWTYPE_FS)      /* shadow fs symlink */               \
  DEF(VERTEX_SHADOW_MODULE  , "shadow-module"  , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_MODULE)  /* shadow fs //modules/id */          \
  DEF(VERTEX_SHADOW_RULE    , "shadow-rule"    , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_RULE)    /* shadow fs //modules/id/rules/id */ \
  DEF(VERTEX_CONTEXT_MODULE , "context-module" , VERTEX_TYPE_OPT, VERTEX_FSE_BPE | VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOWTYPE_CONTEXT_MODULE)     /* shadow fs //module */   \
  DEF(VERTEX_INDEX          , "index"          , VERTEX_TYPE_OPT, VERTEX_KIND_INDEX)                                                                     /* shadow fs //module */   \

typedef enum vertex_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_TYPE_TABLE
} vertex_type;

/* interior to VERTEX_STATE_OPT */
#define VERTEX_EXISTS_BIT   0x00000001
#define VERTEX_INVALID_BIT  0x00000002
#define VERTEX_UNLINKED_BIT 0x00000004

/* each vertex is in one of the following states */
#define VERTEX_STATE_TABLE                                                                                                                        \
  DEF(VERTEX_OK         , ""  , VERTEX_STATE_OPT,                                            VERTEX_EXISTS_BIT) /* up-to-date */                  \
  DEF(VERTEX_INVALID    , "I" , VERTEX_STATE_OPT,                       VERTEX_INVALID_BIT | VERTEX_EXISTS_BIT) /* exists, but needs updating */  \
  DEF(VERTEX_UNCREATED  , "U" , VERTEX_STATE_OPT,                       VERTEX_INVALID_BIT                    ) /* not yet created */             \
  DEF(VERTEX_UNLINKED   , "X" , VERTEX_STATE_OPT, VERTEX_UNLINKED_BIT | VERTEX_INVALID_BIT                    ) /* unlinked from fs */            \

typedef enum vertex_state {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_STATE_TABLE
} vertex_state;

extern struct attrs32 *graph_vertex_attrs;   /* vertex definitions */
extern struct attrs32 *graph_edge_attrs;     /* edge definitions */

extern struct moria_graph g_graph;      /* the graph */

void graph_setup(void);
void graph_cleanup(void);

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

/* map an internal vertex_type to an external fab_fsent_type */
enum bam_vertex_type vertex_type_remap(vertex_type type);
enum bam_vertex_state vertex_state_remap(vertex_state state);

/* disconnect an fsedge or dependency edge and cascade */
void graph_disintegrate(struct moria_edge * restrict ne, struct graph_invalidation_context * restrict context)
  __attribute__((nonnull(1)));

void graph_disintegrate_bulk(
    struct entlist * restrict edges
  , struct entlist * restrict vertices
  , struct graph_invalidation_context * restrict context
)
  __attribute__((nonnull(1)));

void graph_edge_visitor(moria_edge * e, void * ctx, moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict)
  __attribute__((nonnull));
