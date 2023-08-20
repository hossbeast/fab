/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/load.h"

#include "value.h"
#include "load.internal.h"
#include "say.internal.h"
#include "writer.internal.h"

static int handles;

void API bacon_load()
{
  if(handles++ == 0)
  {
    // dependencies
    narrator_load();

    // modules
    say_setup();
    writer_setup();
  }
}

void API bacon_unload()
{
  if(--handles == 0)
  {
    // modules
    say_cleanup();
    writer_cleanup();

    // dependencies
    narrator_unload();
  }
}
