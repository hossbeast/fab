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
/* remember that 0x40000000 is MORIA_EDGE_IDENTITY */
#define EDGE_KIND_TABLE                                                                                                      \
  DEF(EDGE_KIND_FSTREE           , "fs"               , EDGE_KIND_OPT, 0x40000010) /* directory : directory entry */         \
  DEF(EDGE_KIND_USES             , "uses"             , EDGE_KIND_OPT, 0x00000020) /* module A uses rules.bam B */           \
  DEF(EDGE_KIND_REQUIRES         , "requires"         , EDGE_KIND_OPT, 0x00000030) /* module A requires module B */          \
  DEF(EDGE_KIND_VARSET           , "varset"           , EDGE_KIND_OPT, 0x00000040) /* */                                     \
  DEF(EDGE_KIND_BPE_TGT          , "bpe-tgt"          , EDGE_KIND_OPT, 0x00000050) /* bpe B -> foo.o A */                    \
  DEF(EDGE_KIND_SRC_BPE          , "src-bpe"          , EDGE_KIND_OPT, 0x00000060) /* foo.c B -> bpe A */                    \
  DEF(EDGE_KIND_DIR_RMA          , "dir-rma"          , EDGE_KIND_OPT, 0x00000070) /* dirent B -> rma A */                   \
  DEF(EDGE_KIND_FSNAME_RMA       , "fsname-rma"       , EDGE_KIND_OPT, 0x00000080) /* fsname B -> rma A */                   \
  DEF(EDGE_KIND_FSNAME_MOD       , "fsname-mod"       , EDGE_KIND_OPT, 0x00000090) /* fsname B -> rma A */                   \
  DEF(EDGE_KIND_FML_RMA          , "fml-rma"          , EDGE_KIND_OPT, 0x000000a0) /* fml B attached to bpe A */             \
  DEF(EDGE_KIND_RULE_RMA         , "rule-rma"         , EDGE_KIND_OPT, 0x000000b0) /* rule B -> rma A */                     \
  DEF(EDGE_KIND_RMA_MOD          , "rma-mod"          , EDGE_KIND_OPT, 0x000000c0) /* rma B -> module A */                   \
  DEF(EDGE_KIND_RMA_TGT          , "rma-tgt"          , EDGE_KIND_OPT, 0x000000d0) /* rma B -> bind target A */              \
  DEF(EDGE_KIND_RMA_BPE          , "rma-bpe"          , EDGE_KIND_OPT, 0x000000e0) /* rma B -> mod A */                      \
  DEF(EDGE_KIND_RMA_RULESET      , "rma-ruleset"      , EDGE_KIND_OPT, 0x000000f0) /* rma B -> ruleset A */                  \
  DEF(EDGE_KIND_SPECIAL          , "special"          , EDGE_KIND_OPT, 0x00000100) /* fsent B -> special A */                \
  DEF(EDGE_KIND_DIR_FSNAME       , "dir-fsname"       , EDGE_KIND_OPT, 0x00000200) /* dir -> fname */                        \
  DEF(EDGE_KIND_FSNAME_FSINDEX   , "fsname-fsindex"   , EDGE_KIND_OPT, 0x40000300) /* fsindex-entry -> fsindex */            \

typedef enum edge_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_KIND_TABLE
} edge_kind;

