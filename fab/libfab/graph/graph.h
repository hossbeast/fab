/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

/* embedded sub-graphs */
#define FAB_GRAPH_TABLE                                                                              \
  DEF(FAB_GRAPH_FSTREE          , "fs"          , 1) /* filesystem tree */                           \
  DEF(FAB_GRAPH_FSTREE_HIDDEN   , "fs-hidden"   , 2) /* fs, including uncreated/unlinked */          \
  DEF(FAB_GRAPH_DIRTREE         , "dirtree"     , 3) /* fs, directories only */                      \
  DEF(FAB_GRAPH_RELATION        , "relation"    , 4) /* */                                           \
  DEF(FAB_GRAPH_MODTREE         , "modtree"     , 5) /* fs, modules only */                          \
  DEF(FAB_GRAPH_DEPENDS         , "depends"     , 6) /* A : B */                                     \
  DEF(FAB_GRAPH_MODULE_IMPORTS  , "imports"     , 7) /* module A imports directory B into scope */   \
  DEF(FAB_GRAPH_MODULE_USES     , "uses"        , 8) /* module A uses module B */                    \
  DEF(FAB_GRAPH_MODULE_REQUIRES , "requires"    , 9) /* module A requires module B */                \

typedef enum fab_graph {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_GRAPH_TABLE
} fab_graph;

extern struct attrs16 * fab_graph_attrs;
