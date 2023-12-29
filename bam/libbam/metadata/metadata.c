/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "metadata.h"
#include "common/attrs.h"

attrs16 * APIDATA bam_system_state_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s, v) + 1
    num : 0
      BAM_SYSTEM_STATE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : x, range : 0xff },
BAM_SYSTEM_STATE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(bam_system_state_attrs);
}
