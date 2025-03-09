/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/load.h"

#include "value.h"
#include "load.h"
#include "registry.h"

static int handles;

void registry_setup(void);
void registry_cleanup(void);

void bacon_load()
{
  if(handles++ == 0)
  {
    // dependencies
    narrator_load();

    // modules
    registry_setup();
  }
}

void bacon_unload()
{
  if(--handles == 0)
  {
    // modules
    registry_cleanup();

    // dependencies
    narrator_unload();
  }
}
