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
struct edges;
struct vertices;
enum bam_vertex_type;
struct entlist;

/* edge attrs ranges */
#define EDGE_KIND_OPT           0x40000ff0 /* 01000000 00000000 00001111 11110000 */
#define EDGE_GROUPS_OPT         0x00ff0000 /* 00000000 11111111 00000000 00000000 */
#define EDGE_TYPE_OPT           0x40fffff0 /* 01000000 11111111 11111111 11110000 */

/* edge groups */
#define EDGE_INVALIDATIONS      0x00010000 /* invalidations propagate upwards along these edges */
#define EDGE_DEPENDS            0x00020000 /* edges between fsents and bpes, created by running rules */
#define EDGE_MODULES            0x00040000 /* module A uses, imports, var B edges */
#define EDGE_SPECIALS           0x00080000 /* specials graph */
#define EDGE_UNLINK             0x00100000
#define EDGE_RECORDS            0x00200000
#define EDGE_FSTREE_SYMLINKS    0x00400000 /* fstree edges and symlink refs */

/* edge options */
/* remember that 0x40000000 is MORIA_EDGE_IDENTITY */
/* remember that 0x80000000 is MORIA_EDGE_HYPER */
#define EDGE_KIND_TABLE                                                                                                        \
  DEF(EDGE_KIND_FS                , "fs"                , EDGE_KIND_OPT, 0x40000010) /* directory : directory entry */         \
  DEF(EDGE_KIND_RULESET_USES_MOD  , "ruleset-uses-mod"  , EDGE_KIND_OPT, 0x00000020) /* rules.bam B -> module A via uses */    \
  DEF(EDGE_KIND_VARSET            , "varset"            , EDGE_KIND_OPT, 0x00000040) /* */                                     \
  DEF(EDGE_KIND_BPE_TGT           , "bpe-tgt"           , EDGE_KIND_OPT, 0x00000050) /* bpe B -> foo.o A */                    \
  DEF(EDGE_KIND_SRC_BPE           , "src-bpe"           , EDGE_KIND_OPT, 0x00000060) /* foo.c B -> bpe A */                    \
  DEF(EDGE_KIND_DIR_RMA           , "dir-rma"           , EDGE_KIND_OPT, 0x00000070) /* dirent B -> rma A */                   \
  DEF(EDGE_KIND_FSNAME_RMA        , "fsname-rma"        , EDGE_KIND_OPT, 0x00000080) /* fsname B -> rma A */                   \
  DEF(EDGE_KIND_FSNAME_MOD        , "fsname-mod"        , EDGE_KIND_OPT, 0x00000090) /* fsname B -> rma A */                   \
  DEF(EDGE_KIND_FML_RMA           , "fml-rma"           , EDGE_KIND_OPT, 0x000000a0) /* fml B attached to rma A */             \
  DEF(EDGE_KIND_FML_BPE           , "fml-bpe"           , EDGE_KIND_OPT, 0x000000b0) /* fml B attached to bpe A */             \
  DEF(EDGE_KIND_RULE_RMA          , "rule-rma"          , EDGE_KIND_OPT, 0x000000c0) /* rule B -> rma A */                     \
  DEF(EDGE_KIND_RMA_MOD           , "rma-mod"           , EDGE_KIND_OPT, 0x000000d0) /* rma B -> module A */                   \
  DEF(EDGE_KIND_RMA_BIND          , "rma-bind"          , EDGE_KIND_OPT, 0x000000e0) /* rma B -> bind target A */              \
  DEF(EDGE_KIND_RMA_BPE           , "rma-bpe"           , EDGE_KIND_OPT, 0x000000f0) /* rma B -> mod A */                      \
  DEF(EDGE_KIND_RULE_RULESET      , "rule-ruleset"      , EDGE_KIND_OPT, 0x00000100) /* rule B -> ruleset A */                 \
  DEF(EDGE_KIND_RULE_MOD          , "rule-mod"          , EDGE_KIND_OPT, 0x00000110) /* rule B -> module A */                  \
  DEF(EDGE_KIND_DIR_FSNAME        , "dir-fsname"        , EDGE_KIND_OPT, 0x00000120) /* dir -> fname */                        \
  DEF(EDGE_KIND_FSNAME_NS         , "fsname-ns"         , EDGE_KIND_OPT, 0x40000130) /* namespace-entry -> namespace */        \
  DEF(EDGE_KIND_FSTREE_NS         , "fstree-ns"         , EDGE_KIND_OPT, 0x00000140) /* fstree -> namespace */                 \
  DEF(EDGE_KIND_NS_RMA            , "ns-rma"            , EDGE_KIND_OPT, 0x00000150) /* lookup operations */                   \
  DEF(EDGE_KIND_NS_PLUGIN         , "ns-plugin"         , EDGE_KIND_OPT, 0x00000160) /* lookup operations */                   \
  DEF(EDGE_KIND_FSNAME_PLUGIN     , "fsname-plugin"     , EDGE_KIND_OPT, 0x00000170) /* lookup operations */                   \
  DEF(EDGE_KIND_SHADOW_DIR_PLUGIN , "shadow-dir-plugin" , EDGE_KIND_OPT, 0x00000180)                                           \
  DEF(EDGE_KIND_SHADOW_DIR_BPE    , "shadow-dir-bpe"    , EDGE_KIND_OPT, 0x00000190)                                           \
  DEF(EDGE_KIND_LOOKUP            , "lookup"            , EDGE_KIND_OPT, 0x000001a0) /* lookup operations */                   \
  DEF(EDGE_KIND_LOOKUP_PLUGIN_FILE, "lookup-plugin-file", EDGE_KIND_OPT, 0x000001b0) /* lookup operations */                   \
  DEF(EDGE_KIND_SHADOW_DIR_MOD    , "shadow-dir-mod"    , EDGE_KIND_OPT, 0x000001c0)                                           \
  DEF(EDGE_KIND_SHADOW_DIR_NS     , "shadow-dir-ns"     , EDGE_KIND_OPT, 0x000001d0)                                           \
  DEF(EDGE_KIND_SPECIAL           , "special"           , EDGE_KIND_OPT, 0x000001e0)                                           \
  DEF(EDGE_KIND_RULESET_BUILD_MOD , "ruleset-build-mod" , EDGE_KIND_OPT, 0x000001f0)                                           \
  DEF(EDGE_KIND_RULESET_ANCHOR_MOD, "ruleset-anchor-mod", EDGE_KIND_OPT, 0x00000200)                                           \
  DEF(EDGE_KIND_ANCHOR_DIR_MOD    , "anchor-dir-mod"    , EDGE_KIND_OPT, 0x00000210)                                           \
  DEF(EDGE_KIND_BUILD_DIR_MOD     , "build-dir-mod"     , EDGE_KIND_OPT, 0x00000220)                                           \
  DEF(EDGE_KIND_FSENT_FML         , "fsent-fml"         , EDGE_KIND_OPT, 0x00000230)                                           \
  DEF(EDGE_KIND_FSENT_VARSET      , "fsent-varset"      , EDGE_KIND_OPT, 0x00000240)                                           \
  DEF(EDGE_KIND_FSENT_CONFIGSET   , "fsent-configset"   , EDGE_KIND_OPT, 0x00000250)                                           \
  DEF(EDGE_KIND_FSENT_RULESET     , "fsent-ruleset"     , EDGE_KIND_OPT, 0x00000260)                                           \
  DEF(EDGE_KIND_FML_REC_BPE       , "fml-rec-bpe"       , EDGE_KIND_OPT, 0x00000270)                                           \
  DEF(EDGE_KIND_FML_REC_BUILD_REC , "fml-rec-build-rec" , EDGE_KIND_OPT, 0x00000280)                                           \
  DEF(EDGE_KIND_LINK_REF          , "link-ref"          , EDGE_KIND_OPT, 0x00000290)                                           \
  DEF(EDGE_KIND_REF_LINK          , "ref-link"          , EDGE_KIND_OPT, 0x000002a0)                                           \