/* all of the different kinds of edges that can exist in the graph */
#define EDGE_TYPE_TABLE                                                                                                                                                     \
  DEF(EDGE_FSTREE          , "fs"              , EDGE_TYPE_OPT, EDGE_KIND_FSTREE)                                       /* filesystem  */                                   \
  DEF(EDGE_RULESET_MOD     , "ruleset-mod"     , EDGE_TYPE_OPT, EDGE_KIND_USES | EDGE_MODULES | EDGE_INVALIDATIONS)     /* module A uses rules.bam B */                     \
  DEF(EDGE_REQUIRES        , "requires"        , EDGE_TYPE_OPT, EDGE_KIND_REQUIRES | EDGE_MODULES | EDGE_INVALIDATIONS) /* module A requires module B */                    \
  DEF(EDGE_VARSET_MOD      , "varset-mod"      , EDGE_TYPE_OPT, EDGE_KIND_VARSET | EDGE_MODULES | EDGE_INVALIDATIONS)   /* module A loaded var.bam B */                     \
  DEF(EDGE_BPE_TGT         , "bpe-tgt"         , EDGE_TYPE_OPT, EDGE_KIND_BPE_TGT | EDGE_DEPENDS | EDGE_INVALIDATIONS)  /* dependencies */                                  \
  DEF(EDGE_SRC_BPE         , "src-bpe"         , EDGE_TYPE_OPT, EDGE_KIND_SRC_BPE | EDGE_DEPENDS | EDGE_INVALIDATIONS)  /* dependencies */                                  \
  DEF(EDGE_DIR_RMA         , "dir-rma"         , EDGE_TYPE_OPT, EDGE_KIND_DIR_RMA | EDGE_INVALIDATIONS)                 /* rule A matches in directory B */                 \
  DEF(EDGE_FSNAME_RMA      , "fsname-rma"      , EDGE_TYPE_OPT, EDGE_KIND_FSNAME_RMA | EDGE_INVALIDATIONS)              /* bind A looked-up in index-dir B */               \
  DEF(EDGE_FSNAME_MOD      , "fsname-mod"      , EDGE_TYPE_OPT, EDGE_KIND_FSNAME_MOD | EDGE_INVALIDATIONS)              /* module A import looked-up in index-dir B */      \
  DEF(EDGE_DIR_FSNAME      , "dir-fsname"      , EDGE_TYPE_OPT, EDGE_KIND_DIR_FSNAME)                                   /* dir -> index-entry edge */                       \
  DEF(EDGE_FSNAME_FSINDEX  , "fsname-fsindex"  , EDGE_TYPE_OPT, EDGE_KIND_FSNAME_FSINDEX)                               /* fsindex-entry -> fsindex edge */                 \
  DEF(EDGE_FML_RMA         , "fml-bpe"         , EDGE_TYPE_OPT, EDGE_KIND_FML_RMA | EDGE_INVALIDATIONS)                 /* rule A generates formula B */                    \
  DEF(EDGE_RULE_RMA        , "rule-rma"        , EDGE_TYPE_OPT, EDGE_KIND_RULE_RMA)                                     /* comment */                                       \
  DEF(EDGE_RMA_MOD         , "rma-mod"         , EDGE_TYPE_OPT, EDGE_KIND_RMA_MOD)                                      /* comment */                                       \
  DEF(EDGE_RMA_RULESET     , "rma-ruleset"     , EDGE_TYPE_OPT, EDGE_KIND_RMA_RULESET)                                  /* comment */                                       \
  DEF(EDGE_RMA_BPE         , "rma-bpe"         , EDGE_TYPE_OPT, EDGE_KIND_RMA_BPE | EDGE_INVALIDATIONS)                 /* comment */                                       \
  DEF(EDGE_RMA_TGT         , "rma-tgt"         , EDGE_TYPE_OPT, EDGE_KIND_RMA_TGT | EDGE_INVALIDATIONS)                 /* comment */                                       \
  DEF(EDGE_SPECIAL         , "special"         , EDGE_TYPE_OPT, EDGE_KIND_SPECIAL | EDGE_INVALIDATIONS)                 /* B fsent -> A special */                          \
  DEF(EDGE_DIR_MOD         , "dir-mod"         , EDGE_TYPE_OPT, EDGE_KIND_SPECIAL)                                      /* module-dir B -> module A */                      \


typedef enum edge_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_TYPE_TABLE
} edge_type;

