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

#ifndef _FAB_GRAPH_H
#define _FAB_GRAPH_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;

/* embedded sub-graphs */
#define FAB_GRAPH_TABLE                                                                              \
  DEF(FAB_GRAPH_FSTREE          , "fs"          , 1) /* filesystem tree */                           \
  DEF(FAB_GRAPH_DIRTREE         , "dirtree"     , 2) /* fs, directories only */                      \
  DEF(FAB_GRAPH_MODTREE         , "modtree"     , 3) /* fs, modules only */                          \
  DEF(FAB_GRAPH_DEPENDS         , "depends"     , 4) /* A : B */                                     \
  DEF(FAB_GRAPH_MODULE_IMPORTS  , "imports"     , 5) /* module A imports directory B into scope */   \
  DEF(FAB_GRAPH_MODULE_USES     , "uses"        , 6) /* module A uses model B */                     \
  DEF(FAB_GRAPH_MODULE_REQUIRES , "requires"    , 7) /* module A requires module B */                \

typedef enum fab_graph {
#undef DEF
#define DEF(x, s, v) x = v,
FAB_GRAPH_TABLE
} fab_graph;

extern struct attrs16 * fab_graph_attrs;

#endif