typedef enum edge_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_KIND_TABLE
} edge_kind;

/* all of the different kinds of edges that can exist in the graph */
#define EDGE_TYPE_TABLE                                                                                                                                                                          \
  DEF(EDGE_FS                 , "fs"                 , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_FS | EDGE_FSTREE_SYMLINKS)                    /* filesystem  */                                    \
  DEF(EDGE_RULESET_USES_MOD   , "ruleset-uses-mod"   , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_RULESET_USES_MOD | EDGE_MODULES | EDGE_INVALIDATIONS)     /* rules.bam B -> module A via uses */   \
  DEF(EDGE_VARSET_MOD         , "varset-mod"         , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_VARSET | EDGE_MODULES | EDGE_INVALIDATIONS)   /* module A loaded var.bam B */                      \
  DEF(EDGE_BPE_TGT            , "bpe-tgt"            , EDGE_TYPE_OPT,               EDGE_KIND_BPE_TGT | EDGE_RECORDS | EDGE_DEPENDS | EDGE_INVALIDATIONS)  /* dependencies */                    \
  DEF(EDGE_SRC_BPE            , "src-bpe"            , EDGE_TYPE_OPT,               EDGE_KIND_SRC_BPE | EDGE_RECORDS | EDGE_DEPENDS | EDGE_INVALIDATIONS)  /* dependencies */                    \
  DEF(EDGE_DIR_RMA            , "dir-rma"            , EDGE_TYPE_OPT,               EDGE_KIND_DIR_RMA | EDGE_INVALIDATIONS)                 /* rule A matches in directory B */                  \
  DEF(EDGE_NS_RMA             , "ns-rma"             , EDGE_TYPE_OPT,               EDGE_KIND_NS_RMA | EDGE_INVALIDATIONS)                  /*  */                                               \
  DEF(EDGE_FSNAME_RMA         , "fsname-rma"         , EDGE_TYPE_OPT,               EDGE_KIND_FSNAME_RMA | EDGE_INVALIDATIONS)              /* bind A looked-up in index-dir B */                \
  DEF(EDGE_FSNAME_MOD         , "fsname-mod"         , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_FSNAME_MOD | EDGE_INVALIDATIONS)              /* module A import looked-up in index-dir B */       \
  DEF(EDGE_DIR_FSNAME         , "dir-fsname"         , EDGE_TYPE_OPT,               EDGE_KIND_DIR_FSNAME | EDGE_INVALIDATIONS)               /* dir -> index-entry edge */                       \
  DEF(EDGE_FSNAME_NS          , "fsname-ns"          , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_FSNAME_NS)                                    /* namespace-entry -> namespace edge */              \
  DEF(EDGE_FML_BPE            , "fml-bpe"            , EDGE_TYPE_OPT,               EDGE_KIND_FML_BPE | EDGE_INVALIDATIONS)                 /* */                                                \
  DEF(EDGE_FML_RMA            , "fml-rma"            , EDGE_TYPE_OPT,               EDGE_KIND_FML_RMA | EDGE_INVALIDATIONS)                 /* rule A generates formula B */                     \
  DEF(EDGE_RULE_RMA           , "rule-rma"           , EDGE_TYPE_OPT,               EDGE_KIND_RULE_RMA)                                     /* comment */                                        \
  DEF(EDGE_RMA_MOD            , "rma-mod"            , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_RMA_MOD)                                      /* comment */                                        \
  DEF(EDGE_RULE_RULESET       , "rule-ruleset"       , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_RULE_RULESET)                                 /* comment */                                        \
  DEF(EDGE_RMA_BPE            , "rma-bpe"            , EDGE_TYPE_OPT,               EDGE_KIND_RMA_BPE)                                      /* comment */                                        \
  DEF(EDGE_RMA_BIND           , "rma-bind"           , EDGE_TYPE_OPT,               EDGE_KIND_RMA_BIND | EDGE_INVALIDATIONS)                /* comment */                                        \
  DEF(EDGE_FSTREE_NS          , "fstree-ns"          , EDGE_TYPE_OPT, EDGE_UNLINK)                                                                                                               \
  DEF(EDGE_NS_PLUGIN          , "ns-plugin"          , EDGE_TYPE_OPT,               EDGE_KIND_NS_PLUGIN | EDGE_INVALIDATIONS)                                                                    \
  DEF(EDGE_FSNAME_PLUGIN      , "fsname-plugin"      , EDGE_TYPE_OPT,               EDGE_KIND_FSNAME_PLUGIN | EDGE_INVALIDATIONS)                                                                \
  DEF(EDGE_LOOKUP             , "lookup"             , EDGE_TYPE_OPT,               EDGE_KIND_LOOKUP)                                                                                            \
  DEF(EDGE_LOOKUP_PLUGIN_FILE , "lookup-plugin-file" , EDGE_TYPE_OPT,               EDGE_KIND_LOOKUP_PLUGIN_FILE)                                                                                \
  DEF(EDGE_REF_SYMLINK        , "ref-symlink"        , EDGE_TYPE_OPT,               EDGE_KIND_REF_LINK | EDGE_INVALIDATIONS | EDGE_FSTREE_SYMLINKS)                                              \
  DEF(EDGE_SHADOW_REF         , "shadow-ref"         , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_KIND_LINK_REF)                                                                                          \
  DEF(EDGE_FML_REC_BPE        , "fml-rec-bpe"        , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_RECORDS | EDGE_KIND_FML_REC_BPE)                                                                        \
  DEF(EDGE_FML_REC_BUILD_REC  , "fml-rec-build-rec"  , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_RECORDS | EDGE_KIND_FML_REC_BUILD_REC)                                                                  \
  DEF(EDGE_SPECIAL            , "special"            , EDGE_TYPE_OPT, EDGE_UNLINK | EDGE_SPECIALS | EDGE_KIND_SPECIAL)                                                                           \
  DEF(EDGE_RULESET_BUILD_MOD  , "ruleset-build-mod"  , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_RULESET_BUILD_MOD | EDGE_INVALIDATIONS)                                            \
  DEF(EDGE_RULESET_ANCHOR_MOD , "ruleset-anchor-mod" , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_RULESET_ANCHOR_MOD | EDGE_INVALIDATIONS)                                           \
  DEF(EDGE_ANCHOR_DIR_MOD     , "anchor-dir-mod"     , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_ANCHOR_DIR_MOD)                                                                    \
  DEF(EDGE_BUILD_DIR_MOD      , "build-dir-mod"      , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_BUILD_DIR_MOD)                                                                     \
  DEF(EDGE_FSENT_FML          , "fsent-fml"          , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_FSENT_FML | EDGE_INVALIDATIONS)                                                    \
  DEF(EDGE_FSENT_VARSET       , "fsent-varset"       , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_FSENT_VARSET | EDGE_INVALIDATIONS)                                                 \
  DEF(EDGE_FSENT_CONFIGSET    , "fsent-configset"    , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_FSENT_CONFIGSET | EDGE_INVALIDATIONS)                                              \
  DEF(EDGE_FSENT_RULESET      , "fsent-ruleset"      , EDGE_TYPE_OPT,               EDGE_SPECIALS | EDGE_KIND_FSENT_RULESET | EDGE_INVALIDATIONS)                                                \