/* vertex attrs ranges */
#define VERTEX_STATE_OPT       0x0000000f  /* 00000000 00000000 00000000 00001111 */
#define VERTEX_KIND_OPT        0x00000ff0  /* 00000000 00000000 00001111 11110000 */
#define VERTEX_SPECIAL_OPT     0x0000f000  /* 00000000 00000000 11110000 00000000 */
#define VERTEX_FILETYPE_OPT    0x000f0000  /* 00000000 00001111 00000000 00000000 */
#define VERTEX_SHADOW_BIT      0x01000000  /* 00000001 00000000 00000000 00000000 */
#define VERTEX_TYPE_OPT        0x01fffff0  /* 00000001 11111111 11111111 11110000 */
#define VERTEX_PROTECT_BIT     0x02000000  /* 00000010 00000000 00000000 00000000 */
#define VERTEX_SHIELD_BIT      0x04000000  /* 00000100 00000000 00000000 00000000 */

/* vertex options */
#define VERTEX_KIND_TABLE                                                                                                    \
  DEF(VERTEX_KIND_FSENT          , "fsent"          , VERTEX_KIND_OPT , 0x00000010) /* filesystem entity */                  \
  DEF(VERTEX_KIND_SPECIAL        , "special"        , VERTEX_KIND_OPT , 0x00000020) /* corresponds to a filesystem entity */ \
  DEF(VERTEX_KIND_RULE           , "rule"           , VERTEX_KIND_OPT , 0x00000030) /* rule */                               \
  DEF(VERTEX_KIND_RMA            , "rma"            , VERTEX_KIND_OPT , 0x00000040) /* rule/module association */            \
  DEF(VERTEX_KIND_BPE            , "bpe"            , VERTEX_KIND_OPT , 0x00000050) /* build plan entity */                  \
  DEF(VERTEX_KIND_FSINDEX        , "fsindex"        , VERTEX_KIND_OPT , 0x00000060) /* the fsindex node */                   \
  DEF(VERTEX_KIND_NOTIFY         , "notify"         , VERTEX_KIND_OPT , 0x00000070) /* index view */                         \
  DEF(VERTEX_KIND_FSTREE         , "fstree"         , VERTEX_KIND_OPT , 0x00000080) /* fstree view */                        \
  DEF(VERTEX_KIND_PATH_CACHE     , "path-cache"     , VERTEX_KIND_OPT , 0x00000090) /* path-cache view */                    \
  DEF(VERTEX_KIND_FSNAME         , "fsname"         , VERTEX_KIND_OPT , 0x000000a0) /* fsindex entry */                      \
  DEF(VERTEX_KIND_CONFIG         , "config"         , VERTEX_KIND_OPT , 0x000000b0) /* effective config view */              \
  DEF(VERTEX_KIND_GOALS          , "goals"          , VERTEX_KIND_OPT , 0x000000c0) /* goals view */                         \
  DEF(VERTEX_KIND_BUILD          , "build"          , VERTEX_KIND_OPT , 0x000000d0) /* buildplan view */                     \
  DEF(VERTEX_KIND_BUILDPLAN      , "buildplan"      , VERTEX_KIND_OPT , 0x000000e0) /* buildplan view */                     \
  DEF(VERTEX_KIND_START_PARAMS   , "start-params"   , VERTEX_KIND_OPT , 0x000000f0) /* parameters fixed at startup */        \
  DEF(VERTEX_KIND_RUN_PARAMS     , "run-params"     , VERTEX_KIND_OPT , 0x00000100) /* parameters mutable at runtime */      \
  DEF(VERTEX_KIND_CONTEXT_MODULE , "context-module" , VERTEX_KIND_OPT , 0x00000200) /* request context module */             \
  DEF(VERTEX_KIND_MODULE_VARS    , "module-vars"    , VERTEX_KIND_OPT , 0x00000300) /* effective module vars view */         \
  DEF(VERTEX_KIND_MODULE_ENVS    , "module-envs"    , VERTEX_KIND_OPT , 0x00000400) /* effective module envs view */         \
  DEF(VERTEX_KIND_SYSTEM         , "system"         , VERTEX_KIND_OPT , 0x00000500) /* the system node */                    \
  DEF(VERTEX_KIND_FILESYSTEM     , "filesystem"     , VERTEX_KIND_OPT , 0x00000600) /* the filesystem node */                \

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
#define VERTEX_SPECIAL_TABLE                                                 \
  DEF(VERTEX_SPECIAL_MODULE    , "module"    , VERTEX_SPECIAL_OPT , 0x00001000)  \
  DEF(VERTEX_SPECIAL_RULESET   , "ruleset"   , VERTEX_SPECIAL_OPT , 0x00002000)  \
  DEF(VERTEX_SPECIAL_FORMULA   , "fml"       , VERTEX_SPECIAL_OPT , 0x00003000)  \
  DEF(VERTEX_SPECIAL_VARSET    , "varset"    , VERTEX_SPECIAL_OPT , 0x00004000)  \
  DEF(VERTEX_SPECIAL_CONFIGSET , "configset" , VERTEX_SPECIAL_OPT , 0x00005000)  \
  DEF(VERTEX_SPECIAL_PLUGIN    , "plugin"    , VERTEX_SPECIAL_OPT , 0x00006000)  \


