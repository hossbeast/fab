/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

#define FAB_VERTEX_SHADOW  0x100    /* fsent in the shadow fs */
#define FAB_VERTEX_SPECIAL 0x200    /* not an fsent */

/* all of the different kinds of vertex vertices which can exist in the graph */
#define FAB_VERTEX_TYPE_OPT                              0xfff
#define FAB_VERTEX_TYPE_TABLE                                                                          \
  DEF(FAB_VERTEX_TYPE_DIR            , "dir"           , 0x001) /* directory */                        \
  DEF(FAB_VERTEX_TYPE_FILE           , "file"          , 0x002) /* regular file */                     \
  DEF(FAB_VERTEX_TYPE_MODULE_FILE    , "module-file"   , 0x003) /* module.bam */                       \
  DEF(FAB_VERTEX_TYPE_MODULE_DIR     , "module-dir"    , 0x004) /* module directory */                 \
  DEF(FAB_VERTEX_TYPE_MODULE         , "module"        , 0x204) /* module vertex */                    \
  DEF(FAB_VERTEX_TYPE_RULES_FILE     , "rules-file"    , 0x005) /* rules.bam */                        \
  DEF(FAB_VERTEX_TYPE_RULES          , "rules"         , 0x205) /* rules vertex */                     \
  DEF(FAB_VERTEX_TYPE_FORMULA_FILE   , "formula-file"  , 0x006) /* e.g. cc.bam */                      \
  DEF(FAB_VERTEX_TYPE_FORMULA        , "formula"       , 0x206) /* formula vertex */                   \
  DEF(FAB_VERTEX_TYPE_VAR_FILE       , "var-file"      , 0x007) /* var.bam */                          \
  DEF(FAB_VERTEX_TYPE_VAR            , "var"           , 0x207) /* var vertex */                       \
  DEF(FAB_VERTEX_TYPE_CONFIG_FILE    , "config-file"   , 0x008) /* config file */                      \
  DEF(FAB_VERTEX_TYPE_CONFIG         , "config"        , 0x208) /* config vertex */                    \
  DEF(FAB_VERTEX_TYPE_SHADOW_DIR     , "shadow-dir"    , 0x109) /* shadow fs directory */              \
  DEF(FAB_VERTEX_TYPE_SHADOW_FILE    , "shadow-file"   , 0x10a) /* shadow fs regular file */           \
  DEF(FAB_VERTEX_TYPE_SHADOW_LINK    , "shadow-link"   , 0x10b) /* shadow fs symlink */                \
  DEF(FAB_VERTEX_TYPE_SHADOW_MODULE  , "shadow-module" , 0x10c) /* shadow fs //modules/id */           \
  DEF(FAB_VERTEX_TYPE_SHADOW_RULE    , "shadow-rule"   , 0x10e) /* shadow fs //modules/id/rules/id */  \

typedef enum fab_vertex_type {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_VERTEX_TYPE_TABLE
} fab_vertex_type;

extern struct attrs16 * fab_vertex_type_attrs;

#define FAB_VERTEX_STATE_TABLE                                                   \
  DEF(FAB_VERTEX_STATE_FRESH     , "fresh"     , 1)   /* up-to-date */           \
  DEF(FAB_VERTEX_STATE_STALE     , "stale"     , 2)   /* needs to be updated */  \
  DEF(FAB_VERTEX_STATE_UNCREATED , "uncreated" , 3)   /* not yet created */      \
  DEF(FAB_VERTEX_STATE_UNLINKED  , "unlinked"  , 4)   /* deleted from fs */      \

typedef enum fab_vertex_state {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_VERTEX_STATE_TABLE
} fab_vertex_state;

extern struct attrs16 * fab_vertex_state_attrs;
