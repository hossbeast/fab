/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "events.h"
#include "common/attrs.h"

attrs32 bamipc_event_type_attrs = {
#undef DEF
#define DEF(x, s, y) + 1
    num : 0
      BAMIPC_EVENT_BACON_TABLE
      BAMIPC_EVENT_BINARY_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : y, range : 0xffffffff },
BAMIPC_EVENT_BACON_TABLE
BAMIPC_EVENT_BINARY_TABLE
#undef DEF
  }
};

attrs32 bamipc_event_mask_attrs = {
#undef DEF
#define DEF(x, s, y) + 1
    num : 0
      BAMIPC_EVENT_BACON_TABLE
      BAMIPC_EVENT_BINARY_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : y, range : y },
BAMIPC_EVENT_BACON_TABLE
BAMIPC_EVENT_BINARY_TABLE
#undef DEF
  }
};

static void __attribute__((constructor)) init()
{
  attrs32_init(&bamipc_event_type_attrs);
  attrs32_init(&bamipc_event_mask_attrs);
}