typedef enum vertex_special {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_SPECIAL_TABLE
} vertex_special;

/* all of the different kinds of vertices that can exist in the graph */
/* VERTEX_TYPE_OPT */
#define VERTEX_TYPE_TABLE                                                                                                                                                     \
  DEF(VERTEX_DIR            , "dir"            , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR)                              /* directory */                       \
  DEF(VERTEX_FILE           , "file"           , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG)                              /* regular file */                    \
  DEF(VERTEX_SYMLINK        , "symlink"        , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_LINK)                             /* symlink */                         \
  DEF(VERTEX_SHADOW_DIR     , "shadow-dir"     , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOW_BIT)         /* shadow fs directory */             \
  DEF(VERTEX_SHADOW_FILE    , "shadow-file"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SHADOW_BIT)         /* shadow fs regular file */          \
  DEF(VERTEX_SHADOW_LINK    , "shadow-link"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_LINK | VERTEX_SHADOW_BIT)         /* shadow fs symlink */               \
  DEF(VERTEX_MODULE_DIR     , "module-dir"     , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SPECIAL_MODULE)     /* module base directory */           \
  DEF(VERTEX_MODULE_FILE    , "module-file"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_MODULE)     /* module.bam file */                 \
  DEF(VERTEX_RULESET_FILE   , "ruleset-file"   , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_RULESET)    /* rules.bam file */                  \
  DEF(VERTEX_FORMULA_FILE   , "formula-file"   , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_FORMULA)    /* formula file */                    \
  DEF(VERTEX_VARSET_FILE    , "varset-file"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_VARSET)     /* var.bam file */                    \
  DEF(VERTEX_CONFIGSET_FILE , "configset-file" , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_CONFIGSET)  /* config file */                     \
  DEF(VERTEX_PLUGIN_FILE    , "plugin-file"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SPECIAL_PLUGIN)     /* plugin file */                     \
  DEF(VERTEX_MODULE         , "module"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_MODULE)     /* a module */                        \
  DEF(VERTEX_RULESET        , "ruleset"        , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_RULESET)    /* a set of rules */                  \
  DEF(VERTEX_FORMULA        , "formula"        , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_FORMULA)    /* a formula */                       \
  DEF(VERTEX_VARSET         , "varset"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_VARSET)     /* a var */                           \
  DEF(VERTEX_CONFIGSET      , "configset"      , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_CONFIGSET)  /* a config */                        \
  DEF(VERTEX_PLUGIN         , "plugin"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_PLUGIN)     /* a plugin  */                       \
  DEF(VERTEX_PATH_CACHE     , "path-cache"     , VERTEX_TYPE_OPT, VERTEX_KIND_PATH_CACHE)                                               /* linked @ //path-cache */           \
  DEF(VERTEX_FSNAME         , "fsname"         , VERTEX_TYPE_OPT, VERTEX_KIND_FSNAME)                                                   /* linked @ //fsindex/name */         \
  DEF(VERTEX_CONFIG         , "config"         , VERTEX_TYPE_OPT, VERTEX_KIND_CONFIG)                                                   /* linked @ //config */               \
  DEF(VERTEX_GOALS          , "goals"          , VERTEX_TYPE_OPT, VERTEX_KIND_GOALS)                                                    /* linked @ //goals */                \
  DEF(VERTEX_BUILD          , "build"          , VERTEX_TYPE_OPT, VERTEX_KIND_BUILD)                                                    /* linked @ //build */                \
  DEF(VERTEX_BUILDPLAN      , "buildplan"      , VERTEX_TYPE_OPT, VERTEX_KIND_BUILDPLAN)                                                /* linked @ //buildplan */            \
  DEF(VERTEX_CONTEXT_MODULE , "context-module" , VERTEX_TYPE_OPT, VERTEX_KIND_CONTEXT_MODULE)                                           /* linked @ //module */               \
  DEF(VERTEX_MODULE_VARS    , "module-vars"    , VERTEX_TYPE_OPT, VERTEX_KIND_MODULE_VARS)                                              /* //module/id/vars */                \
  DEF(VERTEX_MODULE_ENVS    , "module-envs"    , VERTEX_TYPE_OPT, VERTEX_KIND_MODULE_ENVS)                                              /* //module/id/envs */                \
  DEF(VERTEX_RULE           , "rule"           , VERTEX_TYPE_OPT, VERTEX_KIND_RULE)                                                     /* a rule */                          \
  DEF(VERTEX_RMA            , "rma"            , VERTEX_TYPE_OPT, VERTEX_KIND_RMA)                                                      /* an rule/module association */      \
  DEF(VERTEX_BPE            , "bpe"            , VERTEX_TYPE_OPT, VERTEX_KIND_BPE)                                                      /* a buildlan entity */               \
  DEF(VERTEX_FSINDEX        , "fsindex"        , VERTEX_TYPE_OPT, VERTEX_KIND_FSINDEX)                                                  /* //fsindex */                       \
  DEF(VERTEX_NOTIFY         , "notify"         , VERTEX_TYPE_OPT, VERTEX_KIND_NOTIFY)                                                   /* */                                 \
  DEF(VERTEX_FSTREE         , "fstree"         , VERTEX_TYPE_OPT, VERTEX_KIND_FSTREE)                                                   /* an fstree vertex */                \
  DEF(VERTEX_SYSTEM         , "system"         , VERTEX_TYPE_OPT, VERTEX_KIND_SYSTEM)                                                                                         \
  DEF(VERTEX_FILESYSTEM     , "filesystem"     , VERTEX_TYPE_OPT, VERTEX_KIND_FILESYSTEM)                                                                                     \


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
  DEF(VERTEX_OK         , ""  , "fresh"     , VERTEX_STATE_OPT ,                                            VERTEX_EXISTS_BIT) /* up-to-date */                  \
  DEF(VERTEX_INVALID    , "I" , "stale"     , VERTEX_STATE_OPT ,                       VERTEX_INVALID_BIT | VERTEX_EXISTS_BIT) /* exists, but needs updating */  \
  DEF(VERTEX_UNCREATED  , "U" , "uncreated" , VERTEX_STATE_OPT ,                       VERTEX_INVALID_BIT                    ) /* not yet created */             \
  DEF(VERTEX_UNLINKED   , "X" , "unlinked"  , VERTEX_STATE_OPT , VERTEX_UNLINKED_BIT | VERTEX_INVALID_BIT                    ) /* unlinked from fs */            \

typedef enum vertex_state {
#undef DEF
#define DEF(x, s, n, r, y) x = y,
VERTEX_STATE_TABLE
} vertex_state;

extern struct attrs32 *graph_vertex_attrs;   /* vertex definitions */
extern struct attrs32 *graph_edge_attrs;     /* edge definitions */

extern struct attrs32 *graph_vertex_type_attrs;   /* vertex type names */
extern struct attrs32 *graph_vertex_state_attrs;  /* vertex state names */

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