typedef enum edge_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
EDGE_TYPE_TABLE
} edge_type;

/* vertex attrs ranges - 32 bits
 *  the top bit is reserved
 *   MORIA_VERTEX_LINK 0x80000000 vertex->ref is set
 */
/*                           TYPE UNUSED      01111100 00000000 00000000 00000000 */
#define VERTEX_STATE_OPT       0x00000007  /* 00000000 00000000 00000000 00000111 */
#define VERTEX_TYPE_OPT        0x7ffffff8  /* 01111111 11111111 11111111 11111000 */
#define VERTEX_KIND_OPT        0x00000ff0  /* 00000000 00000000 00001111 11110000 */
#define VERTEX_SPECIAL_OPT     0x0000f000  /* 00000000 00000000 11110000 00000000 */
#define VERTEX_FILETYPE_OPT    0x00070000  /* 00000000 00000111 00000000 00000000 */
#define VERTEX_TYPE2_OPT       0x00380000  /* 00000000 00111000 00000000 00000000 */
#define VERTEX_TYPE3_OPT       0x01c00008  /* 00000001 11000000 00000000 00001000 */
#define VERTEX_SHADOW_BIT      0x02000000  /* 00000010 00000000 00000000 00000000 */

/* vertex options */
#define VERTEX_KIND_TABLE                                            /* 0x00000ff0 */                                              \
  DEF(VERTEX_KIND_FSENT          , "fsent"          , VERTEX_KIND_OPT , 0x00000010) /* filesystem entity */                        \
  DEF(VERTEX_KIND_SPECIAL        , "special"        , VERTEX_KIND_OPT , 0x00000020) /* corresponds to a filesystem entity */       \
  DEF(VERTEX_KIND_RULE           , "rule"           , VERTEX_KIND_OPT , 0x00000030) /* rule */                                     \
  DEF(VERTEX_KIND_RMA            , "rma"            , VERTEX_KIND_OPT , 0x00000040) /* rule/module association */                  \
  DEF(VERTEX_KIND_BPE            , "bpe"            , VERTEX_KIND_OPT , 0x00000050) /* build plan entity */                        \
  DEF(VERTEX_KIND_NAMESPACE      , "namespace"      , VERTEX_KIND_OPT , 0x00000060) /* an namespace node */                        \
  DEF(VERTEX_KIND_FSTREE         , "fstree"         , VERTEX_KIND_OPT , 0x00000070) /* fstree view */                              \
  DEF(VERTEX_KIND_FSNAME         , "fsname"         , VERTEX_KIND_OPT , 0x00000080) /* namespace entry */                          \
  DEF(VERTEX_KIND_SYS_SYSTEM     , "sys-system"     , VERTEX_KIND_OPT , 0x00000090) /* the system node */                          \
  DEF(VERTEX_KIND_SYS_CONFIG     , "sys-config"     , VERTEX_KIND_OPT , 0x000000a0) /* config subsystem */                         \
  DEF(VERTEX_KIND_SYS_NOTIFY     , "sys-notify"     , VERTEX_KIND_OPT , 0x000000b0) /* index view */                               \
  DEF(VERTEX_KIND_SYS_BUILD      , "sys-build"      , VERTEX_KIND_OPT , 0x000000c0) /* the build */                                \
  DEF(VERTEX_KIND_SYS_GOALS      , "sys-goals"      , VERTEX_KIND_OPT , 0x000000d0) /* goals view */                               \
  DEF(VERTEX_KIND_SYS_PLAN       , "sys-plan"       , VERTEX_KIND_OPT , 0x000000e0) /* the plan */                                 \
  DEF(VERTEX_KIND_SYS_FILESYSTEM , "sys-filesystem" , VERTEX_KIND_OPT , 0x000000f0) /* the filesystem node */                      \
  DEF(VERTEX_KIND_SYS_RCU        , "sys-rcu"        , VERTEX_KIND_OPT , 0x00000100) /* the rcu node */                             \
  DEF(VERTEX_KIND_START_PARAMS   , "start-params"   , VERTEX_KIND_OPT , 0x00000110) /* parameters fixed at startup */              \
  DEF(VERTEX_KIND_RUN_PARAMS     , "run-params"     , VERTEX_KIND_OPT , 0x00000120) /* parameters mutable at runtime */            \
  DEF(VERTEX_KIND_CONTEXT_MODULE , "context-module" , VERTEX_KIND_OPT , 0x00000130) /* request context module */                   \
  DEF(VERTEX_KIND_MODULE_VARS    , "module-vars"    , VERTEX_KIND_OPT , 0x00000140) /* effective module vars view */               \
  DEF(VERTEX_KIND_MODULE_ENVS    , "module-envs"    , VERTEX_KIND_OPT , 0x00000150) /* effective module envs view */               \
  DEF(VERTEX_KIND_CLIENT         , "client"         , VERTEX_KIND_OPT , 0x00000160) /* client */                                   \
  DEF(VERTEX_KIND_EXEC           , "exec"           , VERTEX_KIND_OPT , 0x00000170) /* process execution */                        \
  DEF(VERTEX_KIND_RECORD         , "record"         , VERTEX_KIND_OPT , 0x00000180) /* build or other record */                    \

