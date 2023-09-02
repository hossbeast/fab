/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/load.h"

#include "value.h"
#include "load.h"
#include "keywords.h"

static int handles;

void bacon_load()
{
  if(handles++ == 0)
  {
    // dependencies
    narrator_load();

    // modules
    keywords_setup();
  }
}

void bacon_unload()
{
  if(--handles == 0)
  {
    // modules
    keywords_cleanup();

    // dependencies
    narrator_unload();
  }
}
