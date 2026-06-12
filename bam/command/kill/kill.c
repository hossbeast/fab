/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>


#include "kill.h"
#include "command.h"
#include "args.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"bam kill - kill the bamd instance if any\n"
"\n"
"usage: bam kill\n"
  );
}

//
// public
//

typeof(kill_command) kill_command = {
    name : "kill"
  , usage : usage
};

void kill_init()
{
  g_args.kill = true;
}
