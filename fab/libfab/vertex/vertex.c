/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "vertex.h"
#include "common/attrs.h"

attrs16 * APIDATA fab_vertex_type_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s, v) + 1
    num : 0
      FAB_VERTEX_TYPE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : x, range : FAB_VERTEX_TYPE_OPT },
FAB_VERTEX_TYPE_TABLE
#undef DEF
  }
}};

attrs16 * APIDATA fab_vertex_state_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s, v) + 1
    num : 0
      FAB_VERTEX_STATE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : x, range : 0xff },
FAB_VERTEX_STATE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(fab_vertex_type_attrs);
  attrs16_init(fab_vertex_state_attrs);
}