typedef enum vertex_kind {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_KIND_TABLE
} vertex_kind;

#define VERTEX_FILETYPE_TABLE                                   /*  0x00070000  */                   \
  DEF(VERTEX_FILETYPE_REG     , "file"      , VERTEX_FILETYPE_OPT , 0x00010000) /* regular file */   \
  DEF(VERTEX_FILETYPE_SYMLINK , "symlink"   , VERTEX_FILETYPE_OPT , 0x00020000) /* symbolic link */  \
  DEF(VERTEX_FILETYPE_DIR     , "dir"       , VERTEX_FILETYPE_OPT , 0x00030000) /* directory */      \

#define VERTEX_FILETYPE_DIR_OR_SYMLINK 0x00020000

typedef enum vertex_filetype {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_FILETYPE_TABLE
} vertex_filetype;

#define VERTEX_TYPE2_TABLE                               /* 0x00380000 */                         \
  DEF(VERTEX_TYPE2_BUILD   , "build"   , VERTEX_TYPE2_OPT , 0x00080000) /* build execution */     \
  DEF(VERTEX_TYPE2_FORMULA , "formula" , VERTEX_TYPE2_OPT , 0x00100000) /* formula execution */   \
  DEF(VERTEX_TYPE2_PLUGIN  , "plugin"  , VERTEX_TYPE2_OPT , 0x00180000) /* plugin execution */    \

