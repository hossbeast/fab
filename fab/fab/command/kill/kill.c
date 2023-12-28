/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "bam/client.h"

#include "kill.h"
#include "command.h"
#include "args.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"kill options (none)\n"
  );
}

static void attached(command * restrict cmd, bam_client * restrict client)
{
}
static void disconnected(command * restrict cmd)
{
}
static void init(command * restrict cmd, uint32_t flags)
{
  g_args.kill = true;
}

//
// public
//

struct command kill_command = {
    usage : usage
  , init : init
  , attached : attached
  , disconnected : disconnected
};
