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

#ifndef _FAB_FSENT_H
#define _FAB_FSENT_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

#define FAB_FSENT_SHADOW  0x100

/* all of the different kinds of fsent vertices which can exist in the graph */
#define FAB_FSENT_TYPE_TABLE                                                                        \
  DEF(FAB_FSENT_TYPE_DIR                    , "dir"           , 0x001)  /* directory */             \
  DEF(FAB_FSENT_TYPE_FILE                   , "file"          , 0x002)  /* regular file */          \
  DEF(FAB_FSENT_TYPE_MODULE_DIR             , "module-dir"    , 0x003)  /* module directory */      \
  DEF(FAB_FSENT_TYPE_MODULE_FILE            , "module-file"   , 0x004)  /* module.bam */            \
  DEF(FAB_FSENT_TYPE_MODEL_FILE             , "model-file"    , 0x005)  /* model.bam */             \
  DEF(FAB_FSENT_TYPE_FORMULA_FILE           , "formula-file"  , 0x006)  /* e.g. cc.bam */           \
  DEF(FAB_FSENT_TYPE_VAR_FILE               , "var-file"      , 0x007)  /* var.bam */               \
  DEF(FAB_FSENT_TYPE_CONFIG_FILE            , "config-file"   , 0x008)  /* config file */           \
  DEF(FAB_FSENT_TYPE_SHADOW_DIR             , "shadow-dir"    , 0x009)  /* shadow fs directory */   \
  DEF(FAB_FSENT_TYPE_SHADOW_FILE            , "shadow-file"   , 0x101) /* shadow fs regular file */ \
  DEF(FAB_FSENT_TYPE_SHADOW_LINK            , "shadow-link"   , 0x101) /* shadow fs symlink */      \
  DEF(FAB_FSENT_TYPE_SHADOW_MODULE          , "shadow-module" , 0x101) /* shadow fs //module */     \
  DEF(FAB_FSENT_TYPE_SHADOW_MODULES         , "shadow-modules", 0x101) /* shadow fs //modules */    \

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

#endif
