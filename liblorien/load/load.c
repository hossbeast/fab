/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator/load.h"

#include "load.internal.h"

//
// api
//

static int handles;

void API lorien_load()
{
  if(handles++ == 0)
  {
    narrator_load();
  }
}

void API lorien_unload()
{
  if(--handles == 0)
  {
    narrator_unload();
  }
}
