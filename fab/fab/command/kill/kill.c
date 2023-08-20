/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "kill.h"
#include "command.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"kill options\n"
  );
}

//
// public
//

struct command kill_command = {
    usage : usage
};
