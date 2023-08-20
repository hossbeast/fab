/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "value.h"
#include "type.internal.h"

#include "common/attrs.h"

APIDATA attrs32 * value_type_attrs = (attrs32[]){{
#define VALUE_TYPE_DEF(x, r, y) + 1
    num : 0 VALUE_TYPE_TABLE
  , members : (member32[]) {
#undef VALUE_TYPE_DEF
#define VALUE_TYPE_DEF(x, r, y) { name : #x, value : UINT32_C(y), range : UINT32_C(r) },
VALUE_TYPE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(value_type_attrs);
}
