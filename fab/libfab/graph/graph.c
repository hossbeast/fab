/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "graph.h"
#include "common/attrs.h"

attrs16 * APIDATA fab_graph_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s, v) + 1
    num : 0
      FAB_GRAPH_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : x, range : 0xff },
FAB_GRAPH_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(fab_graph_attrs);
}
