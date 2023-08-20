/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "narrator/load.h"

#include "load.h"

static int handles;

//
// api
//

void API fab_load()
{
  if(handles++ == 0)
  {
    narrator_load();
  }
}

void API fab_unload()
{
  if(--handles == 0)
  {
    narrator_unload();
  }
}
