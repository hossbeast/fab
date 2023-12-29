/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

/* embedded sub-graphs */
#define BAM_GRAPH_TABLE                                                                              \
  DEF(BAM_GRAPH_FSTREE          , "fs"          , 1) /* filesystem tree */                           \
  DEF(BAM_GRAPH_FSTREE_HIDDEN   , "fs-hidden"   , 2) /* fs, including uncreated/unlinked */          \
  DEF(BAM_GRAPH_DIRTREE         , "dirtree"     , 3) /* fs, directories only */                      \
  DEF(BAM_GRAPH_RELATION        , "relation"    , 4) /* */                                           \
  DEF(BAM_GRAPH_MODTREE         , "modtree"     , 5) /* fs, modules only */                          \
  DEF(BAM_GRAPH_DEPENDS         , "depends"     , 6) /* A : B */                                     \
  DEF(BAM_GRAPH_MODULE_IMPORTS  , "imports"     , 7) /* module A imports directory B into scope */   \
  DEF(BAM_GRAPH_MODULE_USES     , "uses"        , 8) /* module A uses module B */                    \
  DEF(BAM_GRAPH_MODULE_REQUIRES , "requires"    , 9) /* module A requires module B */                \

typedef enum bam_graph {
#undef DEF
#define DEF(x, s, v) x = v,
BAM_GRAPH_TABLE
} bam_graph;

extern struct attrs16 * bam_graph_attrs;
