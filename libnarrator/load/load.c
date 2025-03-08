/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "load.h"
#include "fd.internal.h"
#include "nullity.internal.h"

static int handles;

//
// api
//

void narrator_load()
{
  if(handles++ == 0)
  {
    nullity_setup();
    fd_setup();
  }
}

void narrator_unload()
{
  if(--handles == 0)
  {
    nullity_cleanup();
    fd_cleanup();
  }
}
