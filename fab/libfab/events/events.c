/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "events.h"
#include "common/attrs.h"

attrs32 * APIDATA fabipc_event_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, y) + 1
    num : 0
      FABIPC_EVENT_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT32_C(y), range : UINT32_C(y) },
FABIPC_EVENT_TYPE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(fabipc_event_type_attrs);
}
