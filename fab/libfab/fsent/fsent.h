/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

#define FAB_FSENT_SHADOW  0x100

/* all of the different kinds of fsent vertices which can exist in the graph */
#define FAB_FSENT_TYPE_TABLE                                                                \
  DEF(FAB_FSENT_TYPE_DIR            , "dir"           , 0x001) /* directory */              \
  DEF(FAB_FSENT_TYPE_FILE           , "file"          , 0x002) /* regular file */           \
  DEF(FAB_FSENT_TYPE_MODULE_DIR     , "module-dir"    , 0x003) /* module directory */       \
  DEF(FAB_FSENT_TYPE_MODULE_FILE    , "module-file"   , 0x004) /* module.bam */             \
  DEF(FAB_FSENT_TYPE_MODEL_FILE     , "model-file"    , 0x005) /* model.bam */              \
  DEF(FAB_FSENT_TYPE_FORMULA_FILE   , "formula-file"  , 0x006) /* e.g. cc.bam */            \
  DEF(FAB_FSENT_TYPE_VAR_FILE       , "var-file"      , 0x007) /* var.bam */                \
  DEF(FAB_FSENT_TYPE_CONFIG_FILE    , "config-file"   , 0x008) /* config file */            \
  DEF(FAB_FSENT_TYPE_SHADOW_DIR     , "shadow-dir"    , 0x009) /* shadow fs directory */    \
  DEF(FAB_FSENT_TYPE_SHADOW_FILE    , "shadow-file"   , 0x101) /* shadow fs regular file */ \
  DEF(FAB_FSENT_TYPE_SHADOW_LINK    , "shadow-link"   , 0x101) /* shadow fs symlink */      \
  DEF(FAB_FSENT_TYPE_SHADOW_MODULE  , "shadow-module" , 0x101) /* shadow fs //module */     \
  DEF(FAB_FSENT_TYPE_SHADOW_MODULES , "shadow-modules", 0x101) /* shadow fs //modules */    \

typedef enum fab_fsent_type {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_FSENT_TYPE_TABLE
} fab_fsent_type;

extern struct attrs16 * fab_fsent_type_attrs;

#define FAB_FSENT_STATE_TABLE                                                   \
  DEF(FAB_FSENT_STATE_FRESH     , "fresh"     , 1)   /* up-to-date */           \
  DEF(FAB_FSENT_STATE_STALE     , "stale"     , 2)   /* needs to be updated */  \
  DEF(FAB_FSENT_STATE_UNCREATED , "uncreated" , 3)   /* not yet created */      \
  DEF(FAB_FSENT_STATE_UNLINKED  , "unlinked"  , 4)   /* deleted from fs */      \

typedef enum fab_fsent_state {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_FSENT_STATE_TABLE
} fab_fsent_state;

extern struct attrs16 * fab_fsent_state_attrs;