typedef enum vertex_type2 {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_TYPE2_TABLE
} vertex_type2;

#define VERTEX_TYPE3_TABLE                                /* 0x01c00008  */                \
  DEF(VERTEX_TYPE3_STDIN   , "stdin"   , VERTEX_TYPE3_OPT  , 0x00000008) /* stdin */       \
  DEF(VERTEX_TYPE3_STDOUT  , "stdout"  , VERTEX_TYPE3_OPT  , 0x00400000) /* stdout */      \
  DEF(VERTEX_TYPE3_STDERR  , "stderr"  , VERTEX_TYPE3_OPT  , 0x00400008) /* stderr */      \
  DEF(VERTEX_TYPE3_AUXOUT  , "auxout"  , VERTEX_TYPE3_OPT  , 0x00800000) /* auxout */      \
  DEF(VERTEX_TYPE3_ARGS    , "args"    , VERTEX_TYPE3_OPT  , 0x00800008) /* args */        \
  DEF(VERTEX_TYPE3_ENVS    , "envs"    , VERTEX_TYPE3_OPT  , 0x00c00000) /* envs */        \
  DEF(VERTEX_TYPE3_FILE    , "file"    , VERTEX_TYPE3_OPT  , 0x00c00008) /* file */        \
  DEF(VERTEX_TYPE3_CWD     , "cwd"     , VERTEX_TYPE3_OPT  , 0x01000000) /* cwd */         \

