/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "command.h"

struct attrs32 command_attrs = (attrs32) {
#undef DEF
#define DEF(x, y) + 1
    num : 0
      COMMAND_ATTRS_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, y) { name : #x, value : y, range : COMMAND_OPT },
COMMAND_ATTRS_TABLE
#undef DEF
  }
};
