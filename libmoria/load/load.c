/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/load.h"


#include "load.h"

//
// api
//

static int handles;

void moria_load()
{
  if(handles++ == 0)
  {
    narrator_load();
  }
}

void moria_unload()
{
  if(--handles == 0)
  {
    narrator_unload();
  }
}