typedef enum vertex_type3 {
#undef DEF
#define DEF(x, s, r, y) x = y,
VERTEX_TYPE3_TABLE
} vertex_type3;

/* special types of fsents with specific logic */
#define VERTEX_SPECIAL_TABLE                                    /*  0x0000f000 */ \
  DEF(VERTEX_SPECIAL_MODULE    , "module"    , VERTEX_SPECIAL_OPT , 0x00001000)   \
  DEF(VERTEX_SPECIAL_RULESET   , "ruleset"   , VERTEX_SPECIAL_OPT , 0x00002000)   \
  DEF(VERTEX_SPECIAL_FORMULA   , "fml"       , VERTEX_SPECIAL_OPT , 0x00003000)   \
  DEF(VERTEX_SPECIAL_VARSET    , "varset"    , VERTEX_SPECIAL_OPT , 0x00004000)   \
  DEF(VERTEX_SPECIAL_CONFIGSET , "configset" , VERTEX_SPECIAL_OPT , 0x00005000)   \
  DEF(VERTEX_SPECIAL_PLUGIN    , "plugin"    , VERTEX_SPECIAL_OPT , 0x00006000)   \


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
  DEF(VERTEX_SYMLINK        , "symlink"        , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_SYMLINK)                          /* symlink */                         \
  DEF(VERTEX_SHADOW_DIR     , "shadow-dir"     , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_DIR  | VERTEX_SHADOW_BIT)         /* shadow fs directory */             \
  DEF(VERTEX_SHADOW_FILE    , "shadow-file"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_REG  | VERTEX_SHADOW_BIT)         /* shadow fs regular file */          \
  DEF(VERTEX_SHADOW_LINK    , "shadow-link"    , VERTEX_TYPE_OPT, VERTEX_KIND_FSENT | VERTEX_FILETYPE_SYMLINK | VERTEX_SHADOW_BIT)      /* shadow fs symlink */               \
  DEF(VERTEX_MODULE         , "module"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_MODULE)     /* a module */                        \
  DEF(VERTEX_RULESET        , "ruleset"        , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_RULESET)    /* a set of rules */                  \
  DEF(VERTEX_FORMULA        , "formula"        , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_FORMULA)    /* a formula */                       \
  DEF(VERTEX_VARSET         , "varset"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_VARSET)     /* a var */                           \
  DEF(VERTEX_CONFIGSET      , "configset"      , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_CONFIGSET)  /* a config */                        \
  DEF(VERTEX_PLUGIN         , "plugin"         , VERTEX_TYPE_OPT, VERTEX_KIND_SPECIAL                      | VERTEX_SPECIAL_PLUGIN)     /* a plugin  */                       \
  DEF(VERTEX_FSNAME         , "fsname"         , VERTEX_TYPE_OPT, VERTEX_KIND_FSNAME)                                                   /* linked @ //namespace/name */       \
  DEF(VERTEX_SYS_SYSTEM     , "sys-system"     , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_SYSTEM)                                               /* //system */                        \
  DEF(VERTEX_SYS_CONFIG     , "sys-config"     , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_CONFIG)                                               /* linked @ //config */               \
  DEF(VERTEX_SYS_NOTIFY     , "sys-notify"     , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_NOTIFY)                                               /* */                                 \
  DEF(VERTEX_SYS_BUILD      , "sys-build"      , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_BUILD)                                                /* linked @ //build */                \
  DEF(VERTEX_SYS_GOALS      , "sys-goals"      , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_GOALS)                                                /* linked @ //goals */                \
  DEF(VERTEX_SYS_PLAN       , "sys-plan"       , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_PLAN)                                                 /* linked @ //plan */                 \
  DEF(VERTEX_SYS_FILESYSTEM , "sys-filesystem" , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_FILESYSTEM)                                           /* //filesystem */                    \
  DEF(VERTEX_SYS_RCU        , "sys-rcu"        , VERTEX_TYPE_OPT, VERTEX_KIND_SYS_RCU)                                                  /* //rcu */                           \
  DEF(VERTEX_CONTEXT_MODULE , "context-module" , VERTEX_TYPE_OPT, VERTEX_KIND_CONTEXT_MODULE)                                           /* linked @ //module */               \
  DEF(VERTEX_MODULE_VARS    , "module-vars"    , VERTEX_TYPE_OPT, VERTEX_KIND_MODULE_VARS)                                              /* //module/id/vars */                \
  DEF(VERTEX_MODULE_ENVS    , "module-envs"    , VERTEX_TYPE_OPT, VERTEX_KIND_MODULE_ENVS)                                              /* //module/id/envs */                \
  DEF(VERTEX_RULE           , "rule"           , VERTEX_TYPE_OPT, VERTEX_KIND_RULE)                                                     /* a rule */                          \
  DEF(VERTEX_RMA            , "rma"            , VERTEX_TYPE_OPT, VERTEX_KIND_RMA)                                                      /* an rule/module association */      \
  DEF(VERTEX_BPE            , "bpe"            , VERTEX_TYPE_OPT, VERTEX_KIND_BPE)                                                      /* a buildlan entity */               \
  DEF(VERTEX_NAMESPACE      , "namespace"      , VERTEX_TYPE_OPT, VERTEX_KIND_NAMESPACE)                                                /* //namespace/namespace */           \
  DEF(VERTEX_FSTREE         , "fstree"         , VERTEX_TYPE_OPT, VERTEX_KIND_FSTREE)                                                   /* //fstrees/<id> */                  \
  DEF(VERTEX_CLIENT         , "client"         , VERTEX_TYPE_OPT, VERTEX_KIND_CLIENT)                                                   /* a client */                        \
  DEF(VERTEX_BUILD_RECORD   , "build-record"   , VERTEX_TYPE_OPT, VERTEX_KIND_RECORD | VERTEX_TYPE2_BUILD )                                             /* //builds/<id> */   \
  DEF(VERTEX_FORMULA_RECORD , "formula-record" , VERTEX_TYPE_OPT, VERTEX_KIND_RECORD | VERTEX_TYPE2_FORMULA)                                                                  \
  DEF(VERTEX_PLUGIN_STDIN   , "plugin-stdin"   , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_PLUGIN | VERTEX_TYPE3_STDIN)          /* //plugins/<id>/stdin */            \
  DEF(VERTEX_PLUGIN_STDOUT  , "plugin-stdout"  , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_PLUGIN | VERTEX_TYPE3_STDOUT)         /* //plugins/<id>/stdout */           \
  DEF(VERTEX_PLUGIN_STDERR  , "plugin-stderr"  , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_PLUGIN | VERTEX_TYPE3_STDERR)         /* //plugins/<id>/stderr */           \
  DEF(VERTEX_PLUGIN_AUXOUT  , "plugin-auxout"  , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_PLUGIN | VERTEX_TYPE3_AUXOUT)         /* //plugins/<id>/auxout */           \
  DEF(VERTEX_FORMULA_STDIN  , "formula-stdin"  , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_STDIN)         /* //bpe/<id>/execs/0/stdin */        \
  DEF(VERTEX_FORMULA_STDOUT , "formula-stdout" , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_STDOUT)        /* //bpe/<id>/execs/0/stdout */       \
  DEF(VERTEX_FORMULA_STDERR , "formula-stderr" , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_STDERR)        /* //bpe/<id>/execs/0/stderr */       \
  DEF(VERTEX_FORMULA_AUXOUT , "formula-auxout" , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_AUXOUT)        /* //bpe/<id>/execs/0/auxout */       \
  DEF(VERTEX_FORMULA_ARGS   , "formula-args"   , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_ARGS)          /* //bpe/<id>/execs/0/args */         \
  DEF(VERTEX_FORMULA_ENVS   , "formula-envs"   , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_ENVS)          /* //bpe/<id>/execs/0/envs */         \
  DEF(VERTEX_FORMULA_FILE   , "formula-file"   , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_FILE)          /* //bpe/<id>/execs/0/file */         \
  DEF(VERTEX_FORMULA_CWD    , "formula-cwd"    , VERTEX_TYPE_OPT, VERTEX_KIND_EXEC | VERTEX_TYPE2_FORMULA | VERTEX_TYPE3_CWD)           /* //bpe/<id>/execs/0/cwd*/           \


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
#define VERTEX_STATE_TABLE                                                                                                                                       \
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
extern struct attrs32 *graph_filetype_attrs;          /* vertex filetype names */
extern struct attrs32 *graph_vertex_type_attrs;       /* vertex type names */
extern struct attrs32 *graph_vertex_state_attrs;      /* vertex state names */

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
